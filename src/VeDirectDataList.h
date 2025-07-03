 #pragma once

#include <Arduino.h>

struct VePrettyEntry {
  const char* key;
  const char* name;
  const char* op;
};

// --- PRETTY-DATENSTRINGS ---
const char key_0[] PROGMEM = "V";
const char name_0[] PROGMEM = "Voltage";
const char op_0[] PROGMEM = "1000";
const char key_1[] PROGMEM = "V2";
const char name_1[] PROGMEM = "Voltage_2";
const char op_1[] PROGMEM = "1000";
const char key_2[] PROGMEM = "V3";
const char name_2[] PROGMEM = "Voltage_3";
const char op_2[] PROGMEM = "1000";
const char key_3[] PROGMEM = "VS";
const char name_3[] PROGMEM = "Starter_voltage";
const char op_3[] PROGMEM = "1000";
const char key_4[] PROGMEM = "VM";
const char name_4[] PROGMEM = "Mid_voltage";
const char op_4[] PROGMEM = "1000";
const char key_5[] PROGMEM = "DM";
const char name_5[] PROGMEM = "Mid_deviation";
const char op_5[] PROGMEM = "10";
const char key_6[] PROGMEM = "VPV";
const char name_6[] PROGMEM = "Panel_voltage";
const char op_6[] PROGMEM = "1000";
const char key_7[] PROGMEM = "PPV";
const char name_7[] PROGMEM = "Panel_power";
const char op_7[] PROGMEM = "0";
const char key_8[] PROGMEM = "I";
const char name_8[] PROGMEM = "Battery_current";
const char op_8[] PROGMEM = "1000";
const char key_9[] PROGMEM = "I2";
const char name_9[] PROGMEM = "Battery_current_2";
const char op_9[] PROGMEM = "1000";
const char key_10[] PROGMEM = "I3";
const char name_10[] PROGMEM = "Battery_current_3";
const char op_10[] PROGMEM = "1000";
const char key_11[] PROGMEM = "IL";
const char name_11[] PROGMEM = "Load_current";
const char op_11[] PROGMEM = "1000";
const char key_12[] PROGMEM = "LOAD";
const char name_12[] PROGMEM = "Load_output_state";
const char op_12[] PROGMEM = "";
const char key_13[] PROGMEM = "T";
const char name_13[] PROGMEM = "Battery_temperature";
const char op_13[] PROGMEM = "0";
const char key_14[] PROGMEM = "P";
const char name_14[] PROGMEM = "Instantaneous_power";
const char op_14[] PROGMEM = "0";
const char key_15[] PROGMEM = "CE";
const char name_15[] PROGMEM = "Consumed_Amp_Hours";
const char op_15[] PROGMEM = "1000";
const char key_16[] PROGMEM = "SOC";
const char name_16[] PROGMEM = "SOC";
const char op_16[] PROGMEM = "10";
const char key_17[] PROGMEM = "TTG";
const char name_17[] PROGMEM = "Time_to_go";
const char op_17[] PROGMEM = "0";
const char key_18[] PROGMEM = "ALARM";
const char name_18[] PROGMEM = "Alarm";
const char op_18[] PROGMEM = "";
const char key_19[] PROGMEM = "RELAY";
const char name_19[] PROGMEM = "Relay";
const char op_19[] PROGMEM = "";
const char key_20[] PROGMEM = "AR";
const char name_20[] PROGMEM = "Alarm_code";
const char op_20[] PROGMEM = "0";
const char key_21[] PROGMEM = "OR";
const char name_21[] PROGMEM = "Off_reason";
const char op_21[] PROGMEM = "0";
const char key_22[] PROGMEM = "H1";
const char name_22[] PROGMEM = "Deepest_discharge";
const char op_22[] PROGMEM = "1000";
const char key_23[] PROGMEM = "H2";
const char name_23[] PROGMEM = "Last_discharge";
const char op_23[] PROGMEM = "1000";
const char key_24[] PROGMEM = "H3";
const char name_24[] PROGMEM = "Average_discharge";
const char op_24[] PROGMEM = "1000";
const char key_25[] PROGMEM = "H4";
const char name_25[] PROGMEM = "Charge_cycles";
const char op_25[] PROGMEM = "0";
const char key_26[] PROGMEM = "H5";
const char name_26[] PROGMEM = "Full_discharges";
const char op_26[] PROGMEM = "0";
const char key_27[] PROGMEM = "H6";
const char name_27[] PROGMEM = "Cumulative_Ah_drawn";
const char op_27[] PROGMEM = "1000";
const char key_28[] PROGMEM = "H7";
const char name_28[] PROGMEM = "Minimum_voltage";
const char op_28[] PROGMEM = "1000";
const char key_29[] PROGMEM = "H8";
const char name_29[] PROGMEM = "Maximum_voltage";
const char op_29[] PROGMEM = "1000";
const char key_30[] PROGMEM = "H9";
const char name_30[] PROGMEM = "Last_full_charge";
const char op_30[] PROGMEM = "3600";
const char key_31[] PROGMEM = "H10";
const char name_31[] PROGMEM = "Num_automatic_sync";
const char op_31[] PROGMEM = "0";
const char key_32[] PROGMEM = "H11";
const char name_32[] PROGMEM = "Num_low_volt_alarms";
const char op_32[] PROGMEM = "0";
const char key_33[] PROGMEM = "H12";
const char name_33[] PROGMEM = "Num_high_volt_alarms";
const char op_33[] PROGMEM = "0";
const char key_34[] PROGMEM = "H13";
const char name_34[] PROGMEM = "Num_low_aux_vol_alarms";
const char op_34[] PROGMEM = "0";
const char key_35[] PROGMEM = "H14";
const char name_35[] PROGMEM = "Num_high_aux_vol_alarms";
const char op_35[] PROGMEM = "0";
const char key_36[] PROGMEM = "H15";
const char name_36[] PROGMEM = "Min_aux_volt";
const char op_36[] PROGMEM = "1000";
const char key_37[] PROGMEM = "H16";
const char name_37[] PROGMEM = "Max_aux_volt";
const char op_37[] PROGMEM = "1000";
const char key_38[] PROGMEM = "H17";
const char name_38[] PROGMEM = "Amount_discharged_energy";
const char op_38[] PROGMEM = "100";
const char key_39[] PROGMEM = "H18";
const char name_39[] PROGMEM = "Amount_charged_energy";
const char op_39[] PROGMEM = "100";
const char key_40[] PROGMEM = "H19";
const char name_40[] PROGMEM = "total_kWh";
const char op_40[] PROGMEM = "100";
const char key_41[] PROGMEM = "H20";
const char name_41[] PROGMEM = "today_kWh";
const char op_41[] PROGMEM = "100";
const char key_42[] PROGMEM = "H21";
const char name_42[] PROGMEM = "Max_pow_today";
const char op_42[] PROGMEM = "0";
const char key_43[] PROGMEM = "H22";
const char name_43[] PROGMEM = "Yesterday_kWh";
const char op_43[] PROGMEM = "100";
const char key_44[] PROGMEM = "H23";
const char name_44[] PROGMEM = "Max_pow_yesterday";
const char op_44[] PROGMEM = "0";
const char key_45[] PROGMEM = "ERR";
const char name_45[] PROGMEM = "Current_error";
const char op_45[] PROGMEM = "0";
const char key_46[] PROGMEM = "CS";
const char name_46[] PROGMEM = "Operation_state";
const char op_46[] PROGMEM = "0";
const char key_47[] PROGMEM = "BMV";
const char name_47[] PROGMEM = "Model_description";
const char op_47[] PROGMEM = "";
const char key_48[] PROGMEM = "FW";
const char name_48[] PROGMEM = "Firmware_version_16";
const char op_48[] PROGMEM = "";
const char key_49[] PROGMEM = "FWE";
const char name_49[] PROGMEM = "Firmware_version_24";
const char op_49[] PROGMEM = "";
const char key_50[] PROGMEM = "PID";
const char name_50[] PROGMEM = "Device_model";
const char op_50[] PROGMEM = "";
const char key_51[] PROGMEM = "SER#";
const char name_51[] PROGMEM = "Serial_number";
const char op_51[] PROGMEM = "";
const char key_52[] PROGMEM = "HSDS";
const char name_52[] PROGMEM = "Day";
const char op_52[] PROGMEM = "0";
const char key_53[] PROGMEM = "MODE";
const char name_53[] PROGMEM = "Device_mode";
const char op_53[] PROGMEM = "";
const char key_54[] PROGMEM = "AC_OUT_V";
const char name_54[] PROGMEM = "AC_out_volt";
const char op_54[] PROGMEM = "100";
const char key_55[] PROGMEM = "AC_OUT_I";
const char name_55[] PROGMEM = "AC_out_current";
const char op_55[] PROGMEM = "10";
const char key_56[] PROGMEM = "AC_OUT_S";
const char name_56[] PROGMEM = "AC_out_apparent_pow";
const char op_56[] PROGMEM = "";
const char key_57[] PROGMEM = "WARN";
const char name_57[] PROGMEM = "Warning_reason";
const char op_57[] PROGMEM = "";
const char key_58[] PROGMEM = "MPPT";
const char name_58[] PROGMEM = "Tracker_operation_mode";
const char op_58[] PROGMEM = "";
const char key_59[] PROGMEM = "MON";
const char name_59[] PROGMEM = "DC_monitor_mode";
const char op_59[] PROGMEM = "";
const char key_60[] PROGMEM = "DC_IN_V";
const char name_60[] PROGMEM = "DC_input_voltage";
const char op_60[] PROGMEM = "100";
const char key_61[] PROGMEM = "DC_IN_I";
const char name_61[] PROGMEM = "DC_input_current";
const char op_61[] PROGMEM = "10";
const char key_62[] PROGMEM = "DC_IN_P";
const char name_62[] PROGMEM = "DC_input_power";
const char op_62[] PROGMEM = "0";

// --- PRETTY-DATENLISTE ---
const VePrettyEntry VePrettyData[] PROGMEM = {
  {key_0, name_0, op_0},
  {key_1, name_1, op_1},
  {key_2, name_2, op_2},
  {key_3, name_3, op_3},
  {key_4, name_4, op_4},
  {key_5, name_5, op_5},
  {key_6, name_6, op_6},
  {key_7, name_7, op_7},
  {key_8, name_8, op_8},
  {key_9, name_9, op_9},
  {key_10, name_10, op_10},
  {key_11, name_11, op_11},
  {key_12, name_12, op_12},
  {key_13, name_13, op_13},
  {key_14, name_14, op_14},
  {key_15, name_15, op_15},
  {key_16, name_16, op_16},
  {key_17, name_17, op_17},
  {key_18, name_18, op_18},
  {key_19, name_19, op_19},
  {key_20, name_20, op_20},
  {key_21, name_21, op_21},
  {key_22, name_22, op_22},
  {key_23, name_23, op_23},
  {key_24, name_24, op_24},
  {key_25, name_25, op_25},
  {key_26, name_26, op_26},
  {key_27, name_27, op_27},
  {key_28, name_28, op_28},
  {key_29, name_29, op_29},
  {key_30, name_30, op_30},
  {key_31, name_31, op_31},
  {key_32, name_32, op_32},
  {key_33, name_33, op_33},
  {key_34, name_34, op_34},
  {key_35, name_35, op_35},
  {key_36, name_36, op_36},
  {key_37, name_37, op_37},
  {key_38, name_38, op_38},
  {key_39, name_39, op_39},
  {key_40, name_40, op_40},
  {key_41, name_41, op_41},
  {key_42, name_42, op_42},
  {key_43, name_43, op_43},
  {key_44, name_44, op_44},
  {key_45, name_45, op_45},
  {key_46, name_46, op_46},
  {key_47, name_47, op_47},
  {key_48, name_48, op_48},
  {key_49, name_49, op_49},
  {key_50, name_50, op_50},
  {key_51, name_51, op_51},
  {key_52, name_52, op_52},
  {key_53, name_53, op_53},
  {key_54, name_54, op_54},
  {key_55, name_55, op_55},
  {key_56, name_56, op_56},
  {key_57, name_57, op_57},
  {key_58, name_58, op_58},
  {key_59, name_59, op_59},
  {key_60, name_60, op_60},
  {key_61, name_61, op_61},
  {key_62, name_62, op_62},
};
const size_t VePrettyDataSize = sizeof(VePrettyData) / sizeof(VePrettyData[0]); 



/* 

static const char * VePrettyData[][3] PROGMEM {
// get name, pretify name, value operator
{"V","Voltage", "1000",}, // display in webUI | Dont edit
{"V2","Voltage_2", "1000",},
{"V3","Voltage_3", "1000",},
{"VS","Starter_voltage", "1000",},
{"VM","Mid_voltage", "1000",},
{"DM","Mid_deviation", "10",},
{"VPV","Panel_voltage", "1000",}, // display in webUI | Dont edit
{"PPV","Panel_power", "0",}, // display in webUI | Dont edit
{"I","Battery_current","1000"}, // display in webUI | Dont edit
{"I2","Battery_current_2","1000"},
{"I3","Battery_current_3","1000"},
{"IL","Load_current","1000"}, // display in webUI | Dont edit
{"LOAD","Load_output_state",""}, // display in webUI | Dont edit
{"T","Battery_temperature","0"}, // display in webUI | Dont edit
{"P","Instantaneous_power","0"},
{"CE","Consumed_Amp_Hours","1000"},
{"SOC", "SOC", "10"}, // display in webUI | Dont edit
{"TTG","Time_to_go","0"},
{"ALARM","Alarm",""},
{"RELAY","Relay",""}, // display in webUI | Dont edit
{"AR", "Alarm_code", "0"},
{"OR", "Off_reason", "0"},
{"H1", "Deepest_discharge", "1000"},
{"H2", "Last_discharge", "1000"},
{"H3", "Average_discharge", "1000"},
{"H4", "Charge_cycles", "0"},
{"H5", "Full_discharges", "0"},
{"H6", "Cumulative_Ah_drawn", "1000"},
{"H7", "Minimum_voltage", "1000"},
{"H8", "Maximum_voltage", "1000"},
{"H9", "Last_full_charge", "3600"},
{"H10", "Num_automatic_sync", "0"},
{"H11", "Num_low_volt_alarms", "0"},
{"H12", "Num_high_volt_alarms", "0"},
{"H13", "Num_low_aux_vol_alarms", "0"},
{"H14", "Num_high_aux_vol_alarms", "0"},
{"H15", "Min_aux_volt", "1000"},
{"H16", "Max_aux_volt", "1000"},
{"H17", "Amount_discharged_energy", "100"},
{"H18", "Amount_charged_energy", "100"},
{"H19", "total_kWh", "100"},
{"H20", "today_kWh", "100"},
{"H21", "Max_pow_today", "0"},
{"H22", "Yesterday_kWh", "100"},
{"H23", "Max_pow_yesterday", "0"},
{"ERR", "Current_error", "0"},
{"CS", "Operation_state", "0"},
{"BMV", "Model_description", ""},
{"FW", "Firmware_version_16", ""},
{"FWE", "Firmware_version_24", ""},
{"PID","Device_model",""},
{"SER#","Serial_number",""},
{"HSDS","Day","0"},
{"MODE","Device_mode",""},
{"AC_OUT_V","AC_out_volt","100"},
{"AC_OUT_I","AC_out_current","10"},
{"AC_OUT_S","AC_out_apparent_pow",""},
{"WARN","Warning_reason",""},
{"MPPT","Tracker_operation_mode",""},
{"MON","DC_monitor_mode",""},
{"DC_IN_V","DC_input_voltage","100"},
{"DC_IN_I","DC_input_current","10"},
{"DC_IN_P","DC_input_power","0"},
};*/
static const char * haDescriptor[][4] PROGMEM {
      // state_topic, icon, unit_ofmeasurement, class
      {"Voltage", "car-battery", "V", "voltage"},
      {"Voltage_2", "car-battery", "V", "voltage"},
      {"Voltage_3", "car-battery", "V", "voltage"},
      {"Starter_voltage", "flash-triangle-outline", "V", "voltage"},
      {"Mid_voltage", "battery-outline", "V", "voltage"},
      {"Mid_deviation", "battery-outline", "%", "battery"},
      {"Panel_voltage", "solar-panel", "V", "voltage"},
      {"Panel_power","solar-power","W","power"},
      {"Battery_current", "current-dc", "A", "current"},
      {"Battery_current_2", "current-dc", "A", "current"},
      {"Battery_current_3", "current-dc", "A", "current"},
      {"Load_current", "battery-charging", "A", "current"},
      {"Load_output_state", "export", "", ""},
      {"Battery_temperature", "thermometer", "°C", "temperature"},
      {"Instantaneous_power", "solar-power", "W", "power"},
      {"Consumed_Amp_Hours", "home-battery-outline", "Ah", ""},
      {"SOC", "battery-charging", "%", "battery"},
      {"Time_to_go", "camera-timer", "min", "duration"},
      {"Alarm", "alarm-light-outline", "", ""},
      {"Relay", "electric-switch", "", ""},
      {"Alarm_code", "alarm-panel-outline", "", ""},
      {"Off_reason", "alarm-panel-outline", "", ""},
      {"Deepest_discharge", "battery-outline", "Ah", ""},
      {"Last_discharge", "battery-outline", "Ah", ""},
      {"Average_discharge", "battery-outline", "Ah", ""},
      {"Charge_cycles", "counter", "", ""},
      {"Full_discharges", "counter", "", ""},
      {"Cumulative_Ah_drawn", "battery-outline", "Ah", ""},
      {"Minimum_voltage", "battery-charging-outline", "V", "voltage"},
      {"Maximum_voltage", "battery-charging-high", "V", "voltage"},
      {"Last_full_charge", "clock-time-eight-outline", "h", "duration"},
      {"Num_automatic_sync", "counter", "", ""},
      {"Num_low_volt_alarms", "counter", "", ""},
      {"Num_high_volt_alarms", "counter", "", ""},
      {"Num_low_aux_vol_alarms", "counter", "", ""},
      {"Num_high_aux_vol_alarms", "counter", "", ""},
      {"Min_aux_volt", "battery-low", "V", "voltage"},
      {"Max_aux_volt", "battery-high", "V", "voltage"},
      {"Amount_discharged_energy", "battery-arrow-down-outline", "kWh", "energy"},
      {"Amount_charged_energy", "battery-arrow-up-outline", "kWh", "energy"},
      {"total_kWh", "solar-power", "kWh", "energy"},
      {"today_kWh", "solar-power", "kWh", "energy"},
      {"Max_pow_today", "solar-power-variant-outline", "W", "power"},
      {"Yesterday_kWh", "solar-power", "kWh", "energy"},
      {"Max_pow_yesterday", "solar-power-variant-outline", "W", "power"},
      {"Current_error", "alert-circle-outline", "", ""},
      {"Operation_state", "state-machine", "", ""},
      {"Model_description", "select-inverse", "", ""},
      {"Firmware_version_16", "select-inverse", "", ""},
      {"Firmware_version_24", "select-inverse", "", ""},
      {"Device_model", "select-inverse", "", ""},
      {"Serial_number", "format-list-numbered", "", ""},
      {"Day", "calendar-today", "", ""},
      {"Device_mode", "tablet-dashboard", "", ""},
      {"AC_out_volt", "current-ac", "V", "voltage"},
      {"AC_out_current", "current-ac", "A", "current"},
      {"AC_out_apparent_pow", "current-ac", "VA", "apparent_power"},
      {"Warning_reason", "alert-outline", "", ""},
      {"Tracker_operation_mode", "pencil-outline", "", ""},
      {"DC_monitor_mode", "multicast", "", ""},
      {"DC_input_voltage", "current-dc", "V", "voltage"},
      {"DC_input_current", "current-dc", "A", "current"},
      {"DC_input_power", "current-dc", "W", "power"},
      {"Device_connection", "alert-circle-outline", "", ""}
      };