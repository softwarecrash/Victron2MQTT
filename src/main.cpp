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

// #include <WebSerialLite.h>
#include <MycilaWebSerial.h>
#include <SoftwareSerial.h>

#include <RTCMemory.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <NonBlockingDallas.h>

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
WebSerial webSerial;

OneWire oneWire(TEMPSENS_PIN);
DallasTemperature dallasTemp(&oneWire);
NonBlockingDallas tempSens(&dallasTemp);

JsonDocument Json;
JsonObject jsonESP = Json["ESP_Data"].to<JsonObject>();
#include "status-LED.h"
ADC_MODE(ADC_VCC);
// --- WiFi watchdog (non-blocking) ---
static uint32_t g_lastReconnectTry = 0;
static uint8_t g_retries = 0;

static const uint32_t WIFI_RECONNECT_INTERVAL_MS = 15UL * 1000UL;
static const uint32_t WIFI_FORCE_REBOOT_MS = 5UL * 60UL * 1000UL;

//----------------------------------------------------------------------
void saveConfigCallback()
{
  writeLog("Should save config");
  shouldSaveConfig = true;
}

// Throttle: max. alle 250 ms senden
static uint32_t _wsLast = 0;

void notifyClients()
{
  if (!ws.count())
    return;
  uint32_t now = millis();
  if (now - _wsLast < 250)
    return;
  _wsLast = now;



/* static const char TEST_WS_JSON[] PROGMEM = R"json(
{"ESP_Data":{"IP":"xxx.xxx.xxx.xxx","sw_version":"1.3.2","Wifi_RSSI":-71,"ESP_VCC":3.793,"Free_Heap":26280,"json_space":812,"WS_Clients":1,"Runtime":858},"Device_name":"Victron2MQTT","Device_model":"0XA443","Device_connection":true,"Remote_Control_State":false,"Serial_number":"xxxxxxx","Firmware_version_24":"126FF","Operation_state":"Off","Current_error":"No error","AC_out_current":0,"AC_out_volt":0,"total_kWh":153.38,"today_kWh":3.4,"Max_pow_today":1354,"Yesterday_kWh":5.51,"Max_pow_yesterday":1705,"Day":29,"Battery_current":0,"Panel_power":0,"Voltage":52.81}
)json";
deserializeJson(Json, FPSTR(TEST_WS_JSON)); */



  size_t len = measureJson(Json);
  if (len == 0)
    return;
  const size_t MAX_WS_BYTES = 1024;
  if (len > MAX_WS_BYTES)
    len = MAX_WS_BYTES;

  AsyncWebSocketMessageBuffer *buf = ws.makeBuffer(len);
  if (!buf)
    return;

  size_t written = serializeJson(Json, (char *)buf->get(), len);
  if (written == 0)
  {
    delete buf;
    return;
  }
  ws.textAll(buf);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (!(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT))
    return;
  std::unique_ptr<char[]> buf(new char[len + 1]);
  memcpy(buf.get(), data, len);
  buf[len] = '\0';

  String msg(buf.get());
  if (msg != "A9")
  {
    if (msg == "remotecontrol_on")
      remoteControl(true);
    else if (msg == "remotecontrol_off")
      remoteControl(false);
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
  case WS_EVT_PING:
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
    break;
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
  if (_settings.keepRcState())
  {
    _settings.setRcState(sw);
    _settings.save();
  }
  remoteControlState = sw;
  mqtttimer = 0;
  return remoteControlState;
}

void checkWiFiAndMaybeReboot()
{
  const uint32_t now = millis();

  if (WiFi.status() == WL_CONNECTED && WiFi.localIP()[0] != 0) {
    lastWifiOK = now;
    return;
  }

  if (now - g_lastReconnectTry >= WIFI_RECONNECT_INTERVAL_MS) {
    g_lastReconnectTry = now;
    g_retries++;
    writeLog("[wifi] reconnect try #%u ...", g_retries);
    WiFi.reconnect();
  }

  const WiFiMode_t mode = WiFi.getMode();
  const bool apActive = (mode == WIFI_AP || mode == WIFI_AP_STA);

  if (!apActive && (now - lastWifiOK >= WIFI_FORCE_REBOOT_MS)) {
    writeLog("[wifi] offline > %lu ms -> reboot", (unsigned long)WIFI_FORCE_REBOOT_MS);
    delay(50);
    ESP.restart();
  }
}


void setup()
{
  _settings.load();
  DBG_BEGIN(DBG_BAUD);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 255 - _settings.LEDBrightness());
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
  if (_settings.keepRcState())
    remoteControlState = _settings.rcState();
  digitalWrite(MYPORT_TX, remoteControlState);

  haAutoDiscTrigger = _settings.haDiscovery();
  WiFi.persistent(true); // fix wifi save bug
  veSerial.begin(VICTRON_BAUD, SWSERIAL_8N1, MYPORT_RX /*, MYPORT_TX, false*/);
  veSerial.flush();
  veSerial.enableRxGPIOPullUp(false);
  myve.callback(prozessData);

  sprintf(mqttClientId, "%s-%06X", _settings.deviceName(), ESP.getChipId());

  AsyncWiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT server", NULL, 32);
  AsyncWiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT User", NULL, 32);
  AsyncWiFiManagerParameter custom_mqtt_pass("mqtt_pass", "MQTT Password", NULL, 32);
  AsyncWiFiManagerParameter custom_mqtt_topic("mqtt_topic", "MQTT Topic", "Victron", 32);
  AsyncWiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", NULL, 6);
  AsyncWiFiManagerParameter custom_mqtt_refresh("mqtt_refresh", "MQTT Send Interval", "300", 4);
  AsyncWiFiManagerParameter custom_mqtt_triggerpath("mqtt_triggerpath", "MQTT Data Trigger Path", NULL, 80);
  AsyncWiFiManagerParameter custom_device_name("device_name", "Device Name", "Victron2MQTT", 32);
  AsyncWiFiManagerParameter custom_static_ip("static_ip", "Static IP (empty for DHCP)", _settings.data.staticIP, 16);
  AsyncWiFiManagerParameter custom_static_gw("static_gw", "Static Gateway (empty for DHCP)", _settings.data.staticGW, 16);
  AsyncWiFiManagerParameter custom_static_sn("static_sn", "Static Subnet (empty for DHCP)", _settings.data.staticSN, 16);
  AsyncWiFiManagerParameter custom_static_dns("static_dns", "Static DNS (empty for DHCP)", _settings.data.staticDNS, 16);

  AsyncWiFiManager wm(&server, &dns); // create wifimanager instance

  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_user);
  wm.addParameter(&custom_mqtt_pass);
  wm.addParameter(&custom_mqtt_topic);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_mqtt_refresh);
  wm.addParameter(&custom_mqtt_triggerpath);
  wm.addParameter(&custom_device_name);
  wm.addParameter(&custom_static_ip);
  wm.addParameter(&custom_static_gw);
  wm.addParameter(&custom_static_sn);
  wm.addParameter(&custom_static_dns);

  wm.setDebugOutput(false);       // disable wifimanager debug output
  wm.setMinimumSignalQuality(25); // filter weak wifi signals
  wm.setConnectTimeout(10);       // how long to try to connect for before continuing
  wm.setConfigPortalTimeout(300); // auto close configportal after n seconds
  wm.setSaveConfigCallback(saveConfigCallback);

  IPAddress ip, gw, sn, dns;
  if (ip.fromString(_settings.staticIP()) && gw.fromString(_settings.staticGW()) && sn.fromString(_settings.staticSN()))
  {
    dns.fromString(_settings.staticDNS());
    wm.setSTAStaticIPConfig(ip, gw, sn, dns);
  }

  bool res = wm.autoConnect("Victron2MQTT-AP");

  // save settings if wifi setup is fire up
  if (shouldSaveConfig)
  {
    _settings.setMqttServer(custom_mqtt_server.getValue());
    _settings.setMqttUser(custom_mqtt_user.getValue());
    _settings.setMqttPassword(custom_mqtt_pass.getValue());
    _settings.setMqttTopic(custom_mqtt_topic.getValue());
    _settings.setMqttPort(atoi(custom_mqtt_port.getValue()));
    _settings.setMqttRefresh(atoi(custom_mqtt_refresh.getValue()));
    _settings.setMqttTriggerPath(custom_mqtt_triggerpath.getValue());
    _settings.setDeviceName(custom_device_name.getValue());
    _settings.setStaticIP(custom_static_ip.getValue());
    _settings.setStaticGW(custom_static_gw.getValue());
    _settings.setStaticSN(custom_static_sn.getValue());
    _settings.setStaticDNS(custom_static_dns.getValue());

    _settings.save();
    ESP.restart();
  }

  topic = _settings.mqttTopic();
  mqttclient.setServer(_settings.mqttServer(), _settings.mqttPort());
  mqttclient.setCallback(mqttCallback);

  if (res)
  {
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &evt)
                            {
  lastWifiOK = millis();
  g_retries  = 0;
  writeLog("[wifi] Connected: %s / %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str()); });

    WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &evt)
                                   {
  writeLog("[wifi] Disconnected (reason=%u) – will try to reconnect", evt.reason);
  g_lastReconnectTry = 0; });

    // set the device name
    MDNS.begin(_settings.deviceName());
    MDNS.addService("http", "tcp", 80);
    WiFi.hostname(_settings.deviceName());

    Json["Device_name"] = _settings.data.deviceName;

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_MAIN, htmlProcessor);
      request->send(response); });

    server.on("/livejson", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();
                AsyncResponseStream *response = request->beginResponseStream("application/json");
                serializeJson(Json, *response);
                request->send(response); });

    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();
                AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_REBOOT, htmlProcessor);
                request->send(response);
                restartNow = true;
                RestartTimer = millis(); });

    server.on("/confirmreset", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_CONFIRM_RESET, htmlProcessor);
      request->send(response); });

    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();
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
      if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();
    String message;
    if (request->hasParam("ha")) {
      haDiscTrigger = true;
    }
    if (request->hasParam("remotecontrol")) {
      message = request->getParam("remotecontrol")->value();
      remoteControl((message == "1") ? true:false);
    }    
        request->send(200, "text/plain", "message received"); });

    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_SETTINGS, htmlProcessor);
      request->send(response); });

    server.on("/settingsedit", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_SETTINGS_EDIT, htmlProcessor);
      request->send(response); });

    server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();

  AsyncResponseStream* res = request->beginResponseStream("application/json");
  res->addHeader("Cache-Control", "no-store");
  res->addHeader("Pragma", "no-cache");

  JsonDocument doc;

  doc["deviceName"]    = _settings.deviceName();

  doc["staticIP"]      = _settings.staticIP();
  doc["staticGW"]      = _settings.staticGW();
  doc["staticSN"]      = _settings.staticSN();
  doc["staticDNS"]     = _settings.staticDNS();

  doc["mqttServer"]    = _settings.mqttServer();
  doc["mqttPort"]      = _settings.mqttPort();
  doc["mqttUser"]      = _settings.mqttUser();
  doc["mqttPassword"]  = _settings.mqttPassword();
  doc["mqttTopic"]     = _settings.mqttTopic();
  doc["mqttRefresh"]   = _settings.mqttRefresh();
  doc["mqttTrigger"]   = _settings.mqttTriggerPath();
  doc["mqttJson"]      = _settings.mqttJson();

  doc["haDiscovery"]   = _settings.haDiscovery();
  doc["webUIdarkmode"] = _settings.webUIdarkmode();
  doc["keepRcState"]   = _settings.keepRcState();
  doc["rcState"]       = _settings.rcState();
  doc["LEDBrightness"] = _settings.LEDBrightness();

  doc["httpUser"]      = _settings.httpUser();
  doc["httpPass"]      = _settings.httpPass();

  serializeJson(doc, *res);
  request->send(res); });

    server.on("/settingssave", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();
                  _settings.setMqttServer(request->arg("post_mqttServer"));
                  _settings.setMqttPort(request->arg("post_mqttPort").toInt());
                  _settings.setMqttUser(request->arg("post_mqttUser"));
                  _settings.setMqttPassword(request->arg("post_mqttPassword"));
                  _settings.setMqttTopic(request->arg("post_mqttTopic"));
                  _settings.setMqttRefresh(request->arg("post_mqttRefresh").toInt());
                  _settings.setDeviceName(request->arg("post_deviceName"));
                  _settings.setMqttJson(request->arg("post_mqttjson") == "true");
                  _settings.setStaticIP(request->arg("post_staticIP"));
                  _settings.setStaticGW(request->arg("post_staticGW"));
                  _settings.setStaticSN(request->arg("post_staticSN"));
                  _settings.setStaticDNS(request->arg("post_staticDNS"));
                  _settings.setMqttTriggerPath(request->arg("post_mqtttrigger"));
                  _settings.setWebUIdarkmode(request->arg("post_webuicolormode") == "true");
                  _settings.setHttpUser(request->arg("post_httpUser"));
                  _settings.setHttpPass(request->arg("post_httpPass"));
                  _settings.setHaDiscovery(request->arg("post_hadiscovery") == "true");
                  _settings.setKeepRcState(request->arg("post_keeprcstate") == "true");
                  _settings.setLEDBrightness(request->arg("post_led").toInt());
                _settings.save();
                request->redirect("/reboot"); });

    server.on(
        "/update", HTTP_POST, [](AsyncWebServerRequest *request)
        {
    //https://gist.github.com/JMishou/60cb762047b735685e8a09cd2eb42a60
    // the request handler is triggered after the upload has finished... 
    // create the response, add header, and send response
    if(strlen(_settings.httpUser()) > 0 && !request->authenticate(_settings.httpUser(), _settings.httpPass())) return request->requestAuthentication();
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

    webSerial.begin(&server);

    server.begin();

    jsonESP["IP"] = WiFi.localIP().toString();
    jsonESP["sw_version"] = SOFTWARE_VERSION;
    tempSens.begin(NonBlockingDallas::resolution_12, TIME_INTERVAL);
    tempSens.onTemperatureChange(handleTemperatureChange);
  }
  analogWrite(LED_PIN, 255);
  RTCmem->bootcount = 0;
  rtcMemory.save();
  lastWifiOK = millis();
}

void loop()
{
  MDNS.update();
  tempSens.update();
  if (Update.isRunning())
  {
    workerCanRun = false;
  }
  if (workerCanRun)
  {
    ReadVEData();

    // Make sure wifi is in the right mode
    if (WiFi.status() == WL_CONNECTED)
    { // No use going to next step unless WIFI is up and running.
      // ws.cleanupClients(); // clean unused client connections
      //  MDNS.update();
      if (millis() - mqtttimer > (_settings.data.mqttRefresh * 1000) || mqtttimer == 0)
      {
        writeLog("<MQTT> Data Send...");
        sendtoMQTT(); // Update data to MQTT server if we should
        mqtttimer = millis();
      }

      mqttclient.loop(); // Check if we have something to read from MQTT
    }
    notificationLED(); // notification LED routine
    checkWiFiAndMaybeReboot();

    if ((haDiscTrigger || _settings.haDiscovery()) && measureJson(Json) > jsonSize)
    {
      if (sendHaDiscovery())
      {
        haDiscTrigger = false;
        jsonSize = measureJson(Json);
      }
    }
    jsonESP["Wifi_RSSI"] = WiFi.RSSI();
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
  jsonESP["Free_Heap"] = ESP.getFreeHeap();
  jsonESP["json_size"] = measureJson(Json);
  jsonESP["WS_Clients"] = ws.count();
  jsonESP["Runtime"] = millis() / 1000;
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
    for (size_t j = 0; j < VePrettyDataSize; j++)
    {
      VePrettyEntry entry;
      memcpy_P(&entry, &VePrettyData[j], sizeof(entry));

      char key[16];
      strcpy_P(key, entry.key);

      if (strcmp(key, myve.veName[i]) == 0)
      {
        char name[32];
        char op[8];
        strcpy_P(name, entry.name);
        strcpy_P(op, entry.op);

        if (strlen(op) > 0 && strcmp(op, "0") != 0)
        {
          Json[FPSTR(entry.name)] = (int)((atof(myve.veValue[i]) / atoi(op)) * 100 + 0.5) / 100.0;
        }
        else if (strcmp(op, "0") == 0)
        {
          Json[FPSTR(entry.name)] = atoi(myve.veValue[i]);
        }
        else
        {
          Json[FPSTR(entry.name)] = myve.veValue[i];
        }
        if (strcmp(name, "Device_model") == 0)
        {
          uint16_t deviceID = strtol(myve.veValue[i], nullptr, 16);
          VeDeviceEntry devEntry;
          size_t left = 0, right = VeDeviceListSize;
          const char *modelName = nullptr;

          while (left < right)
          {
            size_t mid = (left + right) / 2;
            memcpy_P(&devEntry, &VeDeviceList[mid], sizeof(devEntry));
            if (deviceID == devEntry.id)
            {
              modelName = (const char *)pgm_read_ptr(&devEntry.name);
              break;
            }
            else if (deviceID < devEntry.id)
            {
              right = mid;
            }
            else
            {
              left = mid + 1;
            }
          }
          if (modelName)
          {
            Json[FPSTR(entry.name)] = FPSTR(modelName);
          }
        }
        struct CodeMap
        {
          const char *label;
          const VeCodeEntry *table;
          size_t size;
        };

        const CodeMap maps[] = {
            {"Alarm_code", VeDirectDeviceCodeAR, VeDirectDeviceCodeARSize},
            {"Off_reason", VeDirectDeviceCodeOR, VeDirectDeviceCodeORSize},
            {"Operation_state", VeDirectDeviceCodeCS, VeDirectDeviceCodeCSSize},
            {"Current_error", VeDirectDeviceCodeERR, VeDirectDeviceCodeERRSize},
            {"Tracker_operation_mode", VeDirectDeviceCodeMPPT, VeDirectDeviceCodeMPPTSize},
        };

        for (const auto &map : maps)
        {
          if (strcmp(name, map.label) == 0)
          {
            for (size_t k = 0; k < map.size; k++)
            {
              VeCodeEntry codeEntry;
              memcpy_P(&codeEntry, &map.table[k], sizeof(codeEntry));

              char codeBuf[16];
              strcpy_P(codeBuf, codeEntry.code);
              if (strcmp(codeBuf, myve.veValue[i]) == 0)
              {
                Json[FPSTR(entry.name)] = FPSTR(codeEntry.text);
                break;
              }
            }
          }
        }
        break;
      }
    }

    Json["Device_connection"] = !myve.veError;
    Json["Remote_Control_State"] = remoteControlState;
  }
  return true;
}

bool connectMQTT()
{
  if (!mqttclient.connected())
  {
    if (mqttclient.connect(mqttClientId, _settings.mqttUser(), _settings.mqttPassword(), (topic + "/Alive").c_str(), 0, true, "false", true))
    {
      mqttclient.publish((topic + String("/IP")).c_str(), String(WiFi.localIP().toString()).c_str(), true);
      mqttclient.publish((topic + String("/Alive")).c_str(), "true", true); // LWT online message must be retained!
      mqttclient.subscribe((topic + "/Remote_Control").c_str());

      if (strlen(_settings.mqttTriggerPath()) > 0)
      {
        writeLog("MQTT Data Trigger Subscribed");
        mqttclient.subscribe(_settings.mqttTriggerPath());
      }
      return true;
    }
    else
    {
      writeLog("[MQTT] Connection Failed, rc=%d", mqttclient.state());
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
  if (!_settings.mqttJson())
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

  for (unsigned int i = 0; i < length; i++)
  {
    messageTemp += (char)payload[i];
  }

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
    if (!Json[haDescriptor[i][0]].isNull())
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

void handleTemperatureChange(int deviceIndex, int32_t temperatureRAW)
{
  float tempCels = tempSens.rawToCelsius(temperatureRAW);
  if (tempCels <= -55 || tempCels >= 125)
    return;
  writeLog("<DS18x> DS18B20_%d  Celsius:%f", deviceIndex + 1, tempCels);
  char msgBuffer[8];
  jsonESP["DS18B20_" + String(deviceIndex + 1)] = dtostrf(tempCels, 4, 2, msgBuffer);
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