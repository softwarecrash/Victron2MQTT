#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

// Gesamter EEPROM-Bereich für Settings (Bytes).
// Wenn deine JSON mal größer wird: hier auf 4096 erhöhen.
#ifndef EEPROM_SIZE
#define EEPROM_SIZE 2048
#endif

class Settings {
public:
  // Bei Schemaänderungen erhöhen -> Defaults werden ergänzt/angepasst
  static const uint16_t CONFIG_VERSION = 13;

  // ------- Getter (keine festen Längen, nur EEPROM limitiert) -------
  const char* deviceName()       const { return getStr("deviceName", "Victron2MQTT"); }
  const char* mqttServer()       const { return getStr("mqttServer",  ""); }
  const char* mqttUser()         const { return getStr("mqttUser",    ""); }
  const char* mqttPassword()     const { return getStr("mqttPassword",""); }
  const char* mqttTopic()        const { return getStr("mqttTopic",   "Victron"); }
  const char* mqttTriggerPath()  const { return getStr("mqttTrigger", ""); }
  const char* httpUser()         const { return getStr("httpUser",    ""); }
  const char* httpPass()         const { return getStr("httpPass",    ""); }
  const char* staticIP()         const { return getStr("staticIP",    ""); }
  const char* staticGW()         const { return getStr("staticGW",    ""); }
  const char* staticSN()         const { return getStr("staticSN",    ""); }
  const char* staticDNS()        const { return getStr("staticDNS",   ""); }

  uint16_t mqttPort()            const { return (uint16_t)getUInt("mqttPort", 0); }
  uint16_t mqttRefresh()         const { return (uint16_t)getUInt("mqttRefresh", 300); }
  bool     mqttJson()            const { return getBool("mqttJson", false); }
  bool     webUIdarkmode()       const { return getBool("webUIdarkmode", false); }
  bool     haDiscovery()         const { return getBool("haDiscovery", false); }
  bool     keepRcState()         const { return getBool("keepRcState", false); }
  bool     rcState()             const { return getBool("rcState", false); }
  uint8_t  LEDBrightness()       const { return (uint8_t)getUInt("LEDBrightness", 127); }

  // ------- Setter -------
  void setDeviceName(const String& v)      { setStr("deviceName", v); }
  void setMqttServer(const String& v)      { setStr("mqttServer", v); }
  void setMqttUser(const String& v)        { setStr("mqttUser", v); }
  void setMqttPassword(const String& v)    { setStr("mqttPassword", v); }
  void setMqttTopic(const String& v)       { setStr("mqttTopic", v); }
  void setMqttTriggerPath(const String& v) { setStr("mqttTrigger", v); }
  void setHttpUser(const String& v)        { setStr("httpUser", v); }
  void setHttpPass(const String& v)        { setStr("httpPass", v); }
  void setStaticIP(const String& v)        { setStr("staticIP", v); }
  void setStaticGW(const String& v)        { setStr("staticGW", v); }
  void setStaticSN(const String& v)        { setStr("staticSN", v); }
  void setStaticDNS(const String& v)       { setStr("staticDNS", v); }
  void setMqttPort(uint16_t v)             { setUInt("mqttPort", v); }
  void setMqttRefresh(uint16_t v)          { setUInt("mqttRefresh", max<uint16_t>(1, v)); }
  void setMqttJson(bool v)                 { setBool("mqttJson", v); }
  void setWebUIdarkmode(bool v)            { setBool("webUIdarkmode", v); }
  void setHaDiscovery(bool v)              { setBool("haDiscovery", v); }
  void setKeepRcState(bool v)              { setBool("keepRcState", v); }
  void setRcState(bool v)                  { setBool("rcState", v); }
  void setLEDBrightness(uint8_t v)         { setUInt("LEDBrightness", v); }

  // ------- Kompatibles Read-Only View (für Altstellen) -------
  struct DataView {
    const char* deviceName;
    const char* mqttServer;
    const char* mqttUser;
    const char* mqttPassword;
    const char* mqttTopic;
    const char* mqttTriggerPath;
    uint16_t    mqttPort;
    uint16_t    mqttRefresh;
    bool        mqttJson;
    bool        webUIdarkmode;
    const char* httpUser;
    const char* httpPass;
    bool        haDiscovery;
    bool        keepRcState;
    bool        rcState;
    uint8_t     LEDBrightness;
    const char* staticIP;
    const char* staticGW;
    const char* staticSN;
    const char* staticDNS;
  } data;

  // ------- Lifecycle -------
  void load() {
    ensureEEP();
    readFromEEPROM();
    applyDefaultsAndVersion();
    buildView();
  }

  void save() {
    if (mqttRefresh() < 1) setMqttRefresh(1);
    writeToEEPROM();
    buildView();
  }

  void reset() {
    _doc.clear();
    writeDefaults();
    writeToEEPROM();
    buildView();
  }

  String deviceNameStr;

private:
  struct Header {
    uint16_t version;
    uint16_t jsonLen;
    uint32_t crc;
    uint32_t magic;
  };

  static constexpr uint32_t MAGIC = 0x53435721UL; // "SCW!"
  static constexpr int HEADER_SIZE = sizeof(Header);

  // 👉 ArduinoJson 7: generischer Dokumenttyp ohne feste Kapazität
  JsonDocument _doc;

  // temporäre String-Puffer (damit const char* stabil bleibt bis zum nächsten Aufruf)
  mutable String _tmp_deviceName, _tmp_mqttServer, _tmp_mqttUser, _tmp_mqttPassword,
                 _tmp_mqttTopic, _tmp_mqttTrigger, _tmp_httpUser, _tmp_httpPass,
                 _tmp_staticIP, _tmp_staticGW, _tmp_staticSN, _tmp_staticDNS;

  // -------- Helpers --------
  static uint32_t crc32(const uint8_t* data, size_t len) {
    uint32_t crc = 0xffffffff;
    while (len--) {
      uint8_t c = *data++;
      for (uint32_t i = 0x80; i > 0; i >>= 1) {
        bool bit = crc & 0x80000000;
        if (c & i) bit = !bit;
        crc <<= 1;
        if (bit) crc ^= 0x04c11db7;
      }
    }
    return crc;
  }

  void ensureEEP() {
    static bool inited = false;
    if (!inited) { EEPROM.begin(EEPROM_SIZE); inited = true; }
  }

  void writeDefaults() {
    _doc.clear();
    _doc["deviceName"]   = "Victron2MQTT";
    _doc["mqttServer"]   = "";
    _doc["mqttUser"]     = "";
    _doc["mqttPassword"] = "";
    _doc["mqttTopic"]    = "Victron";
    _doc["mqttTrigger"]  = "";
    _doc["mqttPort"]     = 0;
    _doc["mqttRefresh"]  = 300;
    _doc["mqttJson"]     = false;
    _doc["webUIdarkmode"]= false;
    _doc["httpUser"]     = "";
    _doc["httpPass"]     = "";
    _doc["haDiscovery"]  = false;
    _doc["keepRcState"]  = false;
    _doc["rcState"]      = false;
    _doc["LEDBrightness"]= 127;
    _doc["staticIP"]     = "";
    _doc["staticGW"]     = "";
    _doc["staticSN"]     = "";
    _doc["staticDNS"]    = "";
    _doc["_coVers"]      = CONFIG_VERSION;
  }

  // Schlanke Migration: fehlende Keys werden ergänzt; vorhandene bleiben erhalten
void applyDefaultsAndVersion() {
  const uint16_t cur = _doc["_coVers"].isNull()
                         ? 0
                         : (uint16_t)_doc["_coVers"].as<unsigned long>();

  if (cur != CONFIG_VERSION) {
    // fehlende Keys setzen (vorhandene Werte bleiben erhalten)
    if (_doc["deviceName"].isNull())    _doc["deviceName"]    = "Victron2MQTT";
    if (_doc["mqttServer"].isNull())    _doc["mqttServer"]    = "";
    if (_doc["mqttUser"].isNull())      _doc["mqttUser"]      = "";
    if (_doc["mqttPassword"].isNull())  _doc["mqttPassword"]  = "";
    if (_doc["mqttTopic"].isNull())     _doc["mqttTopic"]     = "Victron";
    if (_doc["mqttTrigger"].isNull())   _doc["mqttTrigger"]   = "";
    if (_doc["mqttPort"].isNull())      _doc["mqttPort"]      = 0;
    if (_doc["mqttRefresh"].isNull())   _doc["mqttRefresh"]   = 300;
    if (_doc["mqttJson"].isNull())      _doc["mqttJson"]      = false;
    if (_doc["webUIdarkmode"].isNull()) _doc["webUIdarkmode"] = false;
    if (_doc["httpUser"].isNull())      _doc["httpUser"]      = "";
    if (_doc["httpPass"].isNull())      _doc["httpPass"]      = "";
    if (_doc["haDiscovery"].isNull())   _doc["haDiscovery"]   = false;
    if (_doc["keepRcState"].isNull())   _doc["keepRcState"]   = false;
    if (_doc["rcState"].isNull())       _doc["rcState"]       = false;
    if (_doc["LEDBrightness"].isNull()) _doc["LEDBrightness"] = 127;
    if (_doc["staticIP"].isNull())      _doc["staticIP"]      = "";
    if (_doc["staticGW"].isNull())      _doc["staticGW"]      = "";
    if (_doc["staticSN"].isNull())      _doc["staticSN"]      = "";
    if (_doc["staticDNS"].isNull())     _doc["staticDNS"]     = "";

    _doc["_coVers"] = CONFIG_VERSION;
    writeToEEPROM();
  }
  deviceNameStr = deviceName();
}

  void buildView() {
    data.deviceName       = deviceName();
    data.mqttServer       = mqttServer();
    data.mqttUser         = mqttUser();
    data.mqttPassword     = mqttPassword();
    data.mqttTopic        = mqttTopic();
    data.mqttTriggerPath  = mqttTriggerPath();
    data.mqttPort         = mqttPort();
    data.mqttRefresh      = mqttRefresh();
    data.mqttJson         = mqttJson();
    data.webUIdarkmode    = webUIdarkmode();
    data.httpUser         = httpUser();
    data.httpPass         = httpPass();
    data.haDiscovery      = haDiscovery();
    data.keepRcState      = keepRcState();
    data.rcState          = rcState();
    data.LEDBrightness    = LEDBrightness();
    data.staticIP         = staticIP();
    data.staticGW         = staticGW();
    data.staticSN         = staticSN();
    data.staticDNS        = staticDNS();
  }

  void writeToEEPROM() {
    String payload;
    serializeJson(_doc, payload);

    const uint16_t maxPayload = EEPROM_SIZE - HEADER_SIZE;
    if (payload.length() > maxPayload) {
      // JSON zu groß für EEPROM → Fallback: Defaults + Hinweis
      writeDefaults();
      _doc["oversize"] = true; // optionaler Marker
      payload = String();
      serializeJson(_doc, payload);
    }

    Header h;
    h.version = CONFIG_VERSION;
    h.jsonLen = (uint16_t)payload.length();
    h.magic   = MAGIC;
    h.crc     = crc32((const uint8_t*)payload.c_str(), h.jsonLen);

    int addr = 0;
    writeBytes(addr, (const uint8_t*)&h, sizeof(h)); addr += sizeof(h);
    for (uint16_t i = 0; i < h.jsonLen; ++i) EEPROM.write(addr + i, (uint8_t)payload[i]);
    EEPROM.commit();
  }

  void readFromEEPROM() {
    Header h;
    int addr = 0;
    readBytes(addr, (uint8_t*)&h, sizeof(h)); addr += sizeof(h);

    if (h.magic != MAGIC || h.version == 0 || h.jsonLen > (EEPROM_SIZE - HEADER_SIZE)) {
      writeDefaults();
      writeToEEPROM();
      return;
    }

    String payload; payload.reserve(h.jsonLen + 1);
    for (uint16_t i = 0; i < h.jsonLen; ++i) payload += char(EEPROM.read(addr + i));

    const uint32_t c = crc32((const uint8_t*)payload.c_str(), h.jsonLen);
    if (c != h.crc) {
      writeDefaults();
      writeToEEPROM();
      return;
    }

    _doc.clear();
    if (deserializeJson(_doc, payload)) {
      writeDefaults();
      writeToEEPROM();
    }
  }

  static void writeBytes(int addr, const uint8_t* buf, size_t n) {
    for (size_t i = 0; i < n; ++i) EEPROM.write(addr + i, buf[i]);
  }
  static void readBytes(int addr, uint8_t* buf, size_t n) {
    for (size_t i = 0; i < n; ++i) buf[i] = EEPROM.read(addr + i);
  }

  // ------- JSON Access Helpers -------
  const char* getStr(const char* key, const char* dflt) const {
    String& slot = tmpForKey(key);
    slot = _doc[key].isNull() ? dflt : _doc[key].as<const char*>();
    return slot.c_str();
  }
  bool getBool(const char* key, bool dflt) const {
    return _doc[key].isNull() ? dflt : _doc[key].as<bool>();
  }
  uint32_t getUInt(const char* key, uint32_t dflt) const {
    return _doc[key].isNull() ? dflt : (uint32_t)_doc[key].as<unsigned long>();
  }

  void setStr(const char* key, const String& v) { _doc[key] = v; }
  void setBool(const char* key, bool v)         { _doc[key] = v; }
  void setUInt(const char* key, uint32_t v)     { _doc[key] = v; }

  String& tmpForKey(const char* key) const {
    if      (!strcmp(key, "deviceName")) return _tmp_deviceName;
    else if (!strcmp(key, "mqttServer")) return _tmp_mqttServer;
    else if (!strcmp(key, "mqttUser"))   return _tmp_mqttUser;
    else if (!strcmp(key, "mqttPassword")) return _tmp_mqttPassword;
    else if (!strcmp(key, "mqttTopic"))  return _tmp_mqttTopic;
    else if (!strcmp(key, "mqttTrigger"))return _tmp_mqttTrigger;
    else if (!strcmp(key, "httpUser"))   return _tmp_httpUser;
    else if (!strcmp(key, "httpPass"))   return _tmp_httpPass;
    else if (!strcmp(key, "staticIP"))   return _tmp_staticIP;
    else if (!strcmp(key, "staticGW"))   return _tmp_staticGW;
    else if (!strcmp(key, "staticSN"))   return _tmp_staticSN;
    else if (!strcmp(key, "staticDNS"))  return _tmp_staticDNS;
    return _tmp_deviceName;
  }
};
