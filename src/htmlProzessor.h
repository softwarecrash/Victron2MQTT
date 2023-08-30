// #include <Arduino.h>
String htmlProcessor(const String &var)
{
    extern Settings _settings;
    if (var == F("pre_head_template"))
        return (FPSTR(HTML_HEAD));
    if (var == F("pre_foot_template"))
        return (FPSTR(HTML_FOOT));
    if (var == F("pre_software_version"))
        return (SOFTWARE_VERSION);
    if (var == F("pre_swversion"))
        return (SWVERSION);
    if (var == F("pre_flash_size"))
        return (String(FlashSize).c_str());
    if (var == F("pre_esp01"))
        return (String(ESP01).c_str());
    if (var == F("pre_device_name"))
        return (_settings.data.deviceName);
    if (var == F("pre_mqtt_server"))
        return (_settings.data.mqttServer);
    if (var == F("pre_mqtt_port"))
        return (String(_settings.data.mqttPort).c_str());
    if (var == F("pre_mqtt_user"))
        return (_settings.data.mqttUser);
    if (var == F("pre_mqtt_pass"))
        return (_settings.data.mqttPassword);
    if (var == F("pre_mqtt_topic"))
        return (_settings.data.mqttTopic);
    if (var == F("pre_mqtt_refresh"))
        return (String(_settings.data.mqttRefresh).c_str());
    if (var == F("pre_mqtt_json"))
        return (_settings.data.mqttJson ? "checked" : "");
    if (var == F("pre_mqtt_mqtttrigger"))
        return (_settings.data.mqttTriggerPath);
    if (var == F("pre_darkmode"))
        return (_settings.data.webUIdarkmode ? "dark" : "light");
    if (var == F("pre_webuidarkmode"))
        return (_settings.data.webUIdarkmode ? "checked" : "");
    return String();
}
