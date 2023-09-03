#define ARDUINOJSON_USE_DOUBLE 0
#define ARDUINOJSON_USE_LONG_LONG 0

#define VICTRON_BAUD 19200   // baud rate for modbus
#define MYPORT_TX 12
#define MYPORT_RX 13
#define LED_PIN 02 //D4 with the LED on Wemos D1 Mini

#define DEBUG_BAUD 115200

#define JSON_BUFFER 4096
#define MQTT_BUFFER 512

#define FlashSize ESP.getFreeSketchSpace()
#define ESP01
#ifdef ARDUINO_ESP8266_ESP01
#ifdef MYPORT_TX
#undef MYPORT_TX
#define MYPORT_TX 0
#endif
#ifdef MYPORT_RX
#undef MYPORT_RX
#define MYPORT_RX 2
#ifdef ESP01
#undef ESP01
#define ESP01 "display: none;"
#endif 
#endif
#endif

// DON'T edit version here, place version number in platformio.ini (custom_prog_version) !!!
#define SOFTWARE_VERSION SWVERSION

#define DEBUG_WEB(...) WebSerial.print(__VA_ARGS__)
#define DEBUG_WEBLN(...) WebSerial.println(__VA_ARGS__)
#define DEBUG_WEBF(...) WebSerial.printf(__VA_ARGS__)

#define DEBUG_SERIAL(...) Serial(__VA_ARGS__)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)


bool getJsonData();

void notifyClients();

void mqttCallback(char *top, byte *payload, unsigned int length);

bool sendtoMQTT();

/**
 * @brief function for uart callback to prozess avaible data
 * 
 */
void prozessData();