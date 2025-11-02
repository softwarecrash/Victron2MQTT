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
        return (_settings.deviceName());
/*     if (var == F("pre_mqtt_server"))
        return (_settings.mqttServer());
    if (var == F("pre_mqtt_port"))
        return (String(_settings.mqttPort()).c_str());
    if (var == F("pre_mqtt_user"))
        return (_settings.mqttUser());
    if (var == F("pre_mqtt_pass"))
        return (_settings.mqttPassword());
    if (var == F("pre_mqtt_topic"))
        return (_settings.mqttTopic());
    if (var == F("pre_mqtt_refresh"))
        return (String(_settings.mqttRefresh()).c_str());
    if (var == F("pre_mqtt_json"))
        return (_settings.mqttJson() ? "checked" : "");
    if (var == F("pre_mqtt_mqtttrigger")) 
        return (_settings.mqttTriggerPath());*/
    if (var == F("pre_darkmode"))
        return (_settings.webUIdarkmode() ? "dark" : "light");
   /*  if (var == F("pre_webuidarkmode"))
        return (_settings.webUIdarkmode() ? "checked" : "");
    if (var == F("pre_http_user"))
        return (_settings.httpUser());
    if (var == F("pre_http_pass"))
        return (_settings.httpPass());
    if (var == F("pre_hadiscovery"))
        return (_settings.haDiscovery() ? "checked" : "");
    if (var == F("pre_keeprcstate"))
        return (_settings.keepRcState() ? "checked" : "");
    if (var == F("pre_led"))
        return (String(_settings.LEDBrightness()));
    if (var == F("pre_static_ip"))
        return (_settings.staticIP());
    if (var == F("pre_static_gw"))
        return (_settings.staticGW());
    if (var == F("pre_static_sn"))
        return (_settings.staticSN());
    if (var == F("pre_static_dns"))
        return (_settings.staticDNS()); */
    return String();
}
