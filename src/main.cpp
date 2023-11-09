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

DynamicJsonDocument Json(JSON_BUFFER);
// StaticJsonDocument <JSON_BUFFER>Json;
JsonObject jsonESP = Json.createNestedObject("ESP_Data");
#include "status-LED.h"
ADC_MODE(ADC_VCC);
//----------------------------------------------------------------------
void saveConfigCallback()
{
  DEBUG_WEBLN(F("Should save config"));
  shouldSaveConfig = true;
}

void notifyClients()
{
  if (wsClient != nullptr && wsClient->canSend())
  {
    DEBUG_WEB(F("<WEBS> Data sent to WebSocket... "));
    size_t len = measureJson(Json);
    AsyncWebSocketMessageBuffer *buffer = ws.makeBuffer(len);
    if (buffer)
    {
      serializeJson(Json, (char *)buffer->get(), len + 1);
      wsClient->text(buffer);
    }
    DEBUG_WEBLN(F("Done"));
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{

  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    /*
    data[len] = 0;
    updateProgress = true;
    if (String((char *)data).substring(0, 9) == "wsSelInv_") // get the inverter number to display on the web
    {
      wsReqInvNum = String((char *)data).substring(9, 10).toInt();
    }
    if (String((char *)data).substring(0, 11) == "loadSwitch_") // get switch data from web loadSwitch_1_1
    {
      epnode.setSlaveId(String((char *)data).substring(11, 12).toInt());
      epnode.writeSingleCoil(0x0002, String((char *)data).substring(13, 14).toInt());
    }
    updateProgress = false;
    */
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    wsClient = client;
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    DEBUG_WEBF("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    if (!dataProzessing /*&& wsClient != nullptr && wsClient->canSend()*/)
      notifyClients();

    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    DEBUG_WEBF("WebSocket client #%u disconnected\n", client->id());
    wsClient = nullptr;
    ws.cleanupClients();
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

bool resetCounter(bool count)
{

  if (count)
  {
    if (ESP.getResetInfoPtr()->reason == 6)
    {
      ESP.rtcUserMemoryRead(16, &bootcount, sizeof(bootcount));

      if (bootcount >= 10 && bootcount < 20)
      {
        // bootcount = 0;
        // ESP.rtcUserMemoryWrite(16, &bootcount, sizeof(bootcount));
        _settings.reset();
        ESP.eraseConfig();
        ESP.restart();
      }
      else
      {
        bootcount++;
        ESP.rtcUserMemoryWrite(16, &bootcount, sizeof(bootcount));
      }
    }
    else
    {
      bootcount = 0;
      ESP.rtcUserMemoryWrite(16, &bootcount, sizeof(bootcount));
    }
  }
  else
  {
    bootcount = 0;
    ESP.rtcUserMemoryWrite(16, &bootcount, sizeof(bootcount));
  }
  return true;
}

void ReadVEData()
{
  while (veSerial.available())
  {
    myve.rxData(veSerial.read());
    esp_yield();
  }
  // if (veSerial.available())
  //{
  //   myve.rxData(veSerial.read());
  // }
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);
  digitalWrite(MYPORT_TX, 0);
  resetCounter(true);
  _settings.load();
  WiFi.persistent(true); // fix wifi save bug
  // AsyncWiFiManager wm(&server, &dns); // create wifimanager instance

  veSerial.begin(VICTRON_BAUD, SWSERIAL_8N1, MYPORT_RX /*, MYPORT_TX, false*/);
  veSerial.flush();
  veSerial.enableRxGPIOPullUp(false);
  myve.callback(prozessData);

  Serial.begin(DEBUG_BAUD);

  sprintf(mqttClientId, "%s-%06X", _settings.data.deviceName, ESP.getChipId());

  // https://github.com/alanswx/ESPAsyncWiFiManager/issues/72

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
  // wm.setConnectTimeout(30);       // how long to try to connect for before continuing
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
  // mqttclient.setBufferSize(MQTT_BUFFER);
  //  check is WiFi connected

  if (res)
  {
    // set the device name
    MDNS.begin(_settings.data.deviceName);
    WiFi.hostname(_settings.data.deviceName);

    Json["Device_name"] = _settings.data.deviceName;

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_MAIN, htmlProcessor);
      request->send(response); });

    /*
              server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
    size_t max = (ESP.getFreeHeap() / 3) & 0xFFE0;
    AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", [max](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {

                    // Get the chunk based on the index and maxLen
                    size_t len = HTML_MAIN_LEN - index;
                    if (len > maxLen) len = maxLen;
                    if (len > max) len = max;
                    if (len > 0) memcpy_P(buffer, HTML_MAIN + index, len);


                    // Return the actual length of the chunk (0 for end of file)
                    return len;

                }, htmlProcessor);
                request->send(response);
                  });
    */

    server.on("/livejson", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                AsyncResponseStream *response = request->beginResponseStream("application/json");
                serializeJson(Json, *response);
                request->send(response); });

    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_REBOOT, htmlProcessor);
                request->send(response);
                restartNow = true;
                RestartTimer = millis(); });

    server.on("/confirmreset", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_CONFIRM_RESET, htmlProcessor);
      request->send(response); });

    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
              {
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
      AsyncWebParameter *p = request->getParam(0);
      if (p->name() == "ha")
      {
        haDiscTrigger = true;
        }
        request->send(200, "text/plain", "message received"); });

    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_SETTINGS, htmlProcessor);
      request->send(response); });

    server.on("/settingsedit", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_SETTINGS_EDIT, htmlProcessor);
      request->send(response); });

    server.on("/settingssave", HTTP_POST, [](AsyncWebServerRequest *request)
              {
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
                _settings.save();
                request->redirect("/reboot"); });

    server.on(
        "/update", HTTP_POST, [](AsyncWebServerRequest *request)
        {
    //https://gist.github.com/JMishou/60cb762047b735685e8a09cd2eb42a60
    // the request handler is triggered after the upload has finished... 
    // create the response, add header, and send response
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
    MDNS.addService("http", "tcp", 80);
    MDNS.update();

    jsonESP["IP"] = WiFi.localIP();
    jsonESP["sw_version"] = SOFTWARE_VERSION;
  }
  analogWrite(LED_PIN, 255);
  resetCounter(false);
}

void loop()
{
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
      MDNS.update();
      if (millis() > (mqtttimer + (_settings.data.mqttRefresh * 1000)))
      {
        DEBUG_WEBLN("<MQTT> Data Send...");
        sendtoMQTT(); // Update data to MQTT server if we should
        mqtttimer = millis();
      }

      mqttclient.loop(); // Check if we have something to read from MQTT
    }
    notificationLED(); // notification LED routine

    if (haDiscTrigger)
    {
      sendHaDiscovery();
      haDiscTrigger = false;
    }
  }

  if (restartNow && millis() >= (RestartTimer + 500))
  {
    DEBUG_WEBLN("Restart");
    ESP.reset();
  }
}

void prozessData()
{
  dataProzessing = true;
  DEBUG_WEBLN("Ve callback triggerd... prozessing data");
  getJsonData();
  notifyClients();
  dataProzessing = false;

  // Serial.println(ESP.getFreeHeap());

  // float error = 1/0;
  // Serial.println(error);
}

bool getJsonData()
{
  jsonESP["ESP_VCC"] = (ESP.getVcc() / 1000.0) + 0.3;
  jsonESP["Wifi_RSSI"] = WiFi.RSSI();

  // jsonESP["Flash_Size"] = ESP.getFlashChipSize();
  // jsonESP["Sketch_Size"] = ESP.getSketchSize();
  jsonESP["Free_Sketch_Space"] = ESP.getFreeSketchSpace();
  // jsonESP["Real_Flash_Size"] = ESP.getFlashChipRealSize();
  jsonESP["Free_Heap"] = ESP.getFreeHeap();
  jsonESP["HEAP_Fragmentation"] = ESP.getHeapFragmentation();
  //jsonESP["WS_Clients"] = ws.getClients();
  // jsonESP["Free_BlockSize"] = ESP.getMaxFreeBlockSize();

  // Serial.println();
  // Serial.println("VE recived data: ");
  //  Serial.println(myve.veEnd);
  //  const char *descriptor;
  //  const char *Vevalue;

  String rawVal;

  for (int i = 0; i < myve.veEnd; i++)
  {

    if (myve.veName[i] == NULL || strlen(myve.veName[i]) == 0 || myve.veValue[i] == NULL || strlen(myve.veValue[i]) == 0)
    {
      i = myve.veEnd;
      break;
    }

    rawVal += "{\"";
    rawVal += myve.veName[i];
    rawVal += "\":\"";
    rawVal += myve.veValue[i];
    rawVal += "\"},";
    /*
        Serial.print("[");
        Serial.print(myve.veName[i]);
        Serial.print(":");
        Serial.print(myve.veValue[i]);
        Serial.print("]");
    */
    // search for every Vevalue in the list and replace it with clear name
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
                  for (size_t k = 0; k < sizeof(VeDirectDeviceList) / sizeof(**VeDirectDeviceList[0]); k++)
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
  }

  // Serial.println();
  //  Json["RAW"] = rawVal;
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
      mqttclient.publish((topic + String("/Wifi_RSSI")).c_str(), String(WiFi.RSSI()).c_str());

      if (strlen(_settings.data.mqttTriggerPath) > 0)
      {
        DEBUG_WEBLN("MQTT Data Trigger Subscribed");
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
  // updateProgress = true; // stop servicing data
  if (!_settings.data.mqttJson)
  {
    String messageTemp;
    for (unsigned int i = 0; i < length; i++)
    {
      messageTemp += (char)payload[i];
    }
  }
  else
  {
    StaticJsonDocument<1024> mqttJsonAnswer;
    deserializeJson(mqttJsonAnswer, (const byte *)payload, length);
  }

  if (strlen(_settings.data.mqttTriggerPath) > 0 && strcmp(top, _settings.data.mqttTriggerPath) == 0)
  {
    DEBUG_WEBLN("MQTT Data Trigger Firered Up");
    mqtttimer = 0;
  }
  // updateProgress = false; // start data servicing again
}

bool sendHaDiscovery()
{
  if (!connectMQTT())
  {
    return false;
  }
  char topBuff[128];
  char configBuff[1024];
  size_t mqttContentLength;
  for (size_t i = 0; i < sizeof haDescriptor / sizeof haDescriptor[0]; i++)
  {
    if (Json.containsKey(haDescriptor[i][0]))
    {
      sprintf(topBuff, "homeassistant/sensor/%s/%s/config", _settings.data.deviceName, haDescriptor[i][0]); // build the topic

      mqttContentLength = sprintf(configBuff, "{\"state_topic\": \"%s/%s\",\"unique_id\": \"sensor.%s_%s\",\"name\": \"%s\",\"icon\": \"mdi:%s\",\"unit_of_measurement\": \"%s\",\"device_class\":\"%s\",\"device\":{\"identifiers\":[\"%s\"], \"configuration_url\":\"http://%s\",\"name\":\"%s\", \"model\":\"%s\",\"manufacturer\":\"SoftWareCrash\",\"sw_version\":\"Victron2MQTT %s\"}}",
                                  _settings.data.mqttTopic, haDescriptor[i][0], _settings.data.deviceName, haDescriptor[i][0], haDescriptor[i][0], haDescriptor[i][1], haDescriptor[i][2], haDescriptor[i][3], Json["Serial_number"].as<String>().c_str(), jsonESP["IP"].as<String>().c_str(), _settings.data.deviceName, Json["Model_description"].as<String>().c_str(), SOFTWARE_VERSION);

      mqttclient.beginPublish(topBuff, mqttContentLength, true);
      for (size_t i = 0; i < mqttContentLength; i++)
      {
        mqttclient.write(configBuff[i]);
      }
      mqttclient.endPublish();
    }
  }
  return true;
}