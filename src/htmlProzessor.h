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
        return (_settings.get.deviceName());
    if (var == F("pre_darkmode"))
        return (_settings.get.webUIdarkmode() ? "dark" : "light");
    return String();
}
