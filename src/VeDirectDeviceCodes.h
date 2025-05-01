 #pragma once

#include <Arduino.h>

struct VeCodeEntry {
  const char* code;
  const char* text;
};

// --- Mappingtabelle: VeDirectDeviceCodeAR ---
static const char VeDirectDeviceCodeAR_code_0[] PROGMEM = "1";
static const char VeDirectDeviceCodeAR_text_0[] PROGMEM = "Low Voltage";
static const char VeDirectDeviceCodeAR_code_1[] PROGMEM = "2";
static const char VeDirectDeviceCodeAR_text_1[] PROGMEM = "High Voltage";
static const char VeDirectDeviceCodeAR_code_2[] PROGMEM = "4";
static const char VeDirectDeviceCodeAR_text_2[] PROGMEM = "Low SOC";
static const char VeDirectDeviceCodeAR_code_3[] PROGMEM = "8";
static const char VeDirectDeviceCodeAR_text_3[] PROGMEM = "Low Starter Voltage";
static const char VeDirectDeviceCodeAR_code_4[] PROGMEM = "16";
static const char VeDirectDeviceCodeAR_text_4[] PROGMEM = "High Starter Voltage";
static const char VeDirectDeviceCodeAR_code_5[] PROGMEM = "32";
static const char VeDirectDeviceCodeAR_text_5[] PROGMEM = "Low Temperature";
static const char VeDirectDeviceCodeAR_code_6[] PROGMEM = "64";
static const char VeDirectDeviceCodeAR_text_6[] PROGMEM = "High Temperature";
static const char VeDirectDeviceCodeAR_code_7[] PROGMEM = "128";
static const char VeDirectDeviceCodeAR_text_7[] PROGMEM = "Mid Voltage";
static const char VeDirectDeviceCodeAR_code_8[] PROGMEM = "256";
static const char VeDirectDeviceCodeAR_text_8[] PROGMEM = "Overload";
static const char VeDirectDeviceCodeAR_code_9[] PROGMEM = "512";
static const char VeDirectDeviceCodeAR_text_9[] PROGMEM = "DC-ripple";
static const char VeDirectDeviceCodeAR_code_10[] PROGMEM = "1024";
static const char VeDirectDeviceCodeAR_text_10[] PROGMEM = "Low V AC out";
static const char VeDirectDeviceCodeAR_code_11[] PROGMEM = "2048";
static const char VeDirectDeviceCodeAR_text_11[] PROGMEM = "LHigh V AC out";
static const char VeDirectDeviceCodeAR_code_12[] PROGMEM = "4096";
static const char VeDirectDeviceCodeAR_text_12[] PROGMEM = "Short Circuit";
static const char VeDirectDeviceCodeAR_code_13[] PROGMEM = "8192";
static const char VeDirectDeviceCodeAR_text_13[] PROGMEM = "BMS Lockout";

static const VeCodeEntry VeDirectDeviceCodeAR[] PROGMEM = {
  {VeDirectDeviceCodeAR_code_0, VeDirectDeviceCodeAR_text_0},
  {VeDirectDeviceCodeAR_code_1, VeDirectDeviceCodeAR_text_1},
  {VeDirectDeviceCodeAR_code_2, VeDirectDeviceCodeAR_text_2},
  {VeDirectDeviceCodeAR_code_3, VeDirectDeviceCodeAR_text_3},
  {VeDirectDeviceCodeAR_code_4, VeDirectDeviceCodeAR_text_4},
  {VeDirectDeviceCodeAR_code_5, VeDirectDeviceCodeAR_text_5},
  {VeDirectDeviceCodeAR_code_6, VeDirectDeviceCodeAR_text_6},
  {VeDirectDeviceCodeAR_code_7, VeDirectDeviceCodeAR_text_7},
  {VeDirectDeviceCodeAR_code_8, VeDirectDeviceCodeAR_text_8},
  {VeDirectDeviceCodeAR_code_9, VeDirectDeviceCodeAR_text_9},
  {VeDirectDeviceCodeAR_code_10, VeDirectDeviceCodeAR_text_10},
  {VeDirectDeviceCodeAR_code_11, VeDirectDeviceCodeAR_text_11},
  {VeDirectDeviceCodeAR_code_12, VeDirectDeviceCodeAR_text_12},
  {VeDirectDeviceCodeAR_code_13, VeDirectDeviceCodeAR_text_13},
};
static const size_t VeDirectDeviceCodeARSize = sizeof(VeDirectDeviceCodeAR) / sizeof(VeDirectDeviceCodeAR[0]);

// --- Mappingtabelle: VeDirectDeviceCodeOR ---
static const char VeDirectDeviceCodeOR_code_0[] PROGMEM = "0X00000000";
static const char VeDirectDeviceCodeOR_text_0[] PROGMEM = "None";
static const char VeDirectDeviceCodeOR_code_1[] PROGMEM = "0X00000001";
static const char VeDirectDeviceCodeOR_text_1[] PROGMEM = "No input power";
static const char VeDirectDeviceCodeOR_code_2[] PROGMEM = "0X00000002";
static const char VeDirectDeviceCodeOR_text_2[] PROGMEM = "Switched off (power switch)";
static const char VeDirectDeviceCodeOR_code_3[] PROGMEM = "0X00000004";
static const char VeDirectDeviceCodeOR_text_3[] PROGMEM = "Switched off (device mode register)";
static const char VeDirectDeviceCodeOR_code_4[] PROGMEM = "0X00000008";
static const char VeDirectDeviceCodeOR_text_4[] PROGMEM = "Remote input";
static const char VeDirectDeviceCodeOR_code_5[] PROGMEM = "0X00000010";
static const char VeDirectDeviceCodeOR_text_5[] PROGMEM = "Protection active";
static const char VeDirectDeviceCodeOR_code_6[] PROGMEM = "0X00000020";
static const char VeDirectDeviceCodeOR_text_6[] PROGMEM = "Paygo";
static const char VeDirectDeviceCodeOR_code_7[] PROGMEM = "0X00000040";
static const char VeDirectDeviceCodeOR_text_7[] PROGMEM = "BMS";
static const char VeDirectDeviceCodeOR_code_8[] PROGMEM = "0X00000080";
static const char VeDirectDeviceCodeOR_text_8[] PROGMEM = "Engine shutdown detection";
static const char VeDirectDeviceCodeOR_code_9[] PROGMEM = "0X00000100";
static const char VeDirectDeviceCodeOR_text_9[] PROGMEM = "Analysing input voltage";

static const VeCodeEntry VeDirectDeviceCodeOR[] PROGMEM = {
  {VeDirectDeviceCodeOR_code_0, VeDirectDeviceCodeOR_text_0},
  {VeDirectDeviceCodeOR_code_1, VeDirectDeviceCodeOR_text_1},
  {VeDirectDeviceCodeOR_code_2, VeDirectDeviceCodeOR_text_2},
  {VeDirectDeviceCodeOR_code_3, VeDirectDeviceCodeOR_text_3},
  {VeDirectDeviceCodeOR_code_4, VeDirectDeviceCodeOR_text_4},
  {VeDirectDeviceCodeOR_code_5, VeDirectDeviceCodeOR_text_5},
  {VeDirectDeviceCodeOR_code_6, VeDirectDeviceCodeOR_text_6},
  {VeDirectDeviceCodeOR_code_7, VeDirectDeviceCodeOR_text_7},
  {VeDirectDeviceCodeOR_code_8, VeDirectDeviceCodeOR_text_8},
  {VeDirectDeviceCodeOR_code_9, VeDirectDeviceCodeOR_text_9},
};
static const size_t VeDirectDeviceCodeORSize = sizeof(VeDirectDeviceCodeOR) / sizeof(VeDirectDeviceCodeOR[0]);

// --- Mappingtabelle: VeDirectDeviceCodeCS ---
static const char VeDirectDeviceCodeCS_code_0[] PROGMEM = "0";
static const char VeDirectDeviceCodeCS_text_0[] PROGMEM = "Off";
static const char VeDirectDeviceCodeCS_code_1[] PROGMEM = "1";
static const char VeDirectDeviceCodeCS_text_1[] PROGMEM = "Low power";
static const char VeDirectDeviceCodeCS_code_2[] PROGMEM = "2";
static const char VeDirectDeviceCodeCS_text_2[] PROGMEM = "Fault";
static const char VeDirectDeviceCodeCS_code_3[] PROGMEM = "3";
static const char VeDirectDeviceCodeCS_text_3[] PROGMEM = "Bulk";
static const char VeDirectDeviceCodeCS_code_4[] PROGMEM = "4";
static const char VeDirectDeviceCodeCS_text_4[] PROGMEM = "Absorption";
static const char VeDirectDeviceCodeCS_code_5[] PROGMEM = "5";
static const char VeDirectDeviceCodeCS_text_5[] PROGMEM = "Float";
static const char VeDirectDeviceCodeCS_code_6[] PROGMEM = "6";
static const char VeDirectDeviceCodeCS_text_6[] PROGMEM = "Storage";
static const char VeDirectDeviceCodeCS_code_7[] PROGMEM = "7";
static const char VeDirectDeviceCodeCS_text_7[] PROGMEM = "Equalize (manual)";
static const char VeDirectDeviceCodeCS_code_8[] PROGMEM = "9";
static const char VeDirectDeviceCodeCS_text_8[] PROGMEM = "Inverting";
static const char VeDirectDeviceCodeCS_code_9[] PROGMEM = "11";
static const char VeDirectDeviceCodeCS_text_9[] PROGMEM = "Power supply";
static const char VeDirectDeviceCodeCS_code_10[] PROGMEM = "245";
static const char VeDirectDeviceCodeCS_text_10[] PROGMEM = "Starting-up";
static const char VeDirectDeviceCodeCS_code_11[] PROGMEM = "246";
static const char VeDirectDeviceCodeCS_text_11[] PROGMEM = "Repeated absorption";
static const char VeDirectDeviceCodeCS_code_12[] PROGMEM = "247";
static const char VeDirectDeviceCodeCS_text_12[] PROGMEM = "Auto equalize / Recondition";
static const char VeDirectDeviceCodeCS_code_13[] PROGMEM = "248";
static const char VeDirectDeviceCodeCS_text_13[] PROGMEM = "BatterySafe";
static const char VeDirectDeviceCodeCS_code_14[] PROGMEM = "252";
static const char VeDirectDeviceCodeCS_text_14[] PROGMEM = "External Control";

static const VeCodeEntry VeDirectDeviceCodeCS[] PROGMEM = {
  {VeDirectDeviceCodeCS_code_0, VeDirectDeviceCodeCS_text_0},
  {VeDirectDeviceCodeCS_code_1, VeDirectDeviceCodeCS_text_1},
  {VeDirectDeviceCodeCS_code_2, VeDirectDeviceCodeCS_text_2},
  {VeDirectDeviceCodeCS_code_3, VeDirectDeviceCodeCS_text_3},
  {VeDirectDeviceCodeCS_code_4, VeDirectDeviceCodeCS_text_4},
  {VeDirectDeviceCodeCS_code_5, VeDirectDeviceCodeCS_text_5},
  {VeDirectDeviceCodeCS_code_6, VeDirectDeviceCodeCS_text_6},
  {VeDirectDeviceCodeCS_code_7, VeDirectDeviceCodeCS_text_7},
  {VeDirectDeviceCodeCS_code_8, VeDirectDeviceCodeCS_text_8},
  {VeDirectDeviceCodeCS_code_9, VeDirectDeviceCodeCS_text_9},
  {VeDirectDeviceCodeCS_code_10, VeDirectDeviceCodeCS_text_10},
  {VeDirectDeviceCodeCS_code_11, VeDirectDeviceCodeCS_text_11},
  {VeDirectDeviceCodeCS_code_12, VeDirectDeviceCodeCS_text_12},
  {VeDirectDeviceCodeCS_code_13, VeDirectDeviceCodeCS_text_13},
  {VeDirectDeviceCodeCS_code_14, VeDirectDeviceCodeCS_text_14},
};
static const size_t VeDirectDeviceCodeCSSize = sizeof(VeDirectDeviceCodeCS) / sizeof(VeDirectDeviceCodeCS[0]);

// --- Mappingtabelle: VeDirectDeviceCodeERR ---
static const char VeDirectDeviceCodeERR_code_0[] PROGMEM = "0";
static const char VeDirectDeviceCodeERR_text_0[] PROGMEM = "No error";
static const char VeDirectDeviceCodeERR_code_1[] PROGMEM = "2";
static const char VeDirectDeviceCodeERR_text_1[] PROGMEM = "Battery voltage too high";
static const char VeDirectDeviceCodeERR_code_2[] PROGMEM = "17";
static const char VeDirectDeviceCodeERR_text_2[] PROGMEM = "Charger temperature too high";
static const char VeDirectDeviceCodeERR_code_3[] PROGMEM = "18";
static const char VeDirectDeviceCodeERR_text_3[] PROGMEM = "Charger over current";
static const char VeDirectDeviceCodeERR_code_4[] PROGMEM = "19";
static const char VeDirectDeviceCodeERR_text_4[] PROGMEM = "Charger current reversed";
static const char VeDirectDeviceCodeERR_code_5[] PROGMEM = "20";
static const char VeDirectDeviceCodeERR_text_5[] PROGMEM = "Bulk time limit exceeded";
static const char VeDirectDeviceCodeERR_code_6[] PROGMEM = "21";
static const char VeDirectDeviceCodeERR_text_6[] PROGMEM = "Current sensor issue (sensor bias/sensor broken)";
static const char VeDirectDeviceCodeERR_code_7[] PROGMEM = "26";
static const char VeDirectDeviceCodeERR_text_7[] PROGMEM = "Terminals overheated";
static const char VeDirectDeviceCodeERR_code_8[] PROGMEM = "28";
static const char VeDirectDeviceCodeERR_text_8[] PROGMEM = "Converter issue";
static const char VeDirectDeviceCodeERR_code_9[] PROGMEM = "33";
static const char VeDirectDeviceCodeERR_text_9[] PROGMEM = "Input voltage too high (solar panel)";
static const char VeDirectDeviceCodeERR_code_10[] PROGMEM = "34";
static const char VeDirectDeviceCodeERR_text_10[] PROGMEM = "Input current too high (solar panel)";
static const char VeDirectDeviceCodeERR_code_11[] PROGMEM = "38";
static const char VeDirectDeviceCodeERR_text_11[] PROGMEM = "Input shutdown (excessive battery voltage)";
static const char VeDirectDeviceCodeERR_code_12[] PROGMEM = "39";
static const char VeDirectDeviceCodeERR_text_12[] PROGMEM = "Input shutdown (current flow during off mode)";
static const char VeDirectDeviceCodeERR_code_13[] PROGMEM = "65";
static const char VeDirectDeviceCodeERR_text_13[] PROGMEM = "Lost communication with one of devices";
static const char VeDirectDeviceCodeERR_code_14[] PROGMEM = "66";
static const char VeDirectDeviceCodeERR_text_14[] PROGMEM = "Synchronised charging device configuration issue";
static const char VeDirectDeviceCodeERR_code_15[] PROGMEM = "67";
static const char VeDirectDeviceCodeERR_text_15[] PROGMEM = "BMS connection lost";
static const char VeDirectDeviceCodeERR_code_16[] PROGMEM = "68";
static const char VeDirectDeviceCodeERR_text_16[] PROGMEM = "Network misconfigured";
static const char VeDirectDeviceCodeERR_code_17[] PROGMEM = "116";
static const char VeDirectDeviceCodeERR_text_17[] PROGMEM = "Factory calibration data lost";
static const char VeDirectDeviceCodeERR_code_18[] PROGMEM = "117";
static const char VeDirectDeviceCodeERR_text_18[] PROGMEM = "Invalid/incompatible firmware";
static const char VeDirectDeviceCodeERR_code_19[] PROGMEM = "119";
static const char VeDirectDeviceCodeERR_text_19[] PROGMEM = "User settings invalid";

static const VeCodeEntry VeDirectDeviceCodeERR[] PROGMEM = {
  {VeDirectDeviceCodeERR_code_0, VeDirectDeviceCodeERR_text_0},
  {VeDirectDeviceCodeERR_code_1, VeDirectDeviceCodeERR_text_1},
  {VeDirectDeviceCodeERR_code_2, VeDirectDeviceCodeERR_text_2},
  {VeDirectDeviceCodeERR_code_3, VeDirectDeviceCodeERR_text_3},
  {VeDirectDeviceCodeERR_code_4, VeDirectDeviceCodeERR_text_4},
  {VeDirectDeviceCodeERR_code_5, VeDirectDeviceCodeERR_text_5},
  {VeDirectDeviceCodeERR_code_6, VeDirectDeviceCodeERR_text_6},
  {VeDirectDeviceCodeERR_code_7, VeDirectDeviceCodeERR_text_7},
  {VeDirectDeviceCodeERR_code_8, VeDirectDeviceCodeERR_text_8},
  {VeDirectDeviceCodeERR_code_9, VeDirectDeviceCodeERR_text_9},
  {VeDirectDeviceCodeERR_code_10, VeDirectDeviceCodeERR_text_10},
  {VeDirectDeviceCodeERR_code_11, VeDirectDeviceCodeERR_text_11},
  {VeDirectDeviceCodeERR_code_12, VeDirectDeviceCodeERR_text_12},
  {VeDirectDeviceCodeERR_code_13, VeDirectDeviceCodeERR_text_13},
  {VeDirectDeviceCodeERR_code_14, VeDirectDeviceCodeERR_text_14},
  {VeDirectDeviceCodeERR_code_15, VeDirectDeviceCodeERR_text_15},
  {VeDirectDeviceCodeERR_code_16, VeDirectDeviceCodeERR_text_16},
  {VeDirectDeviceCodeERR_code_17, VeDirectDeviceCodeERR_text_17},
  {VeDirectDeviceCodeERR_code_18, VeDirectDeviceCodeERR_text_18},
  {VeDirectDeviceCodeERR_code_19, VeDirectDeviceCodeERR_text_19},
};
static const size_t VeDirectDeviceCodeERRSize = sizeof(VeDirectDeviceCodeERR) / sizeof(VeDirectDeviceCodeERR[0]);

// --- Mappingtabelle: VeDirectDeviceCodeMPPT ---
static const char VeDirectDeviceCodeMPPT_code_0[] PROGMEM = "0";
static const char VeDirectDeviceCodeMPPT_text_0[] PROGMEM = "Off";
static const char VeDirectDeviceCodeMPPT_code_1[] PROGMEM = "1";
static const char VeDirectDeviceCodeMPPT_text_1[] PROGMEM = "Voltage or current limited";
static const char VeDirectDeviceCodeMPPT_code_2[] PROGMEM = "2";
static const char VeDirectDeviceCodeMPPT_text_2[] PROGMEM = "MPP Tracker active";

static const VeCodeEntry VeDirectDeviceCodeMPPT[] PROGMEM = {
  {VeDirectDeviceCodeMPPT_code_0, VeDirectDeviceCodeMPPT_text_0},
  {VeDirectDeviceCodeMPPT_code_1, VeDirectDeviceCodeMPPT_text_1},
  {VeDirectDeviceCodeMPPT_code_2, VeDirectDeviceCodeMPPT_text_2},
};
static const size_t VeDirectDeviceCodeMPPTSize = sizeof(VeDirectDeviceCodeMPPT) / sizeof(VeDirectDeviceCodeMPPT[0]);
 





/* 


// AR (Alarm_code)
static const char * VeDirectDeviceCodeAR[][2] PROGMEM{
    {"1", "Low Voltage"},
    {"2", "High Voltage"},
    {"4", "Low SOC"},
    {"8", "Low Starter Voltage"},
    {"16", "High Starter Voltage"},
    {"32", "Low Temperature"},
    {"64", "High Temperature"},
    {"128", "Mid Voltage"},
    {"256", "Overload"},
    {"512", "DC-ripple"},
    {"1024", "Low V AC out"},
    {"2048", "LHigh V AC out"},
    {"4096", "Short Circuit"},
    {"8192", "BMS Lockout"}};

// OR (off_reason)
static const char * VeDirectDeviceCodeOR[][2] PROGMEM{
    {"0X00000000", "None"},
    {"0X00000001", "No input power"},
    {"0X00000002", "Switched off (power switch)"},
    {"0X00000004", "Switched off (device mode register)"},
    {"0X00000008", "Remote input"},
    {"0X00000010", "Protection active"},
    {"0X00000020", "Paygo"},
    {"0X00000040", "BMS"},
    {"0X00000080", "Engine shutdown detection"},
    {"0X00000100", "Analysing input voltage"},
};
// CS (operation_state)
static const char * VeDirectDeviceCodeCS[][2] PROGMEM{
    {"0", "Off"},
    {"1", "Low power"},
    {"2", "Fault"},
    {"3", "Bulk"},
    {"4", "Absorption"},
    {"5", "Float"},
    {"6", "Storage"},
    {"7", "Equalize (manual)"},
    {"9", "Inverting"},
    {"11", "Power supply"},
    {"245", "Starting-up"},
    {"246", "Repeated absorption"},
    {"247", "Auto equalize / Recondition"},
    {"248", "BatterySafe"},
    {"252", "External Control"}};

// ERR (Current_error)
static const char * VeDirectDeviceCodeERR[][2] PROGMEM{
    {"0", "No error"},
    {"2", "Battery voltage too high"},
    {"17", "Charger temperature too high"},
    {"18", "Charger over current"},
    {"19", "Charger current reversed"},
    {"20", "Bulk time limit exceeded"},
    {"21", "Current sensor issue (sensor bias/sensor broken)"},
    {"26", "Terminals overheated"},
    {"28", "Converter issue"},
    {"33", "Input voltage too high (solar panel)"},
    {"34", "Input current too high (solar panel)"},
    {"38", "Input shutdown (excessive battery voltage)"},
    {"39", "Input shutdown (current flow during off mode)"},
    {"65", "Lost communication with one of devices"},
    {"66", "Synchronised charging device configuration issue"},
    {"67", "BMS connection lost"},
    {"68", "Network misconfigured"},
    {"116", "Factory calibration data lost"},
    {"117", "Invalid/incompatible firmware"},
    {"119", "User settings invalid"}};

// MPPT (Tracker_operation_mode)
static const char * VeDirectDeviceCodeMPPT[][2] PROGMEM{
    {"0", "Off"},
    {"1", "Voltage or current limited"},
    {"2", "MPP Tracker active"}}; */