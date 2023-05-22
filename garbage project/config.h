/*
  config.h - config the keywords for Victron.DIRECT

  2019-10-30
  Modified : keywords enum instead of defines.
  Added : OR, Relay, MPPT.
  Added : Keywords comments and units.
*/


#define MPPT_75_15

////////////////////////////////////////////// MPPT 75|15
#ifdef MPPT_75_15
const byte MPPT_BUFFSIZE     =   32; // max size of lines read (LABEL+VALUE size)
const byte MPPT_VALUES_BYTES =   12;
const byte MPPT_LABEL_BYTES  =    9;

enum keywordslist {
  VE_Dummy = 0,
  VE_PID,
  VE_FW,
  VE_SER,
  VE_V,
  VE_I,
  VE_VPV,
  VE_PPV,
  VE_Relay,
  VE_OR,
  VE_CS,
  VE_ERR,
  VE_LOAD,
  VE_IL,
  VE_H19,
  VE_H20,
  VE_H21,
  VE_H22,
  VE_H23,
  VE_HSDS,
  VE_MPPT,
  VE_Checksum,
  VE_LastKeyword
};

char keywords[VE_LastKeyword][MPPT_LABEL_BYTES] = {
  "Dummy",   // a string that won't match any label
  "PID",     // Product ID
  "FW",      // Firmware version (16 bit)
  "SER#",    // Serial number
  "V",       // (mV) Main or channel 1 (battery) voltage
  "I",       // (mA) Main or channel 1 battery current
  "VPV",     // (mV) Panel voltage
  "PPV",     // (W) Panel power
  "Relay",   // Relay state
  "OR",      // Off reason
  "CS",      // State of operation
  "ERR",     // Error code
  "LOAD",    // Load output state (ON/OFF)
  "IL",      // (mA) Load current
  "H19",     // (0.01 kWh) Yield total (user resettable counter)
  "H20",     // (0.01 kWh) Yield today
  "H21",     // (W) Maximum power today
  "H22",     // (0.01 kWh) Yield yesterday
  "H23",     // (W) Maximum power yesterday
  "HSDS",    // Day sequence number (0..364)
  "MPPT",    // Tracker operation mode
  "Checksum"
};
#endif


//////////////////////////////////////////////////////////////////////////////
// System battetry voltage (12, 24, 48 V)
const int BV_Unknown =  0;
const int BV_12V     = 12;
const int BV_24V     = 24;
const int BV_36V     = 36;
const int BV_48V     = 48;

int GetSystemBatteryVoltage(float bv) {
  if ((bv > 10) & (bv < 16)) {
    return BV_12V;
  }
  if ((bv > 20) & (bv < 30)) {
    return BV_24V;
  }
  if ((bv > 30) & (bv < 44)) {
    return BV_36V;
  }
  if ((bv > 44) & (bv < 56)) {
    return BV_48V;
  }
  return BV_Unknown;
}

String GetStringBatteryVoltage(float bv) {
  int SystemBV = GetSystemBatteryVoltage(bv);
  switch (SystemBV) {
    case BV_Unknown : return "0V"; break;
    case BV_12V     : return "12V"; break;
    case BV_24V     : return "24V"; break;
    case BV_36V     : return "36V"; break;
    case BV_48V     : return "48V"; break;
    default         : return "Unknown " + String(SystemBV) + "V"; break;
  }
}

//////////////////////////////////////////////////////////////////////////////
// AR
// Alarm reason; this field describes the cause of the alarm. Since multiple alarm conditions can be
// present at the same time the values of the separate alarm conditions are added. The value total is
// sent in decimal notation.
const unsigned long AR_Low_Voltage          =    1;
const unsigned long AR_High_Voltage         =    2;
const unsigned long AR_Low_SOC              =    4;
const unsigned long AR_Low_Starter_Voltage  =    8;
const unsigned long AR_High_Starter_Voltage =   16;
const unsigned long AR_Low_Temperature      =   32;
const unsigned long AR_High_Temperature     =   64;
const unsigned long AR_Mid_Voltage          =  128;
const unsigned long AR_Overload             =  256;
const unsigned long AR_DC_ripple            =  512;
const unsigned long AR_Low_V_AC_out         = 1024;
const unsigned long AR_High_V_AC_out        = 2048;
const unsigned long AR_Short_Circuit        = 4096;
const unsigned long AR_BMS_Lockout          = 8192;
/*
  E.g. a value of 5 would indicate the presence of a low SOC alarm and a low Voltage.
  Note: This field is available in the BMV since version 2.08
*/

//////////////////////////////////////////////////////////////////////////////
// OR
// Off reason, this field described why a unit is switched off.
const unsigned long OR_No_input_power                    = 0x00000001;
const unsigned long OR_Switched_off_power_switch         = 0x00000002;
const unsigned long OR_Switched_off_device_mode_register = 0x00000004;
const unsigned long OR_Remote_input                      = 0x00000008;
const unsigned long OR_Protection_active                 = 0x00000010;
const unsigned long OR_Paygo                             = 0x00000020;
const unsigned long OR_BMS                               = 0x00000040;
const unsigned long OR_Engine_shutdown_detection         = 0x00000080;
const unsigned long OR_Analysing_input_voltage           = 0x00000100;



//////////////////////////////////////////////////////////////////////////////
// CAP_BLE
// Off reason, this field described why a unit is switched off.
const unsigned long CAP_BLE_supports_switching_off     = 0x00000001;
const unsigned long CAP_BLE_switching_off_is_permanent = 0x00000002;


struct PIDProduct
{
  char pid[7];
  char product[40];
};


PIDProduct PIDS[] =
{
  /*
    { 0x203,  "BMV 700" },
    { 0x204,  "BMV 702" },
    { 0x205,  "BMV 700H" },
    // ...
  */
  { "0x0300", "BlueSolar MPPT 70/15" }, //model is phased out.
  { "0xA040", "BlueSolar MPPT 75/50" }, //model is phased out.
  { "0xA041", "BlueSolar MPPT 150/35" }, //model is phased out.
  { "0xA042", "BlueSolar MPPT 75/15" },
  { "0xA043", "BlueSolar MPPT 100/15" },
  { "0xA044", "BlueSolar MPPT 100/30" }, //model is phased out.
  { "0xA045", "BlueSolar MPPT 100/50" }, //model is phased out.
  { "0xA046", "BlueSolar MPPT 150/70" },
  { "0xA047", "BlueSolar MPPT 150/100" },
  { "0xA049", "BlueSolar MPPT 100/50 rev2" },
  { "0xA04A", "BlueSolar MPPT 100/30 rev2" },
  { "0xA04B", "BlueSolar MPPT 150/35 rev2" },
  { "0xA04C", "BlueSolar MPPT 75/10" },
  { "0xA04D", "BlueSolar MPPT 150/45" },
  { "0xA04E", "BlueSolar MPPT 150/60" },
  { "0xA04F", "BlueSolar MPPT 150/85" },
  // ...
  { "0xA050", "SmartSolar MPPT 250/100" },
  { "0xA051", "SmartSolar MPPT 150/100" }, //model is phased out.
  { "0xA052", "SmartSolar MPPT 150/85" },  //model is phased out.
  { "0xA053", "SmartSolar MPPT 75/15" },
  { "0xA054", "SmartSolar MPPT 75/10" },
  { "0xA055", "SmartSolar MPPT 100/15" },
  { "0xA056", "SmartSolar MPPT 100/30" },
  { "0xA057", "SmartSolar MPPT 100/50" },
  { "0xA058", "SmartSolar MPPT 150/35" },
  { "0xA059", "SmartSolar MPPT 150/100 rev2" },
  { "0xA05A", "SmartSolar MPPT 150/85 rev2" },
  { "0xA05B", "SmartSolar MPPT 250/70" },
  { "0xA05C", "SmartSolar MPPT 250/85" },
  { "0xA05D", "SmartSolar MPPT 250/60" },
  { "0xA05E", "SmartSolar MPPT 250/45" },
  { "0xA05F", "SmartSolar MPPT 100/20" },
  { "0xA060", "SmartSolar MPPT 100/20 48V" },
  { "0xA061", "SmartSolar MPPT 150/45" },
  { "0xA062", "SmartSolar MPPT 150/60" },
  { "0xA063", "SmartSolar MPPT 150/70" },
  { "0xA064", "SmartSolar MPPT 250/85 rev2" },
  { "0xA065", "SmartSolar MPPT 250/100 rev2" },
  // ...
  { "0xA102", "SmartSolar MPPT VE Can 150/70" },
  { "0xA103", "SmartSolar MPPT VE Can 150/45" },
  { "0xA104", "SmartSolar MPPT VE Can 150/60" },
  { "0xA105", "SmartSolar MPPT VE Can 150/85" },
  { "0xA106", "SmartSolar MPPT VE Can 150/100" },
  { "0xA107", "SmartSolar MPPT VE Can 250/45" },
  { "0xA108", "SmartSolar MPPT VE Can 250/60" },
  { "0xA109", "SmartSolar MPPT VE Can 250/70" },
  { "0xA10A", "SmartSolar MPPT VE Can 250/85" },
  { "0xA10B", "SmartSolar MPPT VE Can 250/100" }
  // ...
  /*
    { 0xA201, "Phoenix Inverter 12V 250VA 230V" }, //model is phased out.
    { 0xA202, "Phoenix Inverter 24V 250VA 230V" }, //model is phased out.
    { 0xA204, "Phoenix Inverter 48V 250VA 230V" }, //model is phased out.
    { 0xA211, "Phoenix Inverter 12V 375VA 230V" }, //model is phased out.
    { 0xA212, "Phoenix Inverter 24V 375VA 230V" }, //model is phased out.
    { 0xA214, "Phoenix Inverter 48V 375VA 230V" }, //model is phased out.
    { 0xA221, "Phoenix Inverter 12V 500VA 230V" }, //model is phased out.
    { 0xA222, "Phoenix Inverter 24V 500VA 230V" }, //model is phased out.
    { 0xA224, "Phoenix Inverter 48V 500VA 230V" },  //model is phased out.
    // ...
    { 0xA231, "Phoenix Inverter 12V 250VA 230V" },
    { 0xA232, "Phoenix Inverter 24V 250VA 230V" },
    { 0xA234, "Phoenix Inverter 48V 250VA 230V" },
    { 0xA239, "Phoenix Inverter 12V 250VA 120V" },
    { 0xA23A, "Phoenix Inverter 24V 250VA 120V" },
    { 0xA23C, "Phoenix Inverter 48V 250VA 120V" },
    { 0xA241, "Phoenix Inverter 12V 375VA 230V" },
    { 0xA242, "Phoenix Inverter 24V 375VA 230V" },
    { 0xA244, "Phoenix Inverter 48V 375VA 230V" },
    { 0xA249, "Phoenix Inverter 12V 375VA 120V" },
    { 0xA24A, "Phoenix Inverter 24V 375VA 120V" },
    { 0xA24C, "Phoenix Inverter 48V 375VA 120V" },
    { 0xA251, "Phoenix Inverter 12V 500VA 230V" },
    { 0xA252, "Phoenix Inverter 24V 500VA 230V" },
    { 0xA254, "Phoenix Inverter 48V 500VA 230V" },
    { 0xA259, "Phoenix Inverter 12V 500VA 120V" },
    { 0xA25A, "Phoenix Inverter 24V 500VA 120V" },
    { 0xA25C, "Phoenix Inverter 48V 500VA 120V" },
    { 0xA261, "Phoenix Inverter 12V 800VA 230V" },
    { 0xA262, "Phoenix Inverter 24V 800VA 230V" },
    { 0xA264, "Phoenix Inverter 48V 800VA 230V" },
    { 0xA269, "Phoenix Inverter 12V 800VA 120V" },
    { 0xA26A, "Phoenix Inverter 24V 800VA 120V" },
    { 0xA26C, "Phoenix Inverter 48V 800VA 120V" },
    { 0xA271, "Phoenix Inverter 12V 1200VA 230V" },
    { 0xA272, "Phoenix Inverter 24V 1200VA 230V" },
    { 0xA274, "Phoenix Inverter 48V 1200VA 230V" },
    { 0xA279, "Phoenix Inverter 12V 1200VA 120V" },
    { 0xA27A, "Phoenix Inverter 24V 1200VA 120V" },
    { 0xA27C, "Phoenix Inverter 48V 1200VA 120V" },
    { 0xA281, "Phoenix Inverter 12V 1600VA 230V" },
    { 0xA282, "Phoenix Inverter 24V 1600VA 230V" },
    { 0xA284, "Phoenix Inverter 48V 1600VA 230V" },
    { 0xA291, "Phoenix Inverter 12V 2000VA 230V" },
    { 0xA292, "Phoenix Inverter 24V 2000VA 230V" },
    { 0xA294, "Phoenix Inverter 48V 2000VA 230V" },
    { 0xA2A1, "Phoenix Inverter 12V 3000VA 230V" },
    { 0xA2A2, "Phoenix Inverter 24V 3000VA 230V" },
    { 0xA2A4, "Phoenix Inverter 48V 3000VA 230V" },
    // ...
    { 0xA340, "Phoenix Smart IP43 Charger 12/50 (1+1)" },
    { 0xA341, "Phoenix Smart IP43 Charger 12/50 (3)" },
    { 0xA342, "Phoenix Smart IP43 Charger 24/25 (1+1)" },
    { 0xA343, "Phoenix Smart IP43 Charger 24/25 (3)" },
    { 0xA344, "Phoenix Smart IP43 Charger 12/30 (1+1)" },
    { 0xA345, "Phoenix Smart IP43 Charger 12/30 (3)" },
    { 0xA346, "Phoenix Smart IP43 Charger 24/16 (1+1)" },
    { 0xA347, "Phoenix Smart IP43 Charger 24/16 (3)" }
  */
};


/* for (int cnt = 0; cnt < sizeof(pins) / sizeof(pins[0]); cnt++) { Serial.println(pins[cnt].name); } */
String GetStringProduct(char pid[]) {
  for (int cnt = 0; cnt < (sizeof(PIDS) / sizeof(PIDS[0])); cnt++) {
    if (strcmp(PIDS[cnt].pid, pid) == 0) {
      return String(PIDS[cnt].product);
    }
  }
  return String(pid);
}



//////////////////////////////////////////////////////////////////////////////
/*
  const unsigned int PID_BMV_700 = 0x203;
  const unsigned int PID_BMV_702 = 0x204;
  const unsigned int PID_BMV_700H = 0x205;
*/
const unsigned int PID_BlueSolar_MPPT_70_15 = 0x0300;   //model is phased out.
const unsigned int PID_BlueSolar_MPPT_75_50 = 0xA040;   //model is phased out.
const unsigned int PID_BlueSolar_MPPT_150_35 = 0xA041;  //model is phased out.
const unsigned int PID_BlueSolar_MPPT_75_15 = 0xA042;
const unsigned int PID_BlueSolar_MPPT_100_15 = 0xA043;
const unsigned int PID_BlueSolar_MPPT_100_30 = 0xA044; //model is phased out.
const unsigned int PID_BlueSolar_MPPT_100_50 = 0xA045; //model is phased out.
const unsigned int PID_BlueSolar_MPPT_150_70 = 0xA046;
const unsigned int PID_BlueSolar_MPPT_150_100 = 0xA047;
const unsigned int PID_BlueSolar_MPPT_100_50_rev2 = 0xA049;
const unsigned int PID_BlueSolar_MPPT_100_30_rev2 = 0xA04A;
const unsigned int PID_BlueSolar_MPPT_150_35_rev2 = 0xA04B;
const unsigned int PID_BlueSolar_MPPT_75_10 = 0xA04C;
const unsigned int PID_BlueSolar_MPPT_150_45 = 0xA04D;
const unsigned int PID_BlueSolar_MPPT_150_60 = 0xA04E;
const unsigned int PID_BlueSolar_MPPT_150_85 = 0xA04F;
const unsigned int PID_SmartSolar_MPPT_250_100 = 0xA050;
const unsigned int PID_SmartSolar_MPPT_150_100 = 0xA051; //model is phased out.
const unsigned int PID_SmartSolar_MPPT_150_85 = 0xA052; //model is phased out.
const unsigned int PID_SmartSolar_MPPT_75_15 = 0xA053;
const unsigned int PID_SmartSolar_MPPT_75_10 = 0xA054;
const unsigned int PID_SmartSolar_MPPT_100_15 = 0xA055;
const unsigned int PID_SmartSolar_MPPT_100_30 = 0xA056;
const unsigned int PID_SmartSolar_MPPT_100_50 = 0xA057;
const unsigned int PID_SmartSolar_MPPT_150_35 = 0xA058;
const unsigned int PID_SmartSolar_MPPT_150_100_rev2 = 0xA059;
const unsigned int PID_SmartSolar_MPPT_150_85_rev2 = 0xA05A;
const unsigned int PID_SmartSolar_MPPT_250_70 = 0xA05B;
const unsigned int PID_SmartSolar_MPPT_250_85 = 0xA05C;
const unsigned int PID_SmartSolar_MPPT_250_60 = 0xA05D;
const unsigned int PID_SmartSolar_MPPT_250_45 = 0xA05E;
const unsigned int PID_SmartSolar_MPPT_100_20 = 0xA05F;
const unsigned int PID_SmartSolar_MPPT_100_20_48V = 0xA060;
const unsigned int PID_SmartSolar_MPPT_150_45 = 0xA061;
const unsigned int PID_SmartSolar_MPPT_150_60 = 0xA062;
const unsigned int PID_SmartSolar_MPPT_150_70 = 0xA063;
const unsigned int PID_SmartSolar_MPPT_250_85_rev2 = 0xA064;
const unsigned int PID_SmartSolar_MPPT_250_100_rev2 = 0xA065;
const unsigned int PID_SmartSolar_MPPT_VE_Can_150_70 = 0xA102;
const unsigned int PID_SmartSolar_MPPT_VE_Can_150_45 = 0xA103;
const unsigned int PID_SmartSolar_MPPT_VE_Can_150_60 = 0xA104;
const unsigned int PID_SmartSolar_MPPT_VE_Can_150_85 = 0xA105;
const unsigned int PID_SmartSolar_MPPT_VE_Can_150_100 = 0xA106;
const unsigned int PID_SmartSolar_MPPT_VE_Can_250_45 = 0xA107;
const unsigned int PID_SmartSolar_MPPT_VE_Can_250_60 = 0xA108;
const unsigned int PID_SmartSolar_MPPT_VE_Can_250_70 = 0xA109;
const unsigned int PID_SmartSolar_MPPT_VE_Can_250_85 = 0xA10A;
const unsigned int PID_SmartSolar_MPPT_VE_Can_250_100 = 0xA10B;
/*
  const unsigned int PID_Phoenix_Inverter_12V_250VA_230V_Old = 0xA201; //model is phased out.
  const unsigned int PID_Phoenix_Inverter_24V_250VA_230V_Old = 0xA202; //model is phased out.
  const unsigned int PID_Phoenix_Inverter_48V_250VA_230V_Old = 0xA204; //model is phased out.
  const unsigned int PID_Phoenix_Inverter_12V_375VA_230V_Old = 0xA211; //model is phased out.
  const unsigned int PID_Phoenix_Inverter_24V_375VA_230V_Old = 0xA212; //model is phased out.
  const unsigned int PID_Phoenix_Inverter_48V_375VA_230V_Old = 0xA214; //model is phased out.
  const unsigned int PID_Phoenix_Inverter_12V_500VA_230V_Old = 0xA221; //model is phased out.
  const unsigned int PID_Phoenix_Inverter_24V_500VA_230V_Old = 0xA222; //model is phased out.
  const unsigned int PID_Phoenix_Inverter_48V_500VA_230V_Old = 0xA224; //model is phased out.
  const unsigned int PID_Phoenix_Inverter_12V_250VA_230V = 0xA231;
  const unsigned int PID_Phoenix_Inverter_24V_250VA_230V = 0xA232;
  const unsigned int PID_Phoenix_Inverter_48V_250VA_230V = 0xA234;
  const unsigned int PID_Phoenix_Inverter_12V_250VA_120V = 0xA239;
  const unsigned int PID_Phoenix_Inverter_24V_250VA_120V = 0xA23A;
  const unsigned int PID_Phoenix_Inverter_48V_250VA_120V = 0xA23C;
  const unsigned int PID_Phoenix_Inverter_12V_375VA_230V = 0xA241;
  const unsigned int PID_Phoenix_Inverter_24V_375VA_230V = 0xA242;
  const unsigned int PID_Phoenix_Inverter_48V_375VA_230V = 0xA244;
  const unsigned int PID_Phoenix_Inverter_12V_375VA_120V = 0xA249;
  const unsigned int PID_Phoenix_Inverter_24V_375VA_120V = 0xA24A;
  const unsigned int PID_Phoenix_Inverter_48V_375VA_120V = 0xA24C;
  const unsigned int PID_Phoenix_Inverter_12V_500VA_230V = 0xA251;
  const unsigned int PID_Phoenix_Inverter_24V_500VA_230V = 0xA252;
  const unsigned int PID_Phoenix_Inverter_48V_500VA_230V = 0xA254;
  const unsigned int PID_Phoenix_Inverter_12V_500VA_120V = 0xA259;
  const unsigned int PID_Phoenix_Inverter_24V_500VA_120V = 0xA25A;
  const unsigned int PID_Phoenix_Inverter_48V_500VA_120V = 0xA25C;
  const unsigned int PID_Phoenix_Inverter_12V_800VA_230V = 0xA261;
  const unsigned int PID_Phoenix_Inverter_24V_800VA_230V = 0xA262;
  const unsigned int PID_Phoenix_Inverter_48V_800VA_230V = 0xA264;
  const unsigned int PID_Phoenix_Inverter_12V_800VA_120V = 0xA269;
  const unsigned int PID_Phoenix_Inverter_24V_800VA_120V = 0xA26A;
  const unsigned int PID_Phoenix_Inverter_48V_800VA_120V = 0xA26C;
  const unsigned int PID_Phoenix_Inverter_12V_1200VA_230V = 0xA271;
  const unsigned int PID_Phoenix_Inverter_24V_1200VA_230V = 0xA272;
  const unsigned int PID_Phoenix_Inverter_48V_1200VA_230V = 0xA274;
  const unsigned int PID_Phoenix_Inverter_12V_1200VA_120V = 0xA279;
  const unsigned int PID_Phoenix_Inverter_24V_1200VA_120V = 0xA27A;
  const unsigned int PID_Phoenix_Inverter_48V_1200VA_120V = 0xA27C;
  const unsigned int PID_Phoenix_Inverter_12V_1600VA_230V = 0xA281;
  const unsigned int PID_Phoenix_Inverter_24V_1600VA_230V = 0xA282;
  const unsigned int PID_Phoenix_Inverter_48V_1600VA_230V = 0xA284;
  const unsigned int PID_Phoenix_Inverter_12V_2000VA_230V = 0xA291;
  const unsigned int PID_Phoenix_Inverter_24V_2000VA_230V = 0xA292;
  const unsigned int PID_Phoenix_Inverter_48V_2000VA_230V = 0xA294;
  const unsigned int PID_Phoenix_Inverter_12V_3000VA_230V = 0xA2A1;
  const unsigned int PID_Phoenix_Inverter_24V_3000VA_230V = 0xA2A2;
  const unsigned int PID_Phoenix_Inverter_48V_3000VA_230V = 0xA2A4;
  const unsigned int PID_Phoenix_Smart_IP43_Charger_12_50_1p1 = 0xA340;
  const unsigned int PID_Phoenix_Smart_IP43_Charger_12_50_3 = 0xA341;
  const unsigned int PID_Phoenix_Smart_IP43_Charger_24_25_1p1 = 0xA342;
  const unsigned int PID_Phoenix_Smart_IP43_Charger_24_25_3 = 0xA343;
  const unsigned int PID_Phoenix_Smart_IP43_Charger_12_30_1p1 = 0xA344;
  const unsigned int PID_Phoenix_Smart_IP43_Charger_12_30_3 = 0xA345;
  const unsigned int PID_Phoenix_Smart_IP43_Charger_24_16_1p1 = 0xA346;
  const unsigned int PID_Phoenix_Smart_IP43_Charger_24_16_3 = 0xA347;
*/




//////////////////////////////////////////////////////////////////////////////
// CS
// The state of operation. See the table below for the possible values.
/////////////////////////////////////////////////  MPPT  INVERTER  CHARGER
const byte CS_Off                       =   0; //   o       o         o
const byte CS_Low_power                 =   1; //           o
const byte CS_Fault                     =   2; //   o       o         o
const byte CS_Bulk                      =   3; //   o                 o
const byte CS_Absorption                =   4; //   o                 o
const byte CS_Float                     =   5; //   o                 o
const byte CS_Storage                   =   6; //                     o
const byte CS_Equalize_manual           =   7; //   o
const byte CS_Inverting                 =   9; //           o
const byte CS_Power_supply              =  11; //                     o
const byte CS_Starting_up               = 245; //   o
const byte CS_Repeated_absorption       = 246; //                     o
const byte CS_Auto_equalize_Recondition = 247; //   o                 o
const byte CS_BatterySafe               = 248; //                     o
const byte CS_External_Control          = 252; //   o

String GetStringState(int cs) {
  switch (cs) {
    case CS_Off                       : return "Off"; break;
    case CS_Low_power                 : return "Low power"; break;
    case CS_Fault                     : return "Fault"; break;
    case CS_Bulk                      : return "Bulk"; break;
    case CS_Absorption                : return "Absorption"; break;
    case CS_Float                     : return "Float"; break;
    case CS_Storage                   : return "Storage"; break;
    case CS_Equalize_manual           : return "Equalize manual"; break;
    case CS_Inverting                 : return "Inverting"; break;
    case CS_Power_supply              : return "Power supply"; break;
    case CS_Starting_up               : return "Starting up"; break;
    case CS_Repeated_absorption       : return "Repeated absorption"; break;
    case CS_Auto_equalize_Recondition : return "Auto equalize Recondition"; break;
    case CS_BatterySafe               : return "BatterySafe"; break;
    case CS_External_Control          : return "External Control"; break;
    default                           : return "Unknown state (" + String(cs) + ")"; break;
  }
}


//////////////////////////////////////////////////////////////////////////////
// ERR
// The error code of the device (relevant when the device is in the fault state). See the table below for the possible values.
const byte ERR_No_error                       =   0;
const byte ERR_Battery_voltage_too_high       =   2;
const byte ERR_Charger_temperature_too_high   =  17;
const byte ERR_Charger_over_current           =  18;
const byte ERR_Charger_current_reversed       =  19;
const byte ERR_Bulk_time_limit_exceeded       =  20;
const byte ERR_Current_sensor_issue           =  21; // sensor bias/sensor broken
const byte ERR_Terminals_overheated           =  26;
const byte ERR_Converter_issue                =  28;
const byte ERR_Input_voltage_too_high         =  33; // solar panel
const byte ERR_Input_current_too_high         =  34; // solar panel
const byte ERR_Input_shutdown_battery_voltage =  38; // due to excessive battery voltage
const byte ERR_Input_shutdown_current_flow    =  39; // due to current flow during off mode
const byte ERR_Lost_communication_with_one_of_devices = 65;
const byte ERR_Synchronised_charging_device_configuration_issue = 66;
const byte ERR_BMS_connection_lost            =  67;
const byte ERR_Network_misconfigured          =  68;
const byte ERR_Factory_calibration_data_lost  = 116;
const byte ERR_Invalid_incompatible_firmware  = 117;
const byte ERR_User_settings_invalid          = 119;
/*
  Note1: Error 19 can be ignored, this condition regularly occurs during start-up or shutdown of the
  MPPT charger. Since version 1.15 this error will no longer be reported.
  Note2: Error 21 can be ignored for 5 minutes, this condition regularly occurs during start-up or
  shutdown of the MPPT charger. Since version 1.16 this warning will no longer be reported when it is
  not persistent.
*/

String GetStringError(int err) {
  switch (err) {
    ERR_No_error                       : return "No error"; break;
    ERR_Battery_voltage_too_high       : return "Battery voltage too high"; break;
    ERR_Charger_temperature_too_high   : return "Charger temperature too high"; break;
    ERR_Charger_over_current           : return "Charger over current"; break;
    ERR_Charger_current_reversed       : return "Charger current reversed"; break;
    ERR_Bulk_time_limit_exceeded       : return "Bulk time limit exceeded"; break;
    ERR_Current_sensor_issue           : return "Current sensor issue (sensor bias/sensor broken)"; break;
    ERR_Terminals_overheated           : return "Terminals overheated"; break;
    ERR_Converter_issue                : return "Converter issue"; break;
    ERR_Input_voltage_too_high         : return "Input voltage too high (solar panel)"; break;
    ERR_Input_current_too_high         : return "Input current too high (solar panel)"; break;
    ERR_Input_shutdown_battery_voltage : return "Input shutdown battery voltage due to excessive battery voltage"; break;
    ERR_Input_shutdown_current_flow    : return "Input shutdown current flow due to current flow during off mode"; break;
    ERR_Lost_communication_with_one_of_devices : return "Lost communication with one of devices"; break;
    ERR_Synchronised_charging_device_configuration_issue : return "Synchronised charging device configuration issue"; break;
    ERR_BMS_connection_lost            : return "BMS connection lost"; break;
    ERR_Network_misconfigured          : return "Network misconfigured"; break;
    ERR_Factory_calibration_data_lost  : return "Factory calibration data lost"; break;
    ERR_Invalid_incompatible_firmware  : return "Invalid incompatible firmware"; break;
    ERR_User_settings_invalid          : return "User settings invalid"; break;
    default                            : return "Unknown error (" + String(err) + ")"; break;
  }
}




//////////////////////////////////////////////////////////////////////////////
// MODE
//The possible values for the device mode are listed in this table.
/////////////////////////////////////////////// Inverter Charger
const byte MODE_VE_REG_MODE_CHARGER   =   1; //             o
const byte MODE_VE_REG_MODE_INVERTER  =   2; //     o
const byte MODE_VE_REG_MODE_OFF       =   4; //     o       o
const byte MODE_VE_REG_MODE_ECO       =   5; //     o
const byte MODE_VE_REG_MODE_HIBERNATE = 253; // SmartOnly


//////////////////////////////////////////////////////////////////////////////
// MPPT
//The possible values for the tracker operation are listed in this table.
const int MPPT_Off = 0;
const int MPPT_Voltage_or_current_limited = 1;
const int MPPT_MPP_Tracker_active = 2;

String GetStringTracker(int mpp) {
  switch (mpp) {
    case MPPT_Off                        : return "Tracker is off"; break;
    case MPPT_Voltage_or_current_limited : return "Voltage or current limited"; break;
    case MPPT_MPP_Tracker_active         : return "Tracker is active"; break;
    default                         : return "Unknown tracker operation (" + String(mpp) + ")"; break;
  }
}
