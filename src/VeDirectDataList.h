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
};
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
      {"Deepest_discharge", "battery-outline", "Ah", "energy"},
      {"Last_discharge", "battery-outline", "Ah", "energy"},
      {"Average_discharge", "battery-outline", "Ah", "energy"},
      {"Charge_cycles", "counter", "", ""},
      {"Full_discharges", "counter", "", ""},
      {"Cumulative_Ah_drawn", "battery-outline", "Ah", "energy"},
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