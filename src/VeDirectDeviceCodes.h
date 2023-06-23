// AR
const char *VeDirectDeviceCodeAR[][25]{
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
/*
OR
Off reason, this field described why a unit is switched off.
No input power 0x00000001
Switched off (power switch) 0x00000002
Switched off (device mode register) 0x00000004
Remote input 0x00000008
Protection active 0x00000010
Paygo 0x00000020
BMS 0x00000040
Engine shutdown detection 0x00000080
Analysing input voltage 0x00000100

AP_BLE
Off reason, this field described why a unit is switched off.
BLE supports switching off 0x00000001
BLE switching off is permanent 0x00000002

CS
The state of operation. See the table below for the possible values.
MPPT Inverter Charger
Off 0 • • •
Low power 1 • (1)
Fault 2 • • •
Bulk 3 • •
Absorption 4 • •
Float 5 • •
Storage 6 •
Equalize (manual) 7 •
Inverting 9 •
Power supply 11 •
Starting-up 245 •
Repeated absorption 246 •
Auto equalize / Recondition 247 • •
BatterySafe 248 •
External Control 252

ERR
The error code of the device (relevant when the device is in the fault state). See the table below for
the possible values.
No error 0
Battery voltage too high 2
Charger temperature too high 17
Charger over current 18
Charger current reversed 19
Bulk time limit exceeded 20
Current sensor issue (sensor bias/sensor broken) 21
Terminals overheated 26
Converter issue (dual converter models only) 28
Input voltage too high (solar panel) 33
Input current too high (solar panel) 34
Input shutdown (due to excessive battery voltage) 38
Input shutdown (due to current flow during off mode) 39
Lost communication with one of devices 65
Synchronised charging device configuration issue 66
BMS connection lost 67
Network misconfigured 68
Factory calibration data lost 116
Invalid/incompatible firmware 117
User settings invalid 119
*/