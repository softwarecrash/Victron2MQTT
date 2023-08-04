Little Program for ESP82XX to get the Victron data to web and MQTT

# Features:
- captive portal for wifi and MQTT config
- config in webinterface
- get essential data over webinterface, get all data like cell voltage and more over MQTT
- get Json over web at /livejson?
- firmware update over webinterface

**Main screen:**


**Settings:**


**Config:**

**MQTT Data**


# Connection to Victron:
![Victron Pinout MQTT-Projekt](https://github.com/softwarecrash/Victron2MQTT/assets/17761850/27f7120b-f118-4969-8f6f-53af4bb6eaff)


# How to use:
- flash the bin file to an ESP8266 (recommended Wemos D1 Mini) with [Tasmotizer](https://github.com/tasmota/tasmotizer/releases)
- connect the ESP like the wiring diagram
- search for the wifi ap "Victron2MQTT-AP" and connect to it
- surf to 192.168.4.1 and set up your wifi and optional MQTT
- that's it :)

# Questions? 
[Join the Discord Channel (German / English)](https://discord.gg/WeWs7BwZQr)

#
[<img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"/>](https://donate.softwarecrash.de)

# 
[![LICENSE](https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png)](https://creativecommons.org/licenses/by-nc-sa/4.0/)
