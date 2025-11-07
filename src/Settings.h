#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

/*
  Settings.h — namespaced accessors (get./set.), single source of truth,
                self-healing, backup/restore.

  - Use:   _settings.get.rcState()
           _settings.set.rcState(true)
  - One central X-macro list defines all fields (key, name, default).
  - No CONFIG_VERSION: load() reads → appends missing defaults → saves if needed.
  - WebUI helpers: backup() and restore().
  - ArduinoJson 7 (generic JsonDocument), EEPROM header with CRC + MAGIC.
  - Comments in English.
*/

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 2048
#endif

// ---------------------------------------------------------------------------
// SINGLE SOURCE OF TRUTH
//
// Macros:
//   XX_STR (jsonKey, apiName, defaultStr)
//   XX_UINT(jsonKey, apiName, defaultUInt)
//   XX_BOOL(jsonKey, apiName, defaultBool)
//
// jsonKey = key inside JSON/EEPROM
// apiName = lowerCamel for .get.<apiName>() and .set.<apiName>(...)
// ---------------------------------------------------------------------------
#define SETTINGS_FIELDS(XX_STR, XX_UINT, XX_BOOL)                               \
  /* Identity & Network */                                                      \
  XX_STR ("deviceName",    deviceName,    "Victron2MQTT")                       \
  XX_STR ("staticIP",      staticIP,      "")                                   \
  XX_STR ("staticGW",      staticGW,      "")                                   \
  XX_STR ("staticSN",      staticSN,      "")                                   \
  XX_STR ("staticDNS",     staticDNS,     "")                                   \
                                                                                \
  /* MQTT */                                                                    \
  XX_STR ("mqttServer",    mqttServer,    "")                                   \
  XX_STR ("mqttUser",      mqttUser,      "")                                   \
  XX_STR ("mqttPassword",  mqttPassword,  "")                                   \
  XX_STR ("mqttTopic",     mqttTopic,     "Victron")                            \
  XX_STR ("mqttTrigger",   mqttTriggerPath, "")                                 \
  XX_UINT("mqttPort",      mqttPort,      0)                                    \
  XX_UINT("mqttRefresh",   mqttRefresh,   300)                                  \
  XX_BOOL("mqttJson",      mqttJson,      false)                                \
                                                                                \
  /* Web UI */                                                                  \
  XX_STR ("httpUser",      httpUser,      "")                                   \
  XX_STR ("httpPass",      httpPass,      "")                                   \
  XX_BOOL("webUIdarkmode", webUIdarkmode, false)                                \
                                                                                \
  /* Integrations / Flags */                                                    \
  XX_BOOL("haDiscovery",   haDiscovery,   false)                                \
  XX_BOOL("keepRcState",   keepRcState,   false)                                \
  XX_BOOL("rcState",       rcState,       false)                                \
  XX_UINT("LEDBrightness", LEDBrightness, 127)

// ---------------------------------------------------------------------------

class Settings {
public:
  // ========== Proxies ==========
  class GetProxy {
  public:
    explicit GetProxy(Settings* s) : s(s) {}

    // Auto-generated getters
    #define GEN_GETTER_STR(jsonKey, apiName, d) \
      const char* apiName() const { return s->getStr(jsonKey, d, s->_tmp_##apiName); }
    #define GEN_GETTER_UINT(jsonKey, apiName, d) \
      uint16_t apiName() const { return (uint16_t)s->getUInt(jsonKey, (uint32_t)(d)); }
    #define GEN_GETTER_BOOL(jsonKey, apiName, d) \
      bool apiName() const { return s->getBool(jsonKey, d); }
    SETTINGS_FIELDS(GEN_GETTER_STR, GEN_GETTER_UINT, GEN_GETTER_BOOL)
    #undef GEN_GETTER_STR
    #undef GEN_GETTER_UINT
    #undef GEN_GETTER_BOOL

  private:
    Settings* s;
    friend class Settings;
  };

  class SetProxy {
  public:
    explicit SetProxy(Settings* s) : s(s) {}

    // Auto-generated setters (single naming): _settings.set.apiName(...)
    #define GEN_SETTER_STR(jsonKey, apiName, d) \
      void apiName(const String& v) { s->setStr(jsonKey, v); } \
      void apiName(const char* v)  { s->setStr(jsonKey, String(v ? v : "")); }
    #define GEN_SETTER_UINT(jsonKey, apiName, d) \
      void apiName(uint16_t v) { s->setUInt(jsonKey, (uint32_t)v); }
    #define GEN_SETTER_BOOL(jsonKey, apiName, d) \
      void apiName(bool v) { s->setBool(jsonKey, v); }
    SETTINGS_FIELDS(GEN_SETTER_STR, GEN_SETTER_UINT, GEN_SETTER_BOOL)
    #undef GEN_SETTER_STR
    #undef GEN_SETTER_UINT
    #undef GEN_SETTER_BOOL

  private:
    Settings* s;
    friend class Settings;
  };

  GetProxy get{this};
  SetProxy set{this};

  // ========== Legacy DataView (optional, stays for compatibility) ==========
  struct DataView {
    #define GEN_VIEW_STR(jsonKey, apiName, d)  const char* apiName;
    #define GEN_VIEW_UINT(jsonKey, apiName, d) uint16_t    apiName;
    #define GEN_VIEW_BOOL(jsonKey, apiName, d) bool        apiName;
    SETTINGS_FIELDS(GEN_VIEW_STR, GEN_VIEW_UINT, GEN_VIEW_BOOL)
    #undef GEN_VIEW_STR
    #undef GEN_VIEW_UINT
    #undef GEN_VIEW_BOOL
  } data;

  // ========== Lifecycle ==========
  void load() {
    ensureEEP();
    readFromEEPROM();

    bool changed = ensureDefaultsFromList();  // append missing keys only

    // Non-destructive sanity rules
    if (get.mqttRefresh() < 1) { set.mqttRefresh(1); changed = true; }

    if (changed) writeToEEPROM();
    buildView();
  }

  void save() {
    if (get.mqttRefresh() < 1) set.mqttRefresh(1);
    writeToEEPROM();
    buildView();
  }

  void reset() {
    _doc.clear();
    writeDefaultsFromList();
    writeToEEPROM();
    buildView();
  }

  // ========== WebUI helpers ==========
  // Export known settings as JSON (stable ordering). pretty=true enables pretty-print.
  String backup(bool pretty = false) const {
    JsonDocument out;
    exportKnownTo(out);
    String s;
    if (pretty) serializeJsonPretty(out, s);
    else        serializeJson(out, s);
    return s;
  }

  // Restore settings from JSON string.
  // merge=true  → only provided keys overwrite existing; others untouched.
  // merge=false → replace known keys from input; unknown ignored; then fill missing defaults.
  // saveNow=true → persist EEPROM immediately on success.
  // err → optional error text if parse fails.
  bool restore(const String& json, bool merge = true, bool saveNow = true, String* err = nullptr) {
    JsonDocument in;
    DeserializationError de = deserializeJson(in, json);
    if (de) {
      if (err) *err = String("JSON parse error: ") + de.c_str();
      return false;
    }

    bool changed = false;
    if (merge) {
      changed |= mergeKnownFrom(in);
    } else {
      clearKnown();
      changed |= mergeKnownFrom(in);
      changed |= ensureDefaultsFromList();
    }

    // Sanity again
    if (get.mqttRefresh() < 1) { set.mqttRefresh(1); changed = true; }

    if (changed && saveNow) {
      writeToEEPROM();
    }
    buildView();  // reflect in-memory state in any case
    return true;
  }

  String deviceNameStr; // convenience copy (optional)

private:
  // EEPROM header (stable)
  struct Header {
    uint16_t version;  // compatibility only (not used for schema)
    uint16_t jsonLen;
    uint32_t crc;
    uint32_t magic;
  };

  static constexpr uint32_t MAGIC = 0x53435721UL; // "SCW!"
  static constexpr int HEADER_SIZE = sizeof(Header);

  // ArduinoJson 7: generic document (no fixed capacity)
  JsonDocument _doc;

  // String caches for STR getters (keep const char* stable)
  #define GEN_TMP_STR(jsonKey, apiName, d) mutable String _tmp_##apiName;
  #define NOOP_UINT(jsonKey, apiName, d)
  #define NOOP_BOOL(jsonKey, apiName, d)
  SETTINGS_FIELDS(GEN_TMP_STR, NOOP_UINT, NOOP_BOOL)
  #undef GEN_TMP_STR
  #undef NOOP_UINT
  #undef NOOP_BOOL

  // ---------- Helpers ----------
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

  // Write full defaults from list
  void writeDefaultsFromList() {
    #define APPLY_DEFAULT_STR(jsonKey, apiName, d) _doc[jsonKey] = d;
    #define APPLY_DEFAULT_UINT(jsonKey, apiName, d) _doc[jsonKey] = (uint32_t)(d);
    #define APPLY_DEFAULT_BOOL(jsonKey, apiName, d) _doc[jsonKey] = (bool)(d);
    SETTINGS_FIELDS(APPLY_DEFAULT_STR, APPLY_DEFAULT_UINT, APPLY_DEFAULT_BOOL)
    #undef APPLY_DEFAULT_STR
    #undef APPLY_DEFAULT_UINT
    #undef APPLY_DEFAULT_BOOL
  }

  // Append missing keys (no overwrite). Returns changed flag.
  bool ensureDefaultsFromList() {
    bool changed = false;

    #define ENSURE_STR(jsonKey, apiName, d) \
      if (_doc[jsonKey].isNull()) { _doc[jsonKey] = d; changed = true; }

    #define ENSURE_UINT(jsonKey, apiName, d) \
      if (_doc[jsonKey].isNull()) { _doc[jsonKey] = (uint32_t)(d); changed = true; } \
      else if (!_doc[jsonKey].is<unsigned long>()) { \
        const char* s = _doc[jsonKey].as<const char*>(); \
        if (s) { char* e = nullptr; long v = strtol(s, &e, 10); \
          if (e && *e == '\0' && v >= 0) { _doc[jsonKey] = (uint32_t)v; changed = true; } } \
      }

    #define ENSURE_BOOL(jsonKey, apiName, d) \
      if (_doc[jsonKey].isNull()) { _doc[jsonKey] = (bool)(d); changed = true; } \
      else if (!_doc[jsonKey].is<bool>()) { \
        const char* s = _doc[jsonKey].as<const char*>(); \
        if (s) { _doc[jsonKey] = parseBool(s); changed = true; } \
      }

    SETTINGS_FIELDS(ENSURE_STR, ENSURE_UINT, ENSURE_BOOL)

    #undef ENSURE_STR
    #undef ENSURE_UINT
    #undef ENSURE_BOOL

    return changed;
  }

  static bool parseBool(const char* s) {
    if (!s) return false;
    if (!strcasecmp(s, "true"))  return true;
    if (!strcasecmp(s, "false")) return false;
    return atoi(s) != 0; // accept "1"/"0"
  }

  // Export known keys into 'out' (stable ordering)
  void exportKnownTo(JsonDocument& out) const {
    #define COPY_OUT_STR(jsonKey, apiName, d) \
      out[jsonKey] = safeCStr(_doc[jsonKey].as<const char*>(), d);
    #define COPY_OUT_UINT(jsonKey, apiName, d) \
      out[jsonKey] = _doc[jsonKey].isNull() ? (uint32_t)(d) : (uint32_t)_doc[jsonKey].as<unsigned long>();
    #define COPY_OUT_BOOL(jsonKey, apiName, d) \
      out[jsonKey] = _doc[jsonKey].isNull() ? (bool)(d) : _doc[jsonKey].as<bool>();
    SETTINGS_FIELDS(COPY_OUT_STR, COPY_OUT_UINT, COPY_OUT_BOOL)
    #undef COPY_OUT_STR
    #undef COPY_OUT_UINT
    #undef COPY_OUT_BOOL
  }

  // Merge known keys from 'in' (returns changed flag)
  bool mergeKnownFrom(JsonDocument& in) {
    bool changed = false;

    #define MERGE_STR(jsonKey, apiName, d) \
      if (!in[jsonKey].isNull()) { \
        const char* nv = in[jsonKey].as<const char*>(); \
        const char* ov = _doc[jsonKey].as<const char*>(); \
        if (!ov || !nv || strcmp(nv, ov) != 0) { _doc[jsonKey] = nv ? nv : d; changed = true; } \
      }

    #define MERGE_UINT(jsonKey, apiName, d) \
      if (!in[jsonKey].isNull()) { \
        uint32_t nv = in[jsonKey].as<unsigned long>(); \
        uint32_t ov = _doc[jsonKey].isNull() ? (uint32_t)(d) : (uint32_t)_doc[jsonKey].as<unsigned long>(); \
        if (nv != ov) { _doc[jsonKey] = nv; changed = true; } \
      }

    #define MERGE_BOOL(jsonKey, apiName, d) \
      if (!in[jsonKey].isNull()) { \
        bool nv = in[jsonKey].as<bool>(); \
        bool ov = _doc[jsonKey].isNull() ? (bool)(d) : _doc[jsonKey].as<bool>(); \
        if (nv != ov) { _doc[jsonKey] = nv; changed = true; } \
      }

    SETTINGS_FIELDS(MERGE_STR, MERGE_UINT, MERGE_BOOL)

    #undef MERGE_STR
    #undef MERGE_UINT
    #undef MERGE_BOOL

    return changed;
  }

  // Clear known keys (keeps unknown/private keys intact)
  void clearKnown() {
    #define CLEAR_KEY(jsonKey, apiName, d) _doc.remove(jsonKey);
    SETTINGS_FIELDS(CLEAR_KEY, CLEAR_KEY, CLEAR_KEY)
    #undef CLEAR_KEY
  }

  static const char* safeCStr(const char* s, const char* dflt) {
    return s ? s : dflt;
  }

  void buildView() {
    #define FILL_VIEW_STR(jsonKey, apiName, d)  data.apiName = get.apiName();
    #define FILL_VIEW_UINT(jsonKey, apiName, d) data.apiName = get.apiName();
    #define FILL_VIEW_BOOL(jsonKey, apiName, d) data.apiName = get.apiName();
    SETTINGS_FIELDS(FILL_VIEW_STR, FILL_VIEW_UINT, FILL_VIEW_BOOL)
    #undef FILL_VIEW_STR
    #undef FILL_VIEW_UINT
    #undef FILL_VIEW_BOOL

    deviceNameStr = get.deviceName();
  }

  // ---------- EEPROM R/W ----------
  void writeToEEPROM() {
    String payload;
    serializeJson(_doc, payload);

    const uint16_t maxPayload = EEPROM_SIZE - HEADER_SIZE;
    if (payload.length() > maxPayload) {
      // Too large: reset to defaults + marker
      writeDefaultsFromList();
      _doc["oversize"] = true;
      payload = String();
      serializeJson(_doc, payload);
    }

    Header h;
    h.version = 1; // compatibility; not used for schema
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

    if (h.magic != MAGIC || h.jsonLen == 0 || h.jsonLen > (EEPROM_SIZE - HEADER_SIZE)) {
      writeDefaultsFromList();
      writeToEEPROM();
      return;
    }

    String payload; payload.reserve(h.jsonLen + 1);
    for (uint16_t i = 0; i < h.jsonLen; ++i) payload += char(EEPROM.read(addr + i));

    const uint32_t c = crc32((const uint8_t*)payload.c_str(), h.jsonLen);
    if (c != h.crc) {
      writeDefaultsFromList();
      writeToEEPROM();
      return;
    }

    _doc.clear();
    if (deserializeJson(_doc, payload)) {
      writeDefaultsFromList();
      writeToEEPROM();
      return;
    }
  }

  static void writeBytes(int addr, const uint8_t* buf, size_t n) {
    for (size_t i = 0; i < n; ++i) EEPROM.write(addr + i, buf[i]);
  }
  static void readBytes(int addr, uint8_t* buf, size_t n) {
    for (size_t i = 0; i < n; ++i) buf[i] = EEPROM.read(addr + i);
  }

  // ---------- JSON access helpers ----------
  const char* getStr(const char* key, const char* dflt, String& cache) const {
    const char* v = _doc[key].isNull() ? dflt : _doc[key].as<const char*>();
    cache = v ? v : dflt;
    return cache.c_str();
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
};

