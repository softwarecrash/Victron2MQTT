# Victron2MQTT 
[![GitHub release](https://img.shields.io/github/release/softwarecrash/Victron2MQTT?include_prereleases=&sort=semver&color=blue)](https://github.com/softwarecrash/Victron2MQTT/releases/latest) ![Discord](https://img.shields.io/discord/1007020337482973254?logo=discord&label=Discord&link=https%3A%2F%2Fdiscord.gg%2F7gTJk22JDE)

Little Program for ESP82XX to get the Victron data to web and MQTT

# Features:
- captive portal for wifi and MQTT config
- config in webinterface
- get essential data over webinterface, get all data like cell voltage and more over MQTT
- get Json over web at /livejson?
- firmware update over webinterface

**Main screen:**

![grafik](https://github.com/softwarecrash/Victron2MQTT/assets/44615614/41786ae3-4ed0-44af-bc73-5b994b6cd211)


**Settings:**

![grafik](https://github.com/softwarecrash/Victron2MQTT/assets/44615614/6943ef5d-8711-4b32-bbd0-80278a6f50fa)


**Config:**

![grafik](https://github.com/softwarecrash/Victron2MQTT/assets/44615614/64aaa883-aee1-40b9-8e8a-ead788fbf70a)

**MQTT Data**

![grafik](https://github.com/softwarecrash/Victron2MQTT/assets/44615614/73eedc23-fc77-4034-934c-e8c123a9800e)


# Connection to Victron:

![Victron Pinout MQTT-Projekt](https://github.com/softwarecrash/Victron2MQTT/assets/17761850/dbbd37a9-d017-46ab-af72-68232d91544b)



# How to use:
- flash the bin file to an ESP8266 (recommended Wemos D1 Mini) with [Tasmotizer](https://github.com/tasmota/tasmotizer/releases)
- connect the ESP like the wiring diagram
- search for the wifi ap "Victron2MQTT-AP" and connect to it
- surf to 192.168.4.1 and set up your wifi and optional MQTT
- that's it :)

# Questions? 
[Join the Discord Channel (German / English)](https://discord.gg/7gTJk22JDE)

#
[<img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"/>](https://donate.softwarecrash.de)

# 
[![LICENSE](https://licensebuttons.net/l/by-nc-nd/4.0/88x31.png)](https://creativecommons.org/licenses/by-nc-nd/4.0/)
