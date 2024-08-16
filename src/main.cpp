/*Lot of ideas comes from here:
 * https://github.com/glitterkitty/EpEverSolarMonitor
 *
 */
#include <Arduino.h>
#include "main.h"

#include <EEPROM.h>
#include <PubSubClient.h>

#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWiFiManager.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <WebSerialLite.h>
#include <SoftwareSerial.h>

#include <RTCMemory.h>

#include "VeDirectFrameHandler.h"
#include "VeDirectDataList.h"
#include "VeDirectDeviceList.h"
#include "VeDirectDeviceCodes.h"
#include "Settings.h" //settings functions
#include <Updater.h>  //new

#include "html.h"          //the HTML content
#include "htmlProzessor.h" // The html Prozessor

String topic = ""; // Default first part of topic. We will add device ID in setup

// flag for saving data and other things
bool shouldSaveConfig = false;
bool restartNow = false;
bool workerCanRun = true;
bool dataProzessing = false;
bool haDiscTrigger = false;
bool haAutoDiscTrigger = false;
bool remoteControlState = false;
unsigned int jsonSize = 0;
unsigned long mqtttimer = 0;
unsigned long RestartTimer = 0;
byte wsReqInvNum = 1;
char mqtt_server[80];
char mqttClientId[80];
uint32_t bootcount = 0;

WiFiClient client;
Settings _settings;
PubSubClient mqttclient(client);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient *wsClient;
DNSServer dns;
VeDirectFrameHandler myve;
SoftwareSerial veSerial;
RTCMemory<rtcData> rtcMemory;

DynamicJsonDocument Json(JSON_BUFFER);
JsonObject jsonESP = Json.createNestedObject("ESP_Data");
#include "status-LED.h"
ADC_MODE(ADC_VCC);

//----------------------------------------------------------------------
void saveConfigCallback()
{
  writeLog("Should save config");
  shouldSaveConfig = true;
}

void notifyClients()
{
  if (wsClient != nullptr && wsClient->canSend())
  {
    size_t len = measureJson(Json);
    AsyncWebSocketMessageBuffer *buffer = ws.makeBuffer(len);
    if (buffer)
    {
      serializeJson(Json, (char *)buffer->get(), len + 1);
      wsClient->text(buffer);
    }
    writeLog("Data sent to WebSocket");
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{

  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    if (strcmp((char *)data, "A9") != 0)
    {
      if (strcmp((char *)data, "remotecontrol_on") == 0)
      {
        remoteControl(true);
      }
      if (strcmp((char *)data, "remotecontrol_off") == 0)
      {
        remoteControl(false);
      }
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    wsClient = client;
    writeLog("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    if (!dataProzessing /*&& wsClient != nullptr && wsClient->canSend()*/)
      notifyClients();

    break;
  case WS_EVT_DISCONNECT:
    writeLog("WebSocket client #%u disconnected\n", client->id());
    wsClient = nullptr;
    ws.cleanupClients();
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    wsClient = nullptr;
    ws.cleanupClients();
    break;
  }
}

void ReadVEData()
{
  while (veSerial.available())
  {
    myve.rxData(veSerial.read());
    esp_yield();
  }
  //    if (veSerial.available())
  // {
  //   myve.rxData(veSerial.read());
  // }
}

bool remoteControl(bool sw)
{
  writeLog("set Remote Control to: %d", sw);
  digitalWrite(MYPORT_TX, sw);
  rtcData *RTCmem = rtcMemory.getData();
  RTCmem->remoteControlState = sw;
  rtcMemory.save();
  if(_settings.data.keepRcState)
  {
    _settings.data.rcState = sw;
    _settings.save();
  }
  remoteControlState = sw;
  mqtttimer = 0;
  return remoteControlState;
}

void setup()
{
  DBG_BEGIN(DBG_BAUD);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);
  pinMode(MYPORT_TX, OUTPUT);

  if (!rtcMemory.begin())
  {
    remoteControlState = false;
  }
  rtcData *RTCmem = rtcMemory.getData();
  remoteControlState = RTCmem->remoteControlState;
  Serial.printf("bootcount is: %d \n", RTCmem->bootcount);
  if (ESP.getResetInfoPtr()->reason == 6)
  {
    RTCmem->bootcount++;
    if (RTCmem->bootcount >= 10)
    {
      _settings.reset();
      ESP.eraseConfig();
      Serial.println("reset called");
      ESP.restart();
    }
  }
  rtcMemory.save();
  _settings.load();
  if(_settings.data.keepRcState) remoteControlState = _settings.data.rcState;
  digitalWrite(MYPORT_TX, remoteControlState);
  
  haAutoDiscTrigger = _settings.data.haDiscovery;
  WiFi.persistent(true); // fix wifi save bug
  veSerial.begin(VICTRON_BAUD, SWSERIAL_8N1, MYPORT_RX /*, MYPORT_TX, false*/);
  veSerial.flush();
  veSerial.enableRxGPIOPullUp(false);
  myve.callback(prozessData);
  
  sprintf(mqttClientId, "%s-%06X", _settings.data.deviceName, ESP.getChipId());

  AsyncWiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT server", NULL, 32);
  AsyncWiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT User", NULL, 32);
  AsyncWiFiManagerParameter custom_mqtt_pass("mqtt_pass", "MQTT Password", NULL, 32);
  AsyncWiFiManagerParameter custom_mqtt_topic("mqtt_topic", "MQTT Topic", "Victron", 32);
  AsyncWiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", NULL, 6);
  AsyncWiFiManagerParameter custom_mqtt_refresh("mqtt_refresh", "MQTT Send Interval", "300", 4);
  AsyncWiFiManagerParameter custom_mqtt_triggerpath("mqtt_triggerpath", "MQTT Data Trigger Path", NULL, 80);
  AsyncWiFiManagerParameter custom_device_name("device_name", "Device Name", "Victron2MQTT", 32);

  AsyncWiFiManager wm(&server, &dns); // create wifimanager instance

  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_user);
  wm.addParameter(&custom_mqtt_pass);
  wm.addParameter(&custom_mqtt_topic);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_mqtt_refresh);
  wm.addParameter(&custom_mqtt_triggerpath);
  wm.addParameter(&custom_device_name);

  wm.setSaveConfigCallback(saveConfigCallback);
  wm.setConfigPortalTimeout(120); // auto close configportal after n seconds

  bool res = wm.autoConnect("Victron2MQTT-AP");

  // save settings if wifi setup is fire up
  if (shouldSaveConfig)
  {
    strncpy(_settings.data.mqttServer, custom_mqtt_server.getValue(), 40);
    strncpy(_settings.data.mqttUser, custom_mqtt_user.getValue(), 40);
    strncpy(_settings.data.mqttPassword, custom_mqtt_pass.getValue(), 40);
    _settings.data.mqttPort = atoi(custom_mqtt_port.getValue());
    strncpy(_settings.data.deviceName, custom_device_name.getValue(), 40);
    strncpy(_settings.data.mqttTopic, custom_mqtt_topic.getValue(), 40);
    _settings.data.mqttRefresh = atoi(custom_mqtt_refresh.getValue());
    strncpy(_settings.data.mqttTriggerPath, custom_mqtt_triggerpath.getValue(), 80);

    _settings.save();
    ESP.restart();
  }

  topic = _settings.data.mqttTopic;
  mqttclient.setServer(_settings.data.mqttServer, _settings.data.mqttPort);
  mqttclient.setCallback(mqttCallback);

  if (res)
  {
    // set the device name
    MDNS.begin(_settings.data.deviceName);
    MDNS.addService("http", "tcp", 80);
    WiFi.hostname(_settings.data.deviceName);

    Json["Device_name"] = _settings.data.deviceName;

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if(strlen(_settings.data.httpUser) > 0 && !request->authenticate(_settings.data.httpUser, _settings.data.httpPass)) return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_MAIN, htmlProcessor);
      request->send(response); });

    server.on("/livejson", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                if(strlen(_settings.data.httpUser) > 0 && !request->authenticate(_settings.data.httpUser, _settings.data.httpPass)) return request->requestAuthentication();
                AsyncResponseStream *response = request->beginResponseStream("application/json");
                serializeJson(Json, *response);
                request->send(response); });

    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                if(strlen(_settings.data.httpUser) > 0 && !request->authenticate(_settings.data.httpUser, _settings.data.httpPass)) return request->requestAuthentication();
                AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_REBOOT, htmlProcessor);
                request->send(response);
                restartNow = true;
                RestartTimer = millis(); });

    server.on("/confirmreset", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if(strlen(_settings.data.httpUser) > 0 && !request->authenticate(_settings.data.httpUser, _settings.data.httpPass)) return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_CONFIRM_RESET, htmlProcessor);
      request->send(response); });

    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                if(strlen(_settings.data.httpUser) > 0 && !request->authenticate(_settings.data.httpUser, _settings.data.httpPass)) return request->requestAuthentication();
                AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Device is Erasing...");
                response->addHeader("Refresh", "15; url=/");
                response->addHeader("Connection", "close");
                request->send(response);
                delay(500);
                _settings.reset();
                ESP.eraseConfig();
                ESP.reset(); });

    server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if(strlen(_settings.data.httpUser) > 0 && !request->authenticate(_settings.data.httpUser, _settings.data.httpPass)) return request->requestAuthentication();
      AsyncWebParameter *p = request->getParam(0);
      if (p->name() == "ha")
      {
        haDiscTrigger = true;
        }
      if (p->name() == "remotecontrol")
      {
        if(p->value().toInt() == 1){
          remoteControl(true);
        }
        if(p->value().toInt() == 0){
          remoteControl(false);
        }
      }


        request->send(200, "text/plain", "message received"); });

    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if(strlen(_settings.data.httpUser) > 0 && !request->authenticate(_settings.data.httpUser, _settings.data.httpPass)) return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_SETTINGS, htmlProcessor);
      request->send(response); });

    server.on("/settingsedit", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if(strlen(_settings.data.httpUser) > 0 && !request->authenticate(_settings.data.httpUser, _settings.data.httpPass)) return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_SETTINGS_EDIT, htmlProcessor);
      request->send(response); });

    server.on("/settingssave", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                if(strlen(_settings.data.httpUser) > 0 && !request->authenticate(_settings.data.httpUser, _settings.data.httpPass)) return request->requestAuthentication();
                strncpy(_settings.data.mqttServer, request->arg("post_mqttServer").c_str(), 40);
                _settings.data.mqttPort = request->arg("post_mqttPort").toInt();
                strncpy(_settings.data.mqttUser, request->arg("post_mqttUser").c_str(), 40);
                strncpy(_settings.data.mqttPassword, request->arg("post_mqttPassword").c_str(), 40);
                strncpy(_settings.data.mqttTopic, request->arg("post_mqttTopic").c_str(), 40);
                _settings.data.mqttRefresh = request->arg("post_mqttRefresh").toInt() < 1 ? 1 : request->arg("post_mqttRefresh").toInt(); // prevent lower numbers
                strncpy(_settings.data.deviceName, request->arg("post_deviceName").c_str(), 40);
                _settings.data.mqttJson = (request->arg("post_mqttjson") == "true") ? true : false;
                strncpy(_settings.data.mqttTriggerPath, request->arg("post_mqtttrigger").c_str(), 80);
                _settings.data.webUIdarkmode = (request->arg("post_webuicolormode") == "true") ? true : false;
                strncpy(_settings.data.httpUser, request->arg("post_httpUser").c_str(), 40);
                strncpy(_settings.data.httpPass, request->arg("post_httpPass").c_str(), 40);
                _settings.data.haDiscovery = (request->arg("post_hadiscovery") == "true") ? true : false;
                //_settings.data.debugmode = (request->arg("post_debugmode") == "true") ? true : false;
                _settings.data.keepRcState = (request->arg("post_keeprcstate") == "true") ? true : false;
                _settings.save();
                request->redirect("/reboot"); });

    server.on(
        "/update", HTTP_POST, [](AsyncWebServerRequest *request)
        {
    //https://gist.github.com/JMishou/60cb762047b735685e8a09cd2eb42a60
    // the request handler is triggered after the upload has finished... 
    // create the response, add header, and send response
    if(strlen(_settings.data.httpUser) > 0 && !request->authenticate(_settings.data.httpUser, _settings.data.httpPass)) return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError())?"FAIL":"OK");
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    //restartNow = true; // Tell the main loop to restart the ESP
    request->send(response); },
        [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
        {
          // Upload handler chunks in data

          if (!index)
          { // if index == 0 then this is the first frame of data
            Serial.printf("UploadStart: %s\n", filename.c_str());
            Serial.setDebugOutput(true);

            // calculate sketch space required for the update
            uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace))
            { // start with max available size
              Update.printError(Serial);
            }
            Update.runAsync(true); // tell the updaterClass to run in async mode
          }

          // Write chunked data to the free sketch space
          if (Update.write(data, len) != len)
          {
            Update.printError(Serial);
          }

          if (final)
          { // if the final flag is set then this is the last frame of data
            if (Update.end(true))
            { // true to set the size to the current progress
              Serial.printf("Update Success: %u B\nRebooting...\n", index + len);
            }
            else
            {
              Update.printError(Serial);
            }
            Serial.setDebugOutput(false);
          }
        });

    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(418, "text/plain", "418 I'm a teapot"); });

    ws.onEvent(onEvent);
    server.addHandler(&ws);

    // WebSerial is accessible at "<IP Address>/webserial" in browser
    WebSerial.begin(&server);

    server.begin();
    //MDNS.addService("http", "tcp", 80);
    //MDNS.update();

    jsonESP["IP"] = WiFi.localIP();
    jsonESP["sw_version"] = SOFTWARE_VERSION;
  }
  analogWrite(LED_PIN, 255);
  RTCmem->bootcount = 0;
  rtcMemory.save();
}

void loop()
{
MDNS.update();
  if (Update.isRunning())
  {
    workerCanRun = false;
  }
  if (workerCanRun)
  {
    ReadVEData();

    // Make sure wifi is in the right mode
    if (WiFi.status() == WL_CONNECTED)
    {                      // No use going to next step unless WIFI is up and running.
      ws.cleanupClients(); // clean unused client connections
      //MDNS.update();
      if (millis() - mqtttimer > (_settings.data.mqttRefresh * 1000) || mqtttimer == 0)
      {
        writeLog("<MQTT> Data Send...");
        sendtoMQTT(); // Update data to MQTT server if we should
        mqtttimer = millis();
      }

      mqttclient.loop(); // Check if we have something to read from MQTT
    }
    notificationLED(); // notification LED routine

    if ((haDiscTrigger || _settings.data.haDiscovery) && measureJson(Json) > jsonSize)
    {
      if (sendHaDiscovery())
      {
        haDiscTrigger = false;
        jsonSize = measureJson(Json);
      }
    }
  }

  if (restartNow && millis() >= (RestartTimer + 500))
  {
    writeLog("Restart");
    ESP.reset();
  }
}

void prozessData()
{
  dataProzessing = true;
  writeLog("VE callback triggered... prozessing data");
  getJsonData();
  notifyClients();
  dataProzessing = false;
}

bool getJsonData()
{
  jsonESP["ESP_VCC"] = (ESP.getVcc() / 1000.0) + 0.3;
  jsonESP["Wifi_RSSI"] = WiFi.RSSI();
  jsonESP["Flash_Size"] = ESP.getFlashChipSize();
  jsonESP["Sketch_Size"] = ESP.getSketchSize();
  jsonESP["Free_Sketch_Space"] = ESP.getFreeSketchSpace();
  jsonESP["Real_Flash_Size"] = ESP.getFlashChipRealSize();
  jsonESP["Free_Heap"] = ESP.getFreeHeap();
  jsonESP["HEAP_Fragmentation"] = ESP.getHeapFragmentation();
  jsonESP["WS_Clients"] = ws.count();
  jsonESP["Free_BlockSize"] = ESP.getMaxFreeBlockSize();
  jsonESP[F("Runtime")] = millis() / 1000;
  writeLog("VE data: %d:%d:%d", myve.veEnd, myve.veErrorCount, myve.veError);
  for (size_t i = 0; i < myve.veEnd; i++)
  {

    if (myve.veName[i] == NULL || strlen(myve.veName[i]) == 0 || myve.veValue[i] == NULL || strlen(myve.veValue[i]) == 0)
    {
      i = myve.veEnd;
      break;
    }
    // writeLog("[%s:%s]",myve.veName[i], myve.veValue[i]);
    //  search for every Vevalue in the list and replace it with clear name
    for (size_t j = 0; j < sizeof(VePrettyData) / sizeof(VePrettyData[0]); j++)
    {
      if (strcmp(VePrettyData[j][0], myve.veName[i]) == 0) // search the real descriptor in the array
      {
        // check if we have a data operator
        if (strlen(VePrettyData[j][2]) > 0 && strcmp(VePrettyData[j][2], "0") != 0)
        {
          Json[FPSTR(VePrettyData[j][1])] = (int)((atof(myve.veValue[i]) / atoi(VePrettyData[j][2])) * 100 + 0.5) / 100.0;
        }
        else if (strcmp(VePrettyData[j][2], "0") == 0)
        {
          Json[FPSTR(VePrettyData[j][1])] = atoi(myve.veValue[i]);
        }
        else
        {
          Json[FPSTR(VePrettyData[j][1])] = myve.veValue[i];
        }

        // if the Name Device_Model, search in the list for the device code
        if (strcmp(VePrettyData[j][1], "Device_model") == 0)
        {
          for (size_t k = 0; k < sizeof(VeDirectDeviceList) / sizeof(VeDirectDeviceList[0]); k++)
          {
            if (strcmp(VeDirectDeviceList[k][0], myve.veValue[i]) == 0)
            {
              Json[FPSTR(VePrettyData[j][1])] = FPSTR(VeDirectDeviceList[k][1]);
              break;
            }
          }
        }

        // if the Name AR - Alarm_code, search in the list for the device code
        if (strcmp(VePrettyData[j][1], "Alarm_code") == 0)
        {
          for (size_t k = 0; k < sizeof(VeDirectDeviceCodeAR) / sizeof(VeDirectDeviceCodeAR[0]); k++)
          {
            if (strcmp(VeDirectDeviceCodeAR[k][0], myve.veValue[i]) == 0)
            {
              Json[FPSTR(VePrettyData[j][1])] = FPSTR(VeDirectDeviceCodeAR[k][1]);
              break;
            }
          }
        }
        // if the Name OR - Off_reason, search in the list for the device code
        if (strcmp(VePrettyData[j][1], "Off_reason") == 0)
        {
          for (size_t k = 0; k < sizeof(VeDirectDeviceCodeOR) / sizeof(VeDirectDeviceCodeOR[0]); k++)
          {
            if (strcmp(VeDirectDeviceCodeOR[k][0], myve.veValue[i]) == 0)
            {
              Json[FPSTR(VePrettyData[j][1])] = FPSTR(VeDirectDeviceCodeOR[k][1]);
              break;
            }
          }
        }
        // if the Name CS - Operation_state, search in the list for the device code
        if (strcmp(VePrettyData[j][1], "Operation_state") == 0)
        {
          for (size_t k = 0; k < sizeof(VeDirectDeviceCodeCS) / sizeof(VeDirectDeviceCodeCS[0]); k++)
          {
            if (strcmp(VeDirectDeviceCodeCS[k][0], myve.veValue[i]) == 0)
            {
              Json[FPSTR(VePrettyData[j][1])] = FPSTR(VeDirectDeviceCodeCS[k][1]);
              break;
            }
          }
        }
        // if the Name ERR - Current_error, search in the list for the device code
        if (strcmp(VePrettyData[j][1], "Current_error") == 0)
        {
          for (size_t k = 0; k < sizeof(VeDirectDeviceCodeERR) / sizeof(VeDirectDeviceCodeERR[0]); k++)
          {
            if (strcmp(VeDirectDeviceCodeERR[k][0], myve.veValue[i]) == 0)
            {
              Json[FPSTR(VePrettyData[j][1])] = FPSTR(VeDirectDeviceCodeERR[k][1]);
              break;
            }
          }
        }
        // if the Name MPPT - Tracker_operation_mode, search in the list for the device code
        if (strcmp(VePrettyData[j][1], "Tracker_operation_mode") == 0)
        {
          for (size_t k = 0; k < sizeof(VeDirectDeviceCodeMPPT) / sizeof(VeDirectDeviceCodeMPPT[0]); k++)
          {
            if (strcmp(VeDirectDeviceCodeMPPT[k][0], myve.veValue[i]) == 0)
            {
              Json[FPSTR(VePrettyData[j][1])] = FPSTR(VeDirectDeviceCodeMPPT[k][1]);
              break;
            }
          }
        }
        break; // if we have found and prozessed the data, break the loop
      }
    }
    Json["Device_connection"] = myve.veError ? "Disconnected" : "Connected";
    Json["Remote_Control_State"] = remoteControlState;
  }
  return true;
}

bool connectMQTT()
{
  if (!mqttclient.connected())
  {
    if (mqttclient.connect(mqttClientId, _settings.data.mqttUser, _settings.data.mqttPassword, (topic + "/Alive").c_str(), 0, true, "false", true))
    {
      mqttclient.publish((topic + String("/IP")).c_str(), String(WiFi.localIP().toString()).c_str());
      mqttclient.publish((topic + String("/Alive")).c_str(), "true", true); // LWT online message must be retained!
      mqttclient.subscribe((topic + "/Remote_Control").c_str());

      if (strlen(_settings.data.mqttTriggerPath) > 0)
      {
        writeLog("MQTT Data Trigger Subscribed");
        mqttclient.subscribe(_settings.data.mqttTriggerPath);
      }
      return true;
    }
    else
    {
      return false;
    }
    return false;
  }
  else
  {
    return true;
  }
}

bool sendtoMQTT()
{

  if (!connectMQTT())
  {
    return false;
  }

  String mqttDeviceName = topic;

  //-----------------------------------------------------
  mqttclient.publish((mqttDeviceName + String("/Alive")).c_str(), "true", true); // LWT online message must be retained!
  mqttclient.publish((mqttDeviceName + String("/Wifi_RSSI")).c_str(), String(WiFi.RSSI()).c_str());
  mqttclient.publish((mqttDeviceName + String("/Remote_Control_State")).c_str(), remoteControlState ? "true" : "false");
  if (!_settings.data.mqttJson)
  {

    for (JsonPair i : Json.as<JsonObject>())
    {
      mqttclient.publish((mqttDeviceName + "/" + i.key().c_str()).c_str(), i.value().as<String>().c_str());
    }
  }
  else
  {
    mqttclient.beginPublish((String(mqttDeviceName + "/DATA")).c_str(), measureJson(Json), false);
    serializeJson(Json, mqttclient);
    mqttclient.endPublish();
  }

  return true;
}

void mqttCallback(char *top, byte *payload, unsigned int length) // Need rework
{
  String messageTemp;
  // updateProgress = true; // stop servicing data
//  if (!_settings.data.mqttJson)
 // {

    for (unsigned int i = 0; i < length; i++)
    {
      messageTemp += (char)payload[i];
    }
 // }
 // else
 // {
 //   StaticJsonDocument<1024> mqttJsonAnswer;
 //   deserializeJson(mqttJsonAnswer, (const byte *)payload, length);
 // }

  if (strlen(_settings.data.mqttTriggerPath) > 0 && strcmp(top, _settings.data.mqttTriggerPath) == 0)
  {
    writeLog("MQTT Data Trigger Firered Up");
    mqtttimer = 0;
  }
  if (strcmp(top, (topic + "/Remote_Control").c_str()) == 0)
  {
    if (messageTemp == "true")
    {
      mqtttimer = 0;
      remoteControl(true);
    }
    if (messageTemp == "false")
    {
      mqtttimer = 0;
      remoteControl(false);
    }
  }
}

bool sendHaDiscovery()
{
  if (!connectMQTT())
  {
    return false;
  }
  String haDeviceDescription = String("\"dev\":") +
                               "{\"ids\":[\"" + mqttClientId + "\"]," +
                               "\"name\":\"" + _settings.data.deviceName + "\"," +
                               "\"cu\":\"http://" + WiFi.localIP().toString() + "\"," +
                               "\"mdl\":\"" + Json["Device_model"].as<String>().c_str() + "\"," +
                               "\"mf\":\"SoftWareCrash\"," +
                               "\"sw\":\"" + SOFTWARE_VERSION + "\"" +
                               "}";

  char topBuff[128];
  // char configBuff[1024];
  // size_t mqttContentLength;
  for (size_t i = 0; i < sizeof haDescriptor / sizeof haDescriptor[0]; i++)
  {
    if (Json.containsKey(haDescriptor[i][0]))
    {
      String haPayLoad = String("{") +
                         "\"name\":\"" + haDescriptor[i][0] + "\"," +
                         "\"stat_t\":\"" + _settings.data.mqttTopic + "/" + haDescriptor[i][0] + "\"," +
                         "\"avty_t\":\"" + _settings.data.mqttTopic + "/Alive\"," +
                         "\"pl_avail\": \"true\"," +
                         "\"pl_not_avail\": \"false\"," +
                         "\"uniq_id\":\"" + mqttClientId + "." + haDescriptor[i][0] + "\"," +
                         "\"ic\":\"mdi:" + haDescriptor[i][1] + "\",";
      if (strlen(haDescriptor[i][2]) != 0)
        haPayLoad += (String) "\"unit_of_meas\":\"" + haDescriptor[i][2] + "\",";

      if (strcmp(haDescriptor[i][2], "kWh") == 0 || strcmp(haDescriptor[i][2], "Wh") == 0)
        haPayLoad += (String) "\"state_class\":\"total\",";
      if (strcmp(haDescriptor[i][2], "A") == 0 || strcmp(haDescriptor[i][2], "V") == 0 || strcmp(haDescriptor[i][2], "W") == 0)
        haPayLoad += (String) "\"state_class\":\"measurement\",";

      if (strlen(haDescriptor[i][3]) != 0)
        haPayLoad += (String) "\"dev_cla\":\"" + haDescriptor[i][3] + "\",";
      haPayLoad += haDeviceDescription;
      haPayLoad += "}";
      sprintf(topBuff, "homeassistant/sensor/%s/%s/config", _settings.data.mqttTopic, haDescriptor[i][0]); // build the topic
      mqttclient.beginPublish(topBuff, haPayLoad.length(), true);
      for (size_t i = 0; i < haPayLoad.length(); i++)
      {
        mqttclient.write(haPayLoad[i]);
      }
      mqttclient.endPublish();
    }
  }

  // switch
  String haPayLoad = String("{") +
                     "\"name\":\"Remote_Control\"," +
                     "\"command_topic\":\"" + _settings.data.mqttTopic + "/Remote_Control\"," +
                     "\"stat_t\":\"" + _settings.data.mqttTopic + "/Remote_Control_State\"," +
                     "\"uniq_id\":\"" + mqttClientId + ".Remote_Control\"," +
                     "\"avty_t\":\"" + _settings.data.mqttTopic + "/Alive\"," +
                     "\"pl_avail\": \"true\"," +
                     "\"pl_not_avail\": \"false\"," +
                     "\"ic\":\"mdi:toggle-switch-off\"," +
                     "\"pl_on\":\"true\"," +
                     "\"pl_off\":\"false\"," +
                     "\"stat_on\":\"true\"," +
                     "\"stat_off\":\"false\",";

  haPayLoad += haDeviceDescription;
  haPayLoad += "}";
  sprintf(topBuff, "homeassistant/switch/%s/%s/config", _settings.data.mqttTopic, "Remote_Control"); // build the topic

  mqttclient.beginPublish(topBuff, haPayLoad.length(), true);
  for (size_t i = 0; i < haPayLoad.length(); i++)
  {
    mqttclient.write(haPayLoad[i]);
  }
  mqttclient.endPublish();

  return true;
}

void writeLog(const char *format, ...)
{
  char msg[256];
  va_list args;

  va_start(args, format);
  vsnprintf(msg, sizeof(msg), format, args); // do check return value
  va_end(args);

  // write msg to the log
   DBG_PRINTLN(msg);
   DBG_WEBLN(msg);
}