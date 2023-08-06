#define ARDUINOJSON_USE_DOUBLE 0
#define ARDUINOJSON_USE_LONG_LONG 0

#define VICTRON_BAUD 19200   // baud rate for modbus
#define MYPORT_TX 12
#define MYPORT_RX 13
#define LED_PIN 02 //D4 with the LED on Wemos D1 Mini

#define DEBUG_BAUD 9600

#define JSON_BUFFER 2048
#define MQTT_BUFFER 512

//#define EPEVER_SERIAL Serial
//#define SERIAL_DEBUG Serial1

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

void callback(char *top, byte *payload, unsigned int length);

bool sendtoMQTT();