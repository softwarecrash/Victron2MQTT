/*
    Code to grab data from the VE.Direct-Protocol on Arduino / ESP8266.
    Tested on ESP01s

	  2021-02-16

    Added WiFi communication for ESP8266-01 (webserver).
    All MPPT (75/15 in my case) values displayed.
    Consts uppercased.

*/


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "user_interface.h"
#include "config.h"
#include "VeDirectFrameHandler.h"

// web server
ESP8266WebServer server(80);

IPAddress espip(192, 168, 0, 32);      // set the desired IP Address
IPAddress espgateway(192, 168, 0, 1);  // set gateway to match your network
IPAddress espsubnet(255, 255, 255, 0); // set subnet mask to match your network
const char* ssid     = "ssid";
const char* password = "pass";


VeDirectFrameHandler myve;

// The array that holds the verified values data (with MPPT_VALUES_BYTES size max for each value)
char value[VE_LastKeyword][MPPT_VALUES_BYTES]  = {0};

// via the USB serial provided by the NodeMCU.
unsigned long data_count = 0;
unsigned long reset_count = 0;

const short int BUILTIN_LED1 = 2; // blink while WiFi connection process

// enable ESP.getVcc() reading :
ADC_MODE(ADC_VCC);


////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(BUILTIN_LED1, OUTPUT); // Initialize the BUILTIN_LED1 pin as an output

  // Open VE.Direct serial communications and wait for port to open:
  Serial.begin(19200);
  delay(100);
  Serial.flush();

  // remove AP mode (client only):
  WiFi.mode(WIFI_STA);


  // WiFi configuration
  WiFi.config(espip, espgateway, espsubnet);
  WiFi.begin(ssid, password);

  // Wait for connected state
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BUILTIN_LED1, LOW); delay(100);
    digitalWrite(BUILTIN_LED1, HIGH); delay(100);
  }

  // set default page for "/" root URL
  server.on("/", handle_index);       //Handle Index page
  server.on("/raw.html", handle_raw); //Handle raw page
  //Start the server
  server.begin();

  reset_count = millis();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Receive information on Serial from MPPT
  ReadVEData();

  // Add your own code here to use the data.
  // Make sure to not used delay(X)s of bigger than 50ms

  // clients connections management
  server.handleClient();

  // reset toutes les heures :
  if ((millis() - reset_count) > 3600000) {
    ESP.restart();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void ReadVEData() {
  while ( Serial.available() ) {
    myve.rxData(Serial.read());
    data_count++;
  }
  yield();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
void handle_index() {
  server.send(200, "text/html", MPPTPage() );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
void handle_raw() {
  server.send(200, "text/plain", TextPage() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String TextPage() {
  String message = "ESP8266-01s VE.Direct";
  message += "\r\n";  message += "\r\n";

  message += "Voltage ";
  double vcc = (double) (ESP.getVcc() / 1024.00);
  message += vcc;
  message += "V\r\n";

  message += "Update #";
  message += myve.frameCount;
  message += "\r\n";
  message += "data_count ";
  message += data_count;
  message += "B\r\n";
  message += "Working since ";
  message += (int) (millis() / 1000);
  message += "s\r\n\r\n";

  for ( int i = 0; i < myve.veEnd; i++ ) {
    message += myve.veName[i];
    message += ": ";
    message += myve.veValue[i];
    message +=  "\r\n";
  }

  message +=  "\r\n";
  return message;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean isNumeric(char chStr[]) {
  unsigned int stringLength = strlen(chStr);
  if (stringLength == 0) {
    return false;
  }
  for (unsigned int i = 0; i < stringLength; ++i) {
    if (isDigit(chStr[i])) {
      continue;
    }
    if (chStr[i] == '.') {
      continue;
    }
    return false;
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
String MPPTPage() {
  
  // reset (default) values :
  for (unsigned int i = VE_Dummy; i < VE_LastKeyword; ++i) {
    value[i][0] = '\0';
  }

  // remplir le tableau de valeur
  for ( int i = 0; i < myve.veEnd; i++ ) {
    for (unsigned int j = VE_Dummy; j < VE_LastKeyword; ++j) {
      if (strcmp(myve.veName[i], keywords[j]) == 0) {
        strcpy(value[j], myve.veValue[i]);
        continue;
      }
    }
  }

  /*
     //test values :
     strcpy(value[VE_MPPT], "2");
     strcpy(value[VE_PPV], "51");
     strcpy(value[VE_VPV], "18350");
     strcpy(value[VE_V], "12250");
     strcpy(value[VE_I], "990");
     strcpy(value[VE_IL], "400");
     strcpy(value[VE_MPPT], "2");
     strcpy(value[VE_CS], "3");
  */


  String message = "<!DOCTYPE html>\r\n";
  message += "<html>\r\n";
  message += "<head>\r\n";
  message += "<title>ESP8266-VE.Direct</title>\r\n";
  message += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  message += "<style type=\"text/css\">\r\ntext {font-family: Verdana;}\r\n.w{fill: white;}\r\n.value{fill: white; font-size:14px; font-weight:bold; text-anchor:end;}\r\n.g10{fill: #bcdbf6; font-size:10px;}\r\n</style>\r\n";
  message += "</head>\r\n";
  message += "<body style=\"padding: 0px; margin: 0px;\">\r\n";

  message += "\r\n";
  message += "<svg width=\"100%\" viewBox=\"0 0 240 515\"><rect width=\"100%\" height=\"100%\" fill=\"#397dc6\" />\r\n";
  message += "<rect x=\"0\" y=\"60\" width=\"240\" height=\"130\" fill=\"#2e69a3\" />\r\n";
  message += "<rect x=\"0\" y=\"58\" width=\"240\" height=\"2\" fill=\"#1d5281\" />\r\n";
  message += "<rect x=\"0\" y=\"190\" width=\"240\" height=\"20\" fill=\"#4d8aca\" />\r\n";
  message += "<rect x=\"0\" y=\"290\" width=\"240\" height=\"20\" fill=\"#4d8aca\" />\r\n";
  message += "<rect x=\"0\" y=\"420\" width=\"240\" height=\"20\" fill=\"#4d8aca\" />\r\n";

  // draw PV
  message += "<polygon points=\"3,0 12,0 15,13 0,13\" style=\"fill:#bcdbf6;stroke:none;\" transform=\"translate(10,63)\" />\r\n";
  message += "<polygon points=\"3,0 12,0 15,13 0,13\" style=\"fill:#bcdbf6;stroke:none;\" transform=\"translate(10,220)\" />\r\n";
  message += "<polygon points=\"3,0 12,0 15,13 0,13\" style=\"fill:#bcdbf6;stroke:none;\" transform=\"translate(10,255)\" />\r\n";
  // draw battery
  message += "<polygon points=\"0,2 2,2 2,0 4,0 4,2 11,2 11,0 13,0 13,2 15,2 15,13 0,13\" style=\"fill:#bcdbf6;stroke:none;\" transform=\"translate(10,320)\" />\r\n";
  message += "<polygon points=\"0,2 2,2 2,0 4,0 4,2 11,2 11,0 13,0 13,2 15,2 15,13 0,13\" style=\"fill:#bcdbf6;stroke:none;\" transform=\"translate(10,355)\" />\r\n";
  message += "<polygon points=\"0,2 2,2 2,0 4,0 4,2 11,2 11,0 13,0 13,2 15,2 15,13 0,13\" style=\"fill:#bcdbf6;stroke:none;\" transform=\"translate(10,390)\" />\r\n";
  //draw load (lightning)
  message += "<polygon points=\"7,0 3,8 7,8 7,13 11,5 7,5\" style=\"fill:#bcdbf6;stroke:none;\" transform=\"translate(10,450)\" />\r\n";
  message += "<polygon points=\"7,0 3,8 7,8 7,13 11,5 7,5\" style=\"fill:#bcdbf6;stroke:none;\" transform=\"translate(10,485)\" />\r\n";


  // General (top) informations
  message += "<text class=\"w\" x=\"50%\" y=\"20\" font-size=\"12\" font-weight=\"bold\" text-anchor=\"middle\" >" + GetStringProduct(value[VE_PID]) + "</text>\r\n";
  int mppt = 0;
  if (isNumeric(value[VE_MPPT])) {
    mppt = (int) atoi(value[VE_MPPT]);
  }
  message += "<text class=\"g10\" x=\"50%\" y=\"35\" text-anchor=\"middle\" >" + GetStringTracker(mppt) + "</text>\r\n";
  double vcc = (double) (ESP.getVcc() / 1024.00);
  message += "<text class=\"g10\" x=\"50%\" y=\"50\" text-anchor=\"middle\" >Update #" + String(myve.frameCount) + " / " + String(vcc) + "Vcc</text>\r\n";


  // Solar (gauge) informations
  message += "<text class=\"g10\" x=\"30\" y=\"73\" >Solar</text>\r\n";
  // valeur entre 0 et 280 pour 220 Watt de puissance max (en mode 12V)
  // ou     entre 0 et 280 pour 440 Watt de puissance max (en mode 24V)
  message += "<circle r=\"60\" cx=\"50%\" cy=\"135\" stroke=\"#4d8aca\"  stroke-width=\"10\" stroke-dasharray=\"280, 380\" fill=\"none\" transform=\"rotate(137 120 135)\"> </circle>\r\n";
  int PVpower = 0;
  if (isNumeric(value[VE_PPV])) {
    PVpower = (int) atoi(value[VE_PPV]);
    message += "<circle r=\"60\" cx=\"50%\" cy=\"135\" stroke=\"#ffffff\"  stroke-width=\"5\" stroke-dasharray=\"" + String( (int) (280 * PVpower / 220)) + ", 380\" fill=\"none\" transform=\"rotate(137 120 135)\"> </circle>\r\n";
    message += "<text class=\"w\" x=\"145\" y=\"145\"  font-size=\"32\" text-anchor=\"end\" dominant-baseline=\"text-top\" >" + String(PVpower) + "</text>\r\n";
    message += "<text x=\"145\" y=\"145\" fill=\"#bcdbf6\" font-size=\"14\" text-anchor=\"start\" dominant-baseline=\"text-top\" >w</text>\r\n";
  }


  // Solar informations
  message += "<text class=\"g10\" x=\"10\" y=\"203\" >Solar</text>\r\n";
  message += "<text class=\"g10\" x=\"30\" y=\"230\" >Voltage</text>\r\n";
  float PVvoltage = 0;
  if (isNumeric(value[VE_VPV])) {
    PVvoltage =  (float) atoi(value[VE_VPV]) / 1000 ;
    message += "<text class=\"value\" x=\"230\" y=\"230\" >" + String(PVvoltage) + " V</text>\r\n";
  }
  message += "<text class=\"g10\" x=\"30\" y=\"265\" >Current</text>\r\n";
  float PVcurrent = 0;
  if (PVvoltage != 0) {
    PVcurrent = (float) PVpower / PVvoltage;
    message += "<text class=\"value\" x=\"230\" y=\"265\" >" + String(PVcurrent) + " A</text>\r\n";
  }

  // battery informations
  float Batvoltage = 0;
  if (isNumeric(value[VE_V])) {
    Batvoltage = (float) atoi(value[VE_V]) / 1000;
  }
  float Batcurrent = 0;
  if (isNumeric(value[VE_I])) {
    Batcurrent = (float) atoi(value[VE_I]) / 1000;
  }
  int ChargingState = 0;
  if (isNumeric(value[VE_CS])) {
    ChargingState = (int) atoi(value[VE_CS]);
  }
  message += "<text class=\"g10\" x=\"10\" y=\"303\" >Battery (" + GetStringBatteryVoltage(Batvoltage) + " system voltage)</text>\r\n";
  message += "<text class=\"g10\" x=\"30\" y=\"330\" >Voltage</text>\r\n";
  message += "<text class=\"value\" x=\"230\" y=\"330\" >" + String(Batvoltage) + " V</text>\r\n";
  message += "<text class=\"g10\" x=\"30\" y=\"365\" >Current</text>\r\n";
  message += "<text class=\"value\" x=\"230\" y=\"365\" >" + String(Batcurrent) + " A</text>\r\n";
  message += "<text class=\"g10\" x=\"30\" y=\"400\" >State</text>\r\n";
  message += "<text class=\"value\" x=\"230\" y=\"400\" >" + GetStringState(ChargingState) + "</text>\r\n";

  // load informations
  message += "<text class=\"g10\" x=\"10\" y=\"433\" >Load output</text>\r\n";
  message += "<text class=\"g10\" x=\"30\" y=\"460\" >State</text>\r\n";
  message += "<text class=\"value\" x=\"230\" y=\"460\" >" + String(value[VE_LOAD]) + "</text>\r\n";
  message += "<text class=\"g10\" x=\"30\" y=\"495\" >Current</text>\r\n";
  float Loadcurrent = 0;
  if (isNumeric(value[VE_IL])) {
    Loadcurrent = (float) atoi(value[VE_IL]) / 1000;
    message += "<text class=\"value\" x=\"230\" y=\"495\" >" + String(Loadcurrent) + " A</text>\r\n";
  }

  message += "</svg>\r\n\r\n";
  message += "</body>\r\n";
  message += "</html>\r\n";

  return message;
}
