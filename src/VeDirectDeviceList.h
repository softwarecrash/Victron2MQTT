//The product id: PID

const char* VeDirectDeviceList[][2]
{
{"0X203", "BMV-700"},
{"0X204", "BMV-702"},

//data for the Ve Simulator
{"0X9999", "Ve Sim"}
};
/*
BMV-700 0x203
BMV-702 0x204
BMV-700H 0x205
BlueSolar MPPT 70|15* 0x0300*
BlueSolar MPPT 75|50* 0xA040*
BlueSolar MPPT 150|35* 0xA041*
BlueSolar MPPT 75|15 0xA042
BlueSolar MPPT 100|15 0xA043
BlueSolar MPPT 100|30* 0xA044*
BlueSolar MPPT 100|50* 0xA045*
BlueSolar MPPT 150|70 0xA046
BlueSolar MPPT 150|100 0xA047
BlueSolar MPPT 100|50 rev2 0xA049
BlueSolar MPPT 100|30 rev2 0xA04A
BlueSolar MPPT 150|35 rev2 0xA04B
BlueSolar MPPT 75|10 0xA04C
BlueSolar MPPT 150|45 0xA04D
BlueSolar MPPT 150|60 0xA04E
BlueSolar MPPT 150|85 0xA04F
SmartSolar MPPT 250|100 0xA050
SmartSolar MPPT 150|100* 0xA051*
SmartSolar MPPT 150|85* 0xA052*
SmartSolar MPPT 75|15 0xA053
SmartSolar MPPT 75|10 0xA054
SmartSolar MPPT 100|15 0xA055
SmartSolar MPPT 100|30 0xA056
SmartSolar MPPT 100|50 0xA057
SmartSolar MPPT 150|35 0xA058
SmartSolar MPPT 150|100 rev2 0xA059
SmartSolar MPPT 150|85 rev2 0xA05A
SmartSolar MPPT 250|70 0xA05B
SmartSolar MPPT 250|85 0xA05C
SmartSolar MPPT 250|60 0xA05D
SmartSolar MPPT 250|45 0xA05E
SmartSolar MPPT 100|20 0xA05F
SmartSolar MPPT 100|20 48V 0xA060
SmartSolar MPPT 150|45 0xA061
SmartSolar MPPT 150|60 0xA062
SmartSolar MPPT 150|70 0xA063
SmartSolar MPPT 250|85 rev2 0xA064
SmartSolar MPPT 250|100 rev2 0xA065
BlueSolar MPPT 100|20 0xA066
BlueSolar MPPT 100|20 48V 0xA067
SmartSolar MPPT 250|60 rev2 0xA068
SmartSolar MPPT 250|70 rev2 0xA069
SmartSolar MPPT 150|45 rev2 0xA06A
SmartSolar MPPT 150|60 rev2 0xA06B
SmartSolar MPPT 150|70 rev2 0xA06C
SmartSolar MPPT 150|85 rev3 0xA06D
SmartSolar MPPT 150|100 rev3 0xA06E
BlueSolar MPPT 150|45 rev2 0xA06F
BlueSolar MPPT 150|60 rev2 0xA070
BlueSolar MPPT 150|70 rev2 0xA071
BlueSolar MPPT 150/45 rev3 0xA072
SmartSolar MPPT 150/45 rev3 0xA073
SmartSolar MPPT 75/10 rev2 0xA074
SmartSolar MPPT 75/15 rev2 0xA075
BlueSolar MPPT 100/30 rev3 0xA076
BlueSolar MPPT 100/50 rev3 0xA077
BlueSolar MPPT 150/35 rev3 0xA078
BlueSolar MPPT 75/10 rev2 0xA079
BlueSolar MPPT 75/15 rev2 0xA07A
BlueSolar MPPT 100/15 rev2 0xA07B
BlueSolar MPPT 75/10 rev3 0xA07C
BlueSolar MPPT 75/15 rev3 0xA07D
SmartSolar MPPT 100/30 12V 0xA07E
All-In-1 SmartSolar MPPT 75/15 12V 0xA07F
SmartSolar MPPT VE.Can 150/70 0xA102
SmartSolar MPPT VE.Can 150/45 0xA103
SmartSolar MPPT VE.Can 150/60 0xA104
SmartSolar MPPT VE.Can 150/85 0xA105
SmartSolar MPPT VE.Can 150/100 0xA106
SmartSolar MPPT VE.Can 250/45 0xA107
SmartSolar MPPT VE.Can 250/60 0xA108
SmartSolar MPPT VE.Can 250/70 0xA109
SmartSolar MPPT VE.Can 250/85 0xA10A
SmartSolar MPPT VE.Can 250/100 0xA10B
SmartSolar MPPT VE.Can 150/70 rev2 0xA10C
SmartSolar MPPT VE.Can 150/85 rev2 0xA10D
SmartSolar MPPT VE.Can 150/100 rev2 0xA10E
BlueSolar MPPT VE.Can 150/100 0xA10F
BlueSolar MPPT VE.Can 250/70 0xA112
BlueSolar MPPT VE.Can 250/100 0xA113
SmartSolar MPPT VE.Can 250/70 rev2 0xA114
SmartSolar MPPT VE.Can 250/100 rev2 0xA115
SmartSolar MPPT VE.Can 250/85 rev2 0xA116
BlueSolar MPPT VE.Can 150/100 rev2 0xA117
Phoenix Inverter 12V 250VA 230V* 0xA201*
Phoenix Inverter 24V 250VA 230V* 0xA202*
Phoenix Inverter 48V 250VA 230V* 0xA204*
Phoenix Inverter 12V 375VA 230V* 0xA211*
Phoenix Inverter 24V 375VA 230V* 0xA212*
Phoenix Inverter 48V 375VA 230V* 0xA214*
Phoenix Inverter 12V 500VA 230V* 0xA221*
Phoenix Inverter 24V 500VA 230V* 0xA222*
Phoenix Inverter 48V 500VA 230V* 0xA224*
Phoenix Inverter 12V 250VA 230V 0xA231
Phoenix Inverter 24V 250VA 230V 0xA232
Phoenix Inverter 48V 250VA 230V 0xA234
Phoenix Inverter 12V 250VA 120V 0xA239
Phoenix Inverter 24V 250VA 120V 0xA23A
Phoenix Inverter 48V 250VA 120V 0xA23C
Phoenix Inverter 12V 375VA 230V 0xA241
Phoenix Inverter 24V 375VA 230V 0xA242
Phoenix Inverter 48V 375VA 230V 0xA244
Phoenix Inverter 12V 375VA 120V 0xA249
Phoenix Inverter 24V 375VA 120V 0xA24A
Phoenix Inverter 48V 375VA 120V 0xA24C
Phoenix Inverter 12V 500VA 230V 0xA251
Phoenix Inverter 24V 500VA 230V 0xA252
Phoenix Inverter 48V 500VA 230V 0xA254
Phoenix Inverter 12V 500VA 120V 0xA259
Phoenix Inverter 24V 500VA 120V 0xA25A
Phoenix Inverter 48V 500VA 120V 0xA25C
Phoenix Inverter 12V 800VA 230V 0xA261
Phoenix Inverter 24V 800VA 230V 0xA262
Phoenix Inverter 48V 800VA 230V 0xA264
Phoenix Inverter 12V 800VA 120V 0xA269
Phoenix Inverter 24V 800VA 120V 0xA26A
Phoenix Inverter 48V 800VA 120V 0xA26C
Phoenix Inverter 12V 1200VA 230V 0xA271
Phoenix Inverter 24V 1200VA 230V 0xA272
Phoenix Inverter 48V 1200VA 230V 0xA274
Phoenix Inverter 12V 1200VA 120V 0xA279
Phoenix Inverter 24V 1200VA 120V 0xA27A
Phoenix Inverter 48V 1200VA 120V 0xA27C
Phoenix Inverter 12V 1600VA 230V 0xA281
Phoenix Inverter 24V 1600VA 230V 0xA282
Phoenix Inverter 48V 1600VA 230V 0xA284
Phoenix Inverter 12V 2000VA 230V 0xA291
Phoenix Inverter 24V 2000VA 230V 0xA292
Phoenix Inverter 48V 2000VA 230V 0xA294
Phoenix Inverter 12V 3000VA 230V 0xA2A1
Phoenix Inverter 24V 3000VA 230V 0xA2A2
Phoenix Inverter 48V 3000VA 230V 0xA2A4
Phoenix Smart IP43 Charger 12|50 (1+1) 0xA340
Phoenix Smart IP43 Charger 12|50 (3) 0xA341
Phoenix Smart IP43 Charger 24|25 (1+1) 0xA342
Phoenix Smart IP43 Charger 24|25 (3) 0xA343
Phoenix Smart IP43 Charger 12|30 (1+1) 0xA344
Phoenix Smart IP43 Charger 12|30 (3) 0xA345
Phoenix Smart IP43 Charger 24|16 (1+1) 0xA346
Phoenix Smart IP43 Charger 24|16 (3) 0xA347
BMV-712 Smart 0xA381
BMV-710H Smart 0xA382
BMV-712 Smart Rev2 0xA383
SmartShunt 500A/50mV 0xA389
SmartShunt 1000A/50mV 0xA38A
SmartShunt 2000A/50mV 0xA38B
Smart BuckBoost 12V/12V-50A 0xA3F0
*/