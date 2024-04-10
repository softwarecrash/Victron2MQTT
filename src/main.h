#define ARDUINOJSON_USE_DOUBLE 0
#define ARDUINOJSON_USE_LONG_LONG 0

#define VICTRON_BAUD 19200   // baud rate for modbus
#define MYPORT_TX 12
#define MYPORT_RX 13
#define LED_PIN 02 //D4 with the LED on Wemos D1 Mini
#define JSON_BUFFER 2048

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

#define SOFTWARE_VERSION SWVERSION
#define DBG_BAUD 115200
#define DBG Serial
#define DBG_WEBLN(...) WebSerial.println(__VA_ARGS__)
#define DBG_SERIAL(...) DBG(__VA_ARGS__)
#define DBG_BEGIN(...) DBG.begin(__VA_ARGS__)
#define DBG_PRINTLN(...) DBG.println(__VA_ARGS__)

typedef struct {
  byte bootcount;
  bool remoteControlState;
} rtcData;

bool getJsonData();

void notifyClients();

void mqttCallback(char *top, byte *payload, unsigned int length);

bool sendtoMQTT();

/**
 * @brief function for uart callback to prozess avaible data
 * 
 */
void prozessData();

/***
 * 
 * 
*/
bool  sendHaDiscovery();

bool remoteControl(bool sw);

/**
 * @brief this function act like s/n/printf() and give the output to the configured serial and webserial
 *
 */
void writeLog(const char* format, ...);