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
#include <ctype.h>

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
bool veFramePending = false;
bool tempStateDirty = false;
uint32_t veSerialOverflowCount = 0;

struct BufferedTemperatureState
{
  bool valid = false;
  bool dirty = false;
  bool pending = false;
  int32_t raw = DEVICE_DISCONNECTED_RAW;
  int32_t pendingRaw = DEVICE_DISCONNECTED_RAW;
};

BufferedTemperatureState bufferedTemperatures[MAX_TEMPERATURE_SENSORS];

static constexpr int32_t DS18_GLITCH_DELTA_RAW = 384; // 3.0 C
static constexpr int32_t DS18_CONFIRM_DELTA_RAW = 96; // 0.75 C
static constexpr int32_t DS18_POWER_ON_RAW = 10880;   // 85.0 C

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
#include "status-LED.h"
ADC_MODE(ADC_VCC);
// --- WiFi watchdog (non-blocking) ---
static uint32_t g_lastReconnectTry = 0;
static uint8_t g_retries = 0;

static const uint32_t WIFI_RECONNECT_INTERVAL_MS = 15UL * 1000UL;
static const uint32_t WIFI_FORCE_REBOOT_MS = 5UL * 60UL * 1000UL;

static String sanitizeMqttBaseTopic(const String &raw)
{
  String out;
  out.reserve(raw.length() + 4);
  bool lastWasSlash = false;
  bool lastWasUnderscore = false;

  for (size_t i = 0; i < raw.length(); i++)
  {
    const char c = raw[i];
    if (isalnum((unsigned char)c) || c == '_' || c == '-')
    {
      out += c;
      lastWasSlash = false;
      lastWasUnderscore = (c == '_');
      continue;
    }

    if (c == '/')
    {
      if (!lastWasSlash && out.length() > 0)
      {
        out += '/';
        lastWasSlash = true;
        lastWasUnderscore = false;
      }
      continue;
    }

    if (!lastWasUnderscore && out.length() > 0 && !lastWasSlash)
    {
      out += '_';
      lastWasUnderscore = true;
    }
  }

  while (out.length() > 0 && (out[0] == '/' || out[0] == '_'))
    out.remove(0, 1);
  while (out.length() > 0 && (out[out.length() - 1] == '/' || out[out.length() - 1] == '_'))
    out.remove(out.length() - 1, 1);

  if (out.length() == 0)
    out = "Victron";
  return out;
}

static String sanitizeMqttSegment(const char *raw)
{
  if (!raw)
    return String();
  String out;
  out.reserve(strlen(raw));
  bool lastWasUnderscore = false;
  for (size_t i = 0; raw[i] != '\0'; i++)
  {
    const char c = raw[i];
    if (isalnum((unsigned char)c) || c == '_' || c == '-')
    {
      out += c;
      lastWasUnderscore = (c == '_');
    }
    else if (!lastWasUnderscore && out.length() > 0)
    {
      out += '_';
      lastWasUnderscore = true;
    }
  }

  while (out.length() > 0 && out[0] == '_')
    out.remove(0, 1);
  while (out.length() > 0 && out[out.length() - 1] == '_')
    out.remove(out.length() - 1, 1);
  return out;
}

static String buildTopicPath(const String &base, const char *segment)
{
  const String cleanSeg = sanitizeMqttSegment(segment);
  if (cleanSeg.length() == 0)
    return base;
  return base + "/" + cleanSeg;
}

static bool buildHaDiscoveryTopic(char *buffer, size_t bufferSize, const char *component, const char *objectId)
{
  if (!buffer || bufferSize == 0 || !component || !objectId || objectId[0] == '\0')
    return false;

  const int written = snprintf(buffer, bufferSize, "homeassistant/%s/%s/%s/config", component, mqttClientId, objectId);
  return written > 0 && written < (int)bufferSize;
}

static bool jsonVariantToString(JsonVariantConst value, String &out)
{
  out = "";
  if (value.isNull())
    return false;
  if (value.is<JsonObjectConst>() || value.is<JsonArrayConst>())
  {
    serializeJson(value, out);
    return true;
  }
  out = value.as<String>();
  return true;
}

static JsonObject ensureEspDataObject()
{
  JsonVariant v = Json["ESP_Data"];
  if (!v.is<JsonObject>())
  {
    v.clear();
    return v.to<JsonObject>();
  }
  return v.as<JsonObject>();
}

template <typename T>
static void setEspDataValue(const char *key, T value)
{
  ensureEspDataObject();
  Json["ESP_Data"][key] = value;
}

static void removeEspDataKey(const char *key)
{
  JsonVariant v = Json["ESP_Data"];
  if (v.is<JsonObject>())
    v.remove(key);
}

static bool isKnownRootKey(const char *key)
{
  if (!key || key[0] == '\0')
    return false;

  if (strncmp(key, "DS18B20_", 8) == 0)
    return true;

  if (strcmp(key, "ESP_Data") == 0 ||
      strcmp(key, "Device_connection") == 0 ||
      strcmp(key, "Remote_Control_State") == 0 ||
      strcmp(key, "Device_name") == 0)
  {
    return true;
  }

  for (size_t j = 0; j < VePrettyDataSize; j++)
  {
    VePrettyEntry entry;
    memcpy_P(&entry, &VePrettyData[j], sizeof(entry));
    char name[32];
    strcpy_P(name, entry.name);
    if (strcmp(name, key) == 0)
      return true;
  }
  return false;
}

static bool isKnownEspDataKey(const char *key)
{
  if (!key || key[0] == '\0')
    return false;

  static const char *const knownKeys[] = {
      "IP",
      "sw_version",
      "Wifi_RSSI",
      "ESP_VCC",
      "Free_Heap",
      "json_size",
      "WS_Clients",
      "Runtime",
      "VE_RX_Overflow"};

  for (size_t i = 0; i < sizeof(knownKeys) / sizeof(knownKeys[0]); i++)
  {
    if (strcmp(key, knownKeys[i]) == 0)
      return true;
  }

  return strncmp(key, "DS18B20_", 8) == 0;
}

static void pruneUnknownMqttKeys()
{
  bool removed = true;
  while (removed)
  {
    removed = false;
    for (JsonPair i : Json.as<JsonObject>())
    {
      const char *rootKey = i.key().c_str();
      if (!isKnownRootKey(rootKey))
      {
        Json.remove(rootKey);
        removed = true;
        break;
      }
    }
  }

  JsonObject esp = ensureEspDataObject();
  while (true)
  {
    removed = false;
    for (JsonPair i : esp)
    {
      const char *key = i.key().c_str();
      if (!isKnownEspDataKey(key))
      {
        esp.remove(key);
        removed = true;
        break;
      }
    }
    if (!removed)
      break;
  }
}

static void applyBufferedTemperatureData()
{
  if (!tempStateDirty)
    return;

  tempStateDirty = false;
  for (size_t i = 0; i < MAX_TEMPERATURE_SENSORS; i++)
  {
    BufferedTemperatureState &state = bufferedTemperatures[i];
    if (!state.dirty)
      continue;

    state.dirty = false;

    char key[16];
    snprintf(key, sizeof(key), "DS18B20_%u", (unsigned)(i + 1));
    if (!state.valid)
    {
      removeEspDataKey(key);
      Json.remove(key);
      continue;
    }

    char msgBuffer[8];
    const float tempCels = tempSens.rawToCelsius(state.raw);
    dtostrf(tempCels, 4, 2, msgBuffer);
    setEspDataValue(key, msgBuffer);
    Json[key] = msgBuffer;
  }
}

static int32_t ds18RawDelta(int32_t a, int32_t b)
{
  return (a > b) ? (a - b) : (b - a);
}

static bool isValidDs18TemperatureRaw(int32_t temperatureRAW)
{
  return temperatureRAW >= (-55 * 128) && temperatureRAW <= (125 * 128);
}

static void publishAcceptedTemperatureSample(int deviceIndex, int32_t temperatureRAW)
{
  BufferedTemperatureState &state = bufferedTemperatures[deviceIndex];
  if (state.valid && state.raw == temperatureRAW)
    return;

  const float tempCels = tempSens.rawToCelsius(temperatureRAW);
  writeLog("<DS18x> DS18B20_%d Celsius:%.2f", deviceIndex + 1, tempCels);

  state.raw = temperatureRAW;
  state.valid = true;
  state.dirty = true;
  tempStateDirty = true;
}

static void holdSuspiciousTemperatureSample(int deviceIndex, int32_t temperatureRAW)
{
  BufferedTemperatureState &state = bufferedTemperatures[deviceIndex];
  if (state.pending && state.pendingRaw == temperatureRAW)
    return;

  const float tempCels = tempSens.rawToCelsius(temperatureRAW);
  writeLog("<DS18x> DS18B20_%d hold suspicious sample:%.2f", deviceIndex + 1, tempCels);

  state.pending = true;
  state.pendingRaw = temperatureRAW;
}

static bool publishJsonAsFlatTopics(const String &baseTopic, JsonVariantConst value);

static bool publishObjectAsFlatTopics(const String &baseTopic, JsonObjectConst object)
{
  for (JsonPairConst i : object)
  {
    const String cleanKey = sanitizeMqttSegment(i.key().c_str());
    if (cleanKey.length() == 0)
    {
      writeLog("[MQTT] drop invalid key '%s'", i.key().c_str());
      continue;
    }

    const String childTopic = buildTopicPath(baseTopic, cleanKey.c_str());
    if (!publishJsonAsFlatTopics(childTopic, i.value()))
      return false;
  }

  return true;
}

static bool publishEspDataAsFlatTopics(const String &baseTopic, JsonObjectConst object)
{
  for (JsonPairConst i : object)
  {
    if (strncmp(i.key().c_str(), "DS18B20_", 8) == 0)
      continue;

    const String cleanKey = sanitizeMqttSegment(i.key().c_str());
    if (cleanKey.length() == 0)
    {
      writeLog("[MQTT] drop invalid key '%s'", i.key().c_str());
      continue;
    }

    const String childTopic = buildTopicPath(baseTopic, cleanKey.c_str());
    if (!publishJsonAsFlatTopics(childTopic, i.value()))
      return false;
  }

  return true;
}

static bool publishJsonAsFlatTopics(const String &baseTopic, JsonVariantConst value)
{
  if (value.is<JsonObjectConst>())
    return publishObjectAsFlatTopics(baseTopic, value.as<JsonObjectConst>());

  String payload;
  if (!jsonVariantToString(value, payload))
    return false;

  return mqttclient.publish(baseTopic.c_str(), payload.c_str());
}

static JsonVariantConst getDs18ValueFromEspData(const char *sensorKey)
{
  JsonVariantConst espVariant = Json["ESP_Data"];
  if (!espVariant.is<JsonObjectConst>())
    return JsonVariantConst();

  return espVariant[sensorKey];
}

//----------------------------------------------------------------------
void saveConfigCallback()
{
  writeLog("Should save config");
  shouldSaveConfig = true;
}

bool checkAuth(AsyncWebServerRequest *request)
{
  // Prüfe, ob Benutzername in den Settings gesetzt ist
  const char *user = _settings.get.httpUser();
  const char *pass = _settings.get.httpPass();

  // Nur prüfen, wenn ein Benutzername existiert
  if (strlen(user) > 0)
  {
    if (!request->authenticate(user, pass))
    {
      request->requestAuthentication();
      return false; // Anfrage hier stoppen
    }
  }

  return true; // alles ok
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

  applyBufferedTemperatureData();
  Json["Device_name"] = _settings.get.deviceName();
  Json["Device_connection"] = !myve.veError;
  Json["Remote_Control_State"] = remoteControlState;
  setEspDataValue("IP", WiFi.localIP().toString());
  setEspDataValue("sw_version", SOFTWARE_VERSION);
  setEspDataValue("ESP_VCC", (ESP.getVcc() / 1000.0) + 0.3);
  setEspDataValue("Wifi_RSSI", WiFi.RSSI());
  setEspDataValue("Free_Heap", ESP.getFreeHeap());
  setEspDataValue("WS_Clients", ws.count());
  setEspDataValue("Runtime", millis() / 1000);
  setEspDataValue("VE_RX_Overflow", veSerialOverflowCount);
  pruneUnknownMqttKeys();



/* static const char TEST_WS_JSON[] PROGMEM = R"json(
{"ESP_Data":{"IP":"xxx.xxx.xxx.xxx","sw_version":"1.3.2","Wifi_RSSI":-71,"ESP_VCC":3.793,"Free_Heap":26280,"json_space":812,"WS_Clients":1,"Runtime":858},"Device_name":"Victron2MQTT","Device_model":"0XA443","Device_connection":true,"Remote_Control_State":false,"Serial_number":"xxxxxxx","Firmware_version_24":"126FF","Operation_state":"Off","Current_error":"No error","AC_out_current":0,"AC_out_volt":0,"total_kWh":153.38,"today_kWh":3.4,"Max_pow_today":1354,"Yesterday_kWh":5.51,"Max_pow_yesterday":1705,"Day":29,"Battery_current":0,"Panel_power":0,"Voltage":52.81}
)json";
deserializeJson(Json, FPSTR(TEST_WS_JSON)); */



  String payload;
  payload.reserve(measureJson(Json) + 1);
  if (serializeJson(Json, payload) == 0)
    return;
  const size_t len = payload.length();
  if (len == 0)
    return;

  AsyncWebSocketMessageBuffer *buf = ws.makeBuffer(len);
  if (!buf)
    return;

  memcpy(buf->get(), payload.c_str(), len);
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
  size_t drained = 0;
  while (veSerial.available())
  {
    myve.rxData(veSerial.read());
    drained++;
    if ((drained & 0x1F) == 0)
      esp_yield();
  }

  if (veSerial.overflow())
  {
    veSerialOverflowCount++;
    writeLog("[VE] serial rx overflow #%lu", (unsigned long)veSerialOverflowCount);
  }
}

void queueVeDataProcessing()
{
  veFramePending = true;
}

void processPendingVeData()
{
  if (!veFramePending || dataProzessing)
    return;

  veFramePending = false;
  prozessData();
}

bool remoteControl(bool sw)
{
  writeLog("set Remote Control to: %d", sw);
  digitalWrite(MYPORT_TX, sw);
  rtcData *RTCmem = rtcMemory.getData();
  RTCmem->remoteControlState = sw;
  rtcMemory.save();
  if (_settings.get.keepRcState())
  {
    _settings.set.rcState(sw);
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
  analogWrite(LED_PIN, 255 - _settings.get.LEDBrightness());
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
  if (_settings.get.keepRcState())
    remoteControlState = _settings.get.rcState();
  digitalWrite(MYPORT_TX, remoteControlState);

  haAutoDiscTrigger = _settings.get.haDiscovery();
  WiFi.persistent(true); // fix wifi save bug
  veSerial.begin(VICTRON_BAUD, SWSERIAL_8N1, MYPORT_RX, -1, false, VE_RX_BUFFER, VE_ISR_BUFFER);
  veSerial.flush();
  veSerial.enableRxGPIOPullUp(false);
  myve.callback(queueVeDataProcessing);

  String clientIdName = sanitizeMqttSegment(_settings.get.deviceName());
  if (clientIdName.length() == 0)
    clientIdName = "Victron2MQTT";
  snprintf(mqttClientId, sizeof(mqttClientId), "%s-%06X", clientIdName.c_str(), ESP.getChipId());

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
  if (ip.fromString(_settings.get.staticIP()) && gw.fromString(_settings.get.staticGW()) && sn.fromString(_settings.get.staticSN()))
  {
    dns.fromString(_settings.get.staticDNS());
    wm.setSTAStaticIPConfig(ip, gw, sn, dns);
  }

  bool res = wm.autoConnect("Victron2MQTT-AP");

  // save settings if wifi setup is fire up
  if (shouldSaveConfig)
  {
    _settings.set.mqttServer(custom_mqtt_server.getValue());
    _settings.set.mqttUser(custom_mqtt_user.getValue());
    _settings.set.mqttPassword(custom_mqtt_pass.getValue());
    _settings.set.mqttTopic(sanitizeMqttBaseTopic(custom_mqtt_topic.getValue()));
    _settings.set.mqttPort(atoi(custom_mqtt_port.getValue()));
    _settings.set.mqttRefresh(atoi(custom_mqtt_refresh.getValue()));
    _settings.set.mqttTriggerPath(custom_mqtt_triggerpath.getValue());
    _settings.set.deviceName(custom_device_name.getValue());
    _settings.set.staticIP(custom_static_ip.getValue());
    _settings.set.staticGW(custom_static_gw.getValue());
    _settings.set.staticSN(custom_static_sn.getValue());
    _settings.set.staticDNS(custom_static_dns.getValue());

    _settings.save();
    ESP.restart();
  }

  const String configuredTopic = _settings.get.mqttTopic();
  topic = sanitizeMqttBaseTopic(configuredTopic);
  if (topic != configuredTopic)
  {
    writeLog("[MQTT] normalized topic '%s' -> '%s'", configuredTopic.c_str(), topic.c_str());
    _settings.set.mqttTopic(topic);
    _settings.save();
  }
  mqttclient.setServer(_settings.get.mqttServer(), _settings.get.mqttPort());
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
    MDNS.begin(_settings.get.deviceName());
    MDNS.addService("http", "tcp", 80);
    WiFi.hostname(_settings.get.deviceName());

    Json["Device_name"] = _settings.get.deviceName();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if (!checkAuth(request)) return;
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_MAIN, htmlProcessor);
      request->send(response); });

    server.on("/livejson", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                if (!checkAuth(request)) return;
                AsyncResponseStream *response = request->beginResponseStream("application/json");
                serializeJson(Json, *response);
                request->send(response); });

    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                if (!checkAuth(request)) return;
                AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_REBOOT, htmlProcessor);
                request->send(response);
                restartNow = true;
                RestartTimer = millis(); });

    server.on("/confirmreset", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if (!checkAuth(request)) return;
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_CONFIRM_RESET, htmlProcessor);
      request->send(response); });

    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                if (!checkAuth(request)) return;
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
      if (!checkAuth(request)) return;
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
      if (!checkAuth(request)) return;
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_SETTINGS, htmlProcessor);
      request->send(response); });

    server.on("/backuprestore", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if (!checkAuth(request)) return;
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_BACKUPRESTORE, htmlProcessor);
      request->send(response); });

    server.on("/settingsedit", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if (!checkAuth(request)) return;
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", HTML_SETTINGS_EDIT, htmlProcessor);
      request->send(response); });

    server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      if (!checkAuth(request)) return;

  AsyncResponseStream* res = request->beginResponseStream("application/json");
  res->addHeader("Cache-Control", "no-store");
  res->addHeader("Pragma", "no-cache");

  JsonDocument doc;

  doc["deviceName"]    = _settings.get.deviceName();

  doc["staticIP"]      = _settings.get.staticIP();
  doc["staticGW"]      = _settings.get.staticGW();
  doc["staticSN"]      = _settings.get.staticSN();
  doc["staticDNS"]     = _settings.get.staticDNS();

  doc["mqttServer"]    = _settings.get.mqttServer();
  doc["mqttPort"]      = _settings.get.mqttPort();
  doc["mqttUser"]      = _settings.get.mqttUser();
  doc["mqttPassword"]  = _settings.get.mqttPassword();
  doc["mqttTopic"]     = _settings.get.mqttTopic();
  doc["mqttRefresh"]   = _settings.get.mqttRefresh();
  doc["mqttTrigger"]   = _settings.get.mqttTriggerPath();
  doc["mqttJson"]      = _settings.get.mqttJson();

  doc["haDiscovery"]   = _settings.get.haDiscovery();
  doc["webUIdarkmode"] = _settings.get.webUIdarkmode();
  doc["keepRcState"]   = _settings.get.keepRcState();
  doc["rcState"]       = _settings.get.rcState();
  doc["LEDBrightness"] = _settings.get.LEDBrightness();

  doc["httpUser"]      = _settings.get.httpUser();
  doc["httpPass"]      = _settings.get.httpPass();

  serializeJson(doc, *res);
  request->send(res); });

    server.on("/settingssave", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                if (!checkAuth(request)) return;
                  _settings.set.mqttServer(request->arg("post_mqttServer"));
                  _settings.set.mqttPort(request->arg("post_mqttPort").toInt());
                  _settings.set.mqttUser(request->arg("post_mqttUser"));
                  _settings.set.mqttPassword(request->arg("post_mqttPassword"));
                  _settings.set.mqttTopic(sanitizeMqttBaseTopic(request->arg("post_mqttTopic")));
                  _settings.set.mqttRefresh(request->arg("post_mqttRefresh").toInt());
                  _settings.set.deviceName(request->arg("post_deviceName"));
                  _settings.set.mqttJson(request->arg("post_mqttjson") == "true");
                  _settings.set.staticIP(request->arg("post_staticIP"));
                  _settings.set.staticGW(request->arg("post_staticGW"));
                  _settings.set.staticSN(request->arg("post_staticSN"));
                  _settings.set.staticDNS(request->arg("post_staticDNS"));
                  _settings.set.mqttTriggerPath(request->arg("post_mqtttrigger"));
                  _settings.set.webUIdarkmode(request->arg("post_webuicolormode") == "true");
                  _settings.set.httpUser(request->arg("post_httpUser"));
                  _settings.set.httpPass(request->arg("post_httpPass"));
                  _settings.set.haDiscovery(request->arg("post_hadiscovery") == "true");
                  _settings.set.keepRcState(request->arg("post_keeprcstate") == "true");
                  _settings.set.LEDBrightness(request->arg("post_led").toInt());
                _settings.save();
                request->redirect("/reboot"); });

    server.on(
        "/update", HTTP_POST, [](AsyncWebServerRequest *request)
        {
    //https://gist.github.com/JMishou/60cb762047b735685e8a09cd2eb42a60
    // the request handler is triggered after the upload has finished... 
    // create the response, add header, and send response
    if (!checkAuth(request)) return;
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

// === /api/settings/backup (GET) ===
server.on("/api/settings/backup", HTTP_GET, [](AsyncWebServerRequest *request)
{
  if (!checkAuth(request)) return;

  bool pretty = false;
  if (request->hasParam("pretty")) {
    const String v = request->getParam("pretty")->value();
    pretty = (v == "1" || v == "true" || v == "yes");
  }

  const String payload = _settings.backup(pretty);
  request->send(200, "application/json; charset=utf-8", payload);
});

// === /api/settings/restore (POST) ===
// Body = JSON (Backup-Inhalt) – Antwort wird NUR in onBody gesendet.
server.on(
  "/api/settings/restore",
  HTTP_POST,
  // onRequest: KEINE Antwort hier! Nur Auth prüfen und return.
  [](AsyncWebServerRequest *request) {
    if (!checkAuth(request)) return;
    // nichts senden; wir antworten in onBody
  },
  // onUpload: nicht genutzt
  NULL,
  // onBody: JSON sammeln, am Ende verarbeiten und antworten
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
  {
    if (!checkAuth(request)) return;

    // Body-Puffer anlegen/erweitern
    String *body = reinterpret_cast<String*>(request->_tempObject);
    if (index == 0) {
      body = new String();
      body->reserve(total ? total : (len + 64));
      request->_tempObject = body;
    }
    body->concat((const char*)data, len);

    // Letzter Chunk?
    if (index + len >= total) {
      bool merge   = true;
      bool saveNow = true;
      if (request->hasParam("merge")) {
        const String v = request->getParam("merge")->value();
        merge = !(v == "0" || v == "false" || v == "no");
      }
      if (request->hasParam("save")) {
        const String v = request->getParam("save")->value();
        saveNow = !(v == "0" || v == "false" || v == "no");
      }

      String err;
      const bool ok = _settings.restore(*body, /*merge=*/merge, /*saveNow=*/saveNow, &err);

      String reply;
      if (ok) {
        JsonDocument doc;
        doc["ok"]      = true;
        doc["merge"]   = merge;
        doc["saved"]   = saveNow;
        doc["device"]  = _settings.get.deviceName();
        doc["mqttSrv"] = _settings.get.mqttServer();
        serializeJson(doc, reply);
        request->send(200, "application/json; charset=utf-8", reply);
      } else {
        JsonDocument doc;
        doc["ok"]    = false;
        doc["error"] = err;
        serializeJson(doc, reply);
        request->send(400, "application/json; charset=utf-8", reply);
      }

      // Cleanup
      delete body;
      request->_tempObject = nullptr;
    }
  }
);




    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(418, "text/plain", "418 I'm a teapot"); });

    ws.onEvent(onEvent);
    server.addHandler(&ws);

    webSerial.begin(&server);

    server.begin();

    setEspDataValue("IP", WiFi.localIP().toString());
    setEspDataValue("sw_version", SOFTWARE_VERSION);
    tempSens.begin(NonBlockingDallas::resolution_12, TIME_INTERVAL);
    tempSens.onIntervalElapsed(handleTemperatureSample);
    tempSens.onDeviceDisconnected(handleTemperatureDisconnect);
  }
  analogWrite(LED_PIN, 255);
  RTCmem->bootcount = 0;
  rtcMemory.save();
  lastWifiOK = millis();
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
    tempSens.update();
    applyBufferedTemperatureData();
    processPendingVeData();

    // Make sure wifi is in the right mode
    if (WiFi.status() == WL_CONNECTED)
    { // No use going to next step unless WIFI is up and running.
      mqttclient.loop(); // Check if we have something to read from MQTT

      if (millis() - mqtttimer > (_settings.get.mqttRefresh() * 1000UL) || mqtttimer == 0)
      {
        writeLog("<MQTT> Data Send...");
        sendtoMQTT(); // Update data to MQTT server if we should
        mqtttimer = millis();
      }
    }
    notificationLED(); // notification LED routine
    checkWiFiAndMaybeReboot();

    if ((haDiscTrigger || _settings.get.haDiscovery()) && measureJson(Json) > jsonSize)
    {
      if (sendHaDiscovery())
      {
        haDiscTrigger = false;
        jsonSize = measureJson(Json);
      }
    }
    Json["Device_name"] = _settings.get.deviceName();
    Json["Device_connection"] = !myve.veError;
    Json["Remote_Control_State"] = remoteControlState;
    setEspDataValue("IP", WiFi.localIP().toString());
    setEspDataValue("Wifi_RSSI", WiFi.RSSI());
    setEspDataValue("VE_RX_Overflow", veSerialOverflowCount);
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
  pruneUnknownMqttKeys();
  Json["Device_name"] = _settings.get.deviceName();
  applyBufferedTemperatureData();
  setEspDataValue("IP", WiFi.localIP().toString());
  setEspDataValue("sw_version", SOFTWARE_VERSION);
  setEspDataValue("ESP_VCC", (ESP.getVcc() / 1000.0) + 0.3);
  setEspDataValue("Wifi_RSSI", WiFi.RSSI());
  setEspDataValue("Free_Heap", ESP.getFreeHeap());
  setEspDataValue("json_size", measureJson(Json));
  setEspDataValue("WS_Clients", ws.count());
  setEspDataValue("Runtime", millis() / 1000);
  setEspDataValue("VE_RX_Overflow", veSerialOverflowCount);
  writeLog("VE data: %d:%d:%d", myve.veEnd, myve.veErrorCount, myve.veError);
  for (size_t i = 0; i < myve.veEnd; i++)
  {

    if (strlen(myve.veName[i]) == 0 || strlen(myve.veValue[i]) == 0)
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

  }
  Json["Device_connection"] = !myve.veError;
  Json["Remote_Control_State"] = remoteControlState;
  return true;
}

bool connectMQTT()
{
  if (!mqttclient.connected())
  {
    const String aliveTopic = buildTopicPath(topic, "Alive");
    if (mqttclient.connect(mqttClientId, _settings.get.mqttUser(), _settings.get.mqttPassword(), aliveTopic.c_str(), 0, true, "false", true))
    {
      const String ipTopic = buildTopicPath(topic, "IP");
      const String remoteTopic = buildTopicPath(topic, "Remote_Control");
      const String ipPayload = WiFi.localIP().toString();
      mqttclient.publish(ipTopic.c_str(), ipPayload.c_str(), true);
      mqttclient.publish(aliveTopic.c_str(), "true", true); // LWT online message must be retained!
      mqttclient.subscribe(remoteTopic.c_str());

      const char *triggerPath = _settings.get.mqttTriggerPath();
      if (strlen(triggerPath) > 0)
      {
        writeLog("MQTT Data Trigger Subscribed");
        mqttclient.subscribe(triggerPath);
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
  Json["Device_name"] = _settings.get.deviceName();
  Json["Device_connection"] = !myve.veError;
  Json["Remote_Control_State"] = remoteControlState;
  applyBufferedTemperatureData();
  setEspDataValue("IP", WiFi.localIP().toString());
  setEspDataValue("sw_version", SOFTWARE_VERSION);
  setEspDataValue("ESP_VCC", (ESP.getVcc() / 1000.0) + 0.3);
  setEspDataValue("Wifi_RSSI", WiFi.RSSI());
  setEspDataValue("Free_Heap", ESP.getFreeHeap());
  setEspDataValue("WS_Clients", ws.count());
  setEspDataValue("Runtime", millis() / 1000);
  setEspDataValue("VE_RX_Overflow", veSerialOverflowCount);
  setEspDataValue("json_size", measureJson(Json));
  pruneUnknownMqttKeys();

  const String aliveTopic = buildTopicPath(topic, "Alive");

  //-----------------------------------------------------
  mqttclient.publish(aliveTopic.c_str(), "true", true); // LWT online message must be retained!

  if (!_settings.get.mqttJson())
  {
    for (JsonPair i : Json.as<JsonObject>())
    {
      const char *rootKey = i.key().c_str();
      if (!isKnownRootKey(rootKey))
      {
        writeLog("[MQTT] drop unknown key '%s'", rootKey);
        continue;
      }

      if (strcmp(rootKey, "ESP_Data") == 0)
      {
        if (!i.value().is<JsonObject>())
        {
          writeLog("[MQTT] drop invalid object '%s'", rootKey);
          continue;
        }

        if (!publishEspDataAsFlatTopics(topic, i.value().as<JsonObjectConst>()))
          return false;
        continue;
      }

      const String cleanKey = sanitizeMqttSegment(rootKey);
      if (cleanKey.length() == 0)
      {
        writeLog("[MQTT] drop invalid key '%s'", rootKey);
        continue;
      }

      const String valueTopic = buildTopicPath(topic, cleanKey.c_str());
      if (!publishJsonAsFlatTopics(valueTopic, i.value().as<JsonVariantConst>()))
        return false;
    }
  }
  else
  {
    const String dataTopic = buildTopicPath(topic, "DATA");
    mqttclient.beginPublish(dataTopic.c_str(), measureJson(Json), false);
    serializeJson(Json, mqttclient);
    mqttclient.endPublish();
  }

  return true;
}

void mqttCallback(char *top, byte *payload, unsigned int length) // Need rework
{
  String messageTemp;
  messageTemp.reserve(length);

  for (unsigned int i = 0; i < length; i++)
  {
    messageTemp += (char)payload[i];
  }

  const char *triggerPath = _settings.get.mqttTriggerPath();
  if (strlen(triggerPath) > 0 && strcmp(top, triggerPath) == 0)
  {
    writeLog("MQTT Data Trigger Firered Up");
    mqtttimer = 0;
  }

  const String remoteTopic = buildTopicPath(topic, "Remote_Control");
  if (strcmp(top, remoteTopic.c_str()) == 0)
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

  const String availabilityTopic = buildTopicPath(topic, "Alive");
  const String rcCommandTopic = buildTopicPath(topic, "Remote_Control");
  const String rcStateTopic = buildTopicPath(topic, "Remote_Control_State");
  const String deviceModel = Json["Device_model"].as<String>();

  String haDeviceDescription = String("\"dev\":") +
                               "{\"ids\":[\"" + mqttClientId + "\"]," +
                               "\"name\":\"" + String(_settings.get.deviceName()) + "\"," +
                               "\"cu\":\"http://" + WiFi.localIP().toString() + "\"," +
                               "\"mdl\":\"" + deviceModel + "\"," +
                               "\"mf\":\"SoftWareCrash\"," +
                               "\"sw\":\"" + SOFTWARE_VERSION + "\"" +
                               "}";

  char topBuff[128];
  for (size_t i = 0; i < sizeof haDescriptor / sizeof haDescriptor[0]; i++)
  {
    const char *stateName = (const char *)pgm_read_ptr(&haDescriptor[i][0]);
    const char *icon = (const char *)pgm_read_ptr(&haDescriptor[i][1]);
    const char *unit = (const char *)pgm_read_ptr(&haDescriptor[i][2]);
    const char *devClass = (const char *)pgm_read_ptr(&haDescriptor[i][3]);
    const String cleanStateName = sanitizeMqttSegment(stateName);

    if (cleanStateName.length() == 0 || Json[stateName].isNull())
      continue;

    const String stateTopic = buildTopicPath(topic, cleanStateName.c_str());

    String haPayLoad = String("{") +
                       "\"name\":\"" + String(stateName) + "\"," +
                       "\"stat_t\":\"" + stateTopic + "\"," +
                       "\"avty_t\":\"" + availabilityTopic + "\"," +
                       "\"pl_avail\": \"true\"," +
                       "\"pl_not_avail\": \"false\"," +
                       "\"uniq_id\":\"" + String(mqttClientId) + "." + cleanStateName + "\"," +
                       "\"ic\":\"mdi:" + String(icon) + "\",";

    if (strlen(unit) != 0)
      haPayLoad += (String) "\"unit_of_meas\":\"" + unit + "\",";

    if (strcmp(unit, "kWh") == 0 || strcmp(unit, "Wh") == 0)
      haPayLoad += (String) "\"state_class\":\"total\",";
    if (strcmp(unit, "A") == 0 || strcmp(unit, "V") == 0 || strcmp(unit, "W") == 0)
      haPayLoad += (String) "\"state_class\":\"measurement\",";

    if (strlen(devClass) != 0)
      haPayLoad += (String) "\"dev_cla\":\"" + devClass + "\",";
    haPayLoad += haDeviceDescription;
    haPayLoad += "}";

    if (!buildHaDiscoveryTopic(topBuff, sizeof(topBuff), "sensor", cleanStateName.c_str()))
    {
      writeLog("[MQTT] HA sensor topic too long: %s", cleanStateName.c_str());
      continue;
    }

    mqttclient.beginPublish(topBuff, haPayLoad.length(), true);
    for (size_t k = 0; k < haPayLoad.length(); k++)
    {
      mqttclient.write(haPayLoad[k]);
    }
    mqttclient.endPublish();
  }

  for (size_t i = 0; i < MAX_TEMPERATURE_SENSORS; i++)
  {
    char stateName[16];
    snprintf(stateName, sizeof(stateName), "DS18B20_%u", (unsigned)(i + 1));
    if (getDs18ValueFromEspData(stateName).isNull())
      continue;

    const String cleanStateName = sanitizeMqttSegment(stateName);
    const String stateTopic = buildTopicPath(topic, cleanStateName.c_str());
    String haPayLoad = String("{") +
                       "\"name\":\"" + String(stateName) + "\"," +
                       "\"stat_t\":\"" + stateTopic + "\"," +
                       "\"avty_t\":\"" + availabilityTopic + "\"," +
                       "\"pl_avail\": \"true\"," +
                       "\"pl_not_avail\": \"false\"," +
                       "\"uniq_id\":\"" + String(mqttClientId) + "." + cleanStateName + "\"," +
                       "\"ic\":\"mdi:thermometer\"," +
                       "\"unit_of_meas\":\"\xC2\xB0"
                       "C\"," +
                       "\"state_class\":\"measurement\"," +
                       "\"dev_cla\":\"temperature\",";
    haPayLoad += haDeviceDescription;
    haPayLoad += "}";

    if (!buildHaDiscoveryTopic(topBuff, sizeof(topBuff), "sensor", cleanStateName.c_str()))
    {
      writeLog("[MQTT] HA sensor topic too long: %s", cleanStateName.c_str());
      continue;
    }

    mqttclient.beginPublish(topBuff, haPayLoad.length(), true);
    for (size_t k = 0; k < haPayLoad.length(); k++)
    {
      mqttclient.write(haPayLoad[k]);
    }
    mqttclient.endPublish();
  }

  // switch
  String haPayLoad = String("{") +
                     "\"name\":\"Remote_Control\"," +
                     "\"command_topic\":\"" + rcCommandTopic + "\"," +
                     "\"stat_t\":\"" + rcStateTopic + "\"," +
                     "\"uniq_id\":\"" + String(mqttClientId) + ".Remote_Control\"," +
                     "\"avty_t\":\"" + availabilityTopic + "\"," +
                     "\"pl_avail\": \"true\"," +
                     "\"pl_not_avail\": \"false\"," +
                     "\"ic\":\"mdi:toggle-switch-off\"," +
                     "\"pl_on\":\"true\"," +
                     "\"pl_off\":\"false\"," +
                     "\"stat_on\":\"true\"," +
                     "\"stat_off\":\"false\",";

  haPayLoad += haDeviceDescription;
  haPayLoad += "}";
  if (!buildHaDiscoveryTopic(topBuff, sizeof(topBuff), "switch", "Remote_Control"))
  {
    writeLog("[MQTT] HA switch topic too long");
    return false;
  }

  mqttclient.beginPublish(topBuff, haPayLoad.length(), true);
  for (size_t i = 0; i < haPayLoad.length(); i++)
  {
    mqttclient.write(haPayLoad[i]);
  }
  mqttclient.endPublish();

  return true;
}

void handleTemperatureSample(int deviceIndex, int32_t temperatureRAW)
{
  if (deviceIndex < 0 || deviceIndex >= MAX_TEMPERATURE_SENSORS)
    return;

  if (!isValidDs18TemperatureRaw(temperatureRAW))
    return;

  BufferedTemperatureState &state = bufferedTemperatures[deviceIndex];

  if (state.pending)
  {
    if (ds18RawDelta(state.pendingRaw, temperatureRAW) <= DS18_CONFIRM_DELTA_RAW)
    {
      state.pending = false;
      publishAcceptedTemperatureSample(deviceIndex, temperatureRAW);
      return;
    }

    state.pending = false;
  }

  if (!state.valid && temperatureRAW == DS18_POWER_ON_RAW)
  {
    holdSuspiciousTemperatureSample(deviceIndex, temperatureRAW);
    return;
  }

  if (state.valid && ds18RawDelta(state.raw, temperatureRAW) >= DS18_GLITCH_DELTA_RAW)
  {
    holdSuspiciousTemperatureSample(deviceIndex, temperatureRAW);
    return;
  }

  publishAcceptedTemperatureSample(deviceIndex, temperatureRAW);
}

void handleTemperatureDisconnect(int deviceIndex)
{
  if (deviceIndex < 0 || deviceIndex >= MAX_TEMPERATURE_SENSORS)
    return;

  writeLog("<DS18x> DS18B20_%d disconnected", deviceIndex + 1);
  bufferedTemperatures[deviceIndex].raw = DEVICE_DISCONNECTED_RAW;
  bufferedTemperatures[deviceIndex].valid = false;
  bufferedTemperatures[deviceIndex].dirty = true;
  bufferedTemperatures[deviceIndex].pending = false;
  bufferedTemperatures[deviceIndex].pendingRaw = DEVICE_DISCONNECTED_RAW;
  tempStateDirty = true;
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
