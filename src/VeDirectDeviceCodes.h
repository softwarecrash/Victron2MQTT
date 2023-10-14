// AR (Alarm_code)
static const char * const VeDirectDeviceCodeAR[][2]{
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
static const char * const VeDirectDeviceCodeOR[][2]{
    {"0X00000000", "None"},
    {"0X00000001", "No input power"},
    {"0X00000002", "Switched off (power switch)"},
    {"0X00000004", "Switched off (device mode register)"},
    {"0X00000008", "Remote input"},
    {"0X00000010", "Protection active"},
    {"0X00000020", "Paygo"},
    {"0X00000040", "BMS"},
    {"0X00000080", "Engine shutdown detection"},
    {"0X00000100", "Analysing input voltage"},};

// AP_BLE
static const char * const VeDirectDeviceCodeAP_BLE[][2]{
    {"0X00000001", "BLE supports switching off"},
    {"0X00000002", "BLE switching off is permanent"},};



// CS (operation_state)
static const char * const VeDirectDeviceCodeCS[][2]{
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
    {"252", "External Control"},};

// ERR (Current_error)
static const char * const VeDirectDeviceCodeERR[][2]{
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
    {"119", "User settings invalid"},};

// MPPT (Tracker_operation_mode)
static const char * const VeDirectDeviceCodeMPPT[][2]{
    {"0", "Off"},
    {"1", "Voltage or current limited"},
    {"2", "MPP Tracker active"},};