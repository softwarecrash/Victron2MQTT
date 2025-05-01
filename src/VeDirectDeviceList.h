// The product id: PID
#pragma once
#include <Arduino.h>
struct VeDeviceEntry {
  uint16_t id;
  const char* name;
};
const char name_203[] PROGMEM = "BMV-700";
const char name_204[] PROGMEM = "BMV-702";
const char name_205[] PROGMEM = "BMV-700H";
const char name_0300[] PROGMEM = "BlueSolar MPPT 70|15";
const char name_A040[] PROGMEM = "BlueSolar MPPT 75|50";
const char name_A041[] PROGMEM = "BlueSolar MPPT 150|35";
const char name_A042[] PROGMEM = "BlueSolar MPPT 75|15";
const char name_A043[] PROGMEM = "BlueSolar MPPT 100|15";
const char name_A044[] PROGMEM = "BlueSolar MPPT 100|30";
const char name_A045[] PROGMEM = "BlueSolar MPPT 100|50";
const char name_A046[] PROGMEM = "BlueSolar MPPT 150|70";
const char name_A047[] PROGMEM = "BlueSolar MPPT 150|100 ";
const char name_A049[] PROGMEM = "BlueSolar MPPT 100|50 rev2";
const char name_A04A[] PROGMEM = "BlueSolar MPPT 100|30 rev2";
const char name_A04B[] PROGMEM = "BlueSolar MPPT 150|35";
const char name_A04C[] PROGMEM = "BlueSolar MPPT 75|10";
const char name_A04D[] PROGMEM = "BlueSolar MPPT 150|45";
const char name_A04E[] PROGMEM = "BlueSolar MPPT 150|60";
const char name_A04F[] PROGMEM = "BlueSolar MPPT 150|85";
const char name_A050[] PROGMEM = "SmartSolar MPPT 250|100";
const char name_A051[] PROGMEM = "SmartSolar MPPT 150|100";
const char name_A052[] PROGMEM = "SmartSolar MPPT 150|85";
const char name_A053[] PROGMEM = "SmartSolar MPPT 75|15";
const char name_A054[] PROGMEM = "SmartSolar MPPT 75|10";
const char name_A055[] PROGMEM = "SmartSolar MPPT 100|15";
const char name_A056[] PROGMEM = "SmartSolar MPPT 100|30";
const char name_A057[] PROGMEM = "SmartSolar MPPT 100|50";
const char name_A058[] PROGMEM = "SmartSolar MPPT 150|35";
const char name_A059[] PROGMEM = "SmartSolar MPPT 150|100 rev2";
const char name_A05A[] PROGMEM = "SmartSolar MPPT 150|85 rev2";
const char name_A05B[] PROGMEM = "SmartSolar MPPT 250|70";
const char name_A05C[] PROGMEM = "SmartSolar MPPT 250|85";
const char name_A05D[] PROGMEM = "SmartSolar MPPT 250|60";
const char name_A05E[] PROGMEM = "SmartSolar MPPT 250|45";
const char name_A05F[] PROGMEM = "SmartSolar MPPT 100|20";
const char name_A060[] PROGMEM = "SmartSolar MPPT 100|20 48V";
const char name_A061[] PROGMEM = "SmartSolar MPPT 150|45";
const char name_A062[] PROGMEM = "SmartSolar MPPT 150|60";
const char name_A063[] PROGMEM = "SmartSolar MPPT 150|70";
const char name_A064[] PROGMEM = "SmartSolar MPPT 250|85 rev2";
const char name_A065[] PROGMEM = "SmartSolar MPPT 250|100 rev2";
const char name_A066[] PROGMEM = "BlueSolar MPPT 100|20";
const char name_A067[] PROGMEM = "BlueSolar MPPT 100|20 48V";
const char name_A068[] PROGMEM = "SmartSolar MPPT 250|60 rev2";
const char name_A069[] PROGMEM = "SmartSolar MPPT 250|70 rev2";
const char name_A06A[] PROGMEM = "SmartSolar MPPT 150|45 rev2";
const char name_A06B[] PROGMEM = "SmartSolar MPPT 150|60 rev2";
const char name_A06C[] PROGMEM = "SmartSolar MPPT 150|70 rev2";
const char name_A06D[] PROGMEM = "SmartSolar MPPT 150|85 rev3";
const char name_A06E[] PROGMEM = "SmartSolar MPPT 150|100 rev3";
const char name_A06F[] PROGMEM = "BlueSolar MPPT 150|45 rev2";
const char name_A070[] PROGMEM = "BlueSolar MPPT 150|60 rev2";
const char name_A071[] PROGMEM = "BlueSolar MPPT 150|70 rev2";
const char name_A072[] PROGMEM = "BlueSolar MPPT 150/45 rev3";
const char name_A073[] PROGMEM = "SmartSolar MPPT 150/45 rev3";
const char name_A074[] PROGMEM = "SmartSolar MPPT 75/10 rev2";
const char name_A075[] PROGMEM = "SmartSolar MPPT 75/15 rev2";
const char name_A076[] PROGMEM = "BlueSolar MPPT 100/30 rev3";
const char name_A077[] PROGMEM = "BlueSolar MPPT 100/50 rev3";
const char name_A078[] PROGMEM = "BlueSolar MPPT 150/35 rev3";
const char name_A079[] PROGMEM = "BlueSolar MPPT 75/10 rev2";
const char name_A07A[] PROGMEM = "BlueSolar MPPT 75/15 rev2";
const char name_A07B[] PROGMEM = "BlueSolar MPPT 100/15 rev2";
const char name_A07C[] PROGMEM = "BlueSolar MPPT 75/10 rev3";
const char name_A07D[] PROGMEM = "BlueSolar MPPT 75/15 rev3";
const char name_A07E[] PROGMEM = "SmartSolar MPPT 100/30 12V";
const char name_A07F[] PROGMEM = "All-In-1 SmartSolar MPPT 75/15 12V";
const char name_A102[] PROGMEM = "SmartSolar MPPT VE.Can 150/70";
const char name_A103[] PROGMEM = "SmartSolar MPPT VE.Can 150/45";
const char name_A104[] PROGMEM = "SmartSolar MPPT VE.Can 150/60";
const char name_A105[] PROGMEM = "SmartSolar MPPT VE.Can 150/85";
const char name_A106[] PROGMEM = "SmartSolar MPPT VE.Can 150/100";
const char name_A107[] PROGMEM = "SmartSolar MPPT VE.Can 250/45";
const char name_A108[] PROGMEM = "SmartSolar MPPT VE.Can 250/60";
const char name_A109[] PROGMEM = "SmartSolar MPPT VE.Can 250/70";
const char name_A10A[] PROGMEM = "SmartSolar MPPT VE.Can 250/85";
const char name_A10B[] PROGMEM = "SmartSolar MPPT VE.Can 250/100";
const char name_A10C[] PROGMEM = "SmartSolar MPPT VE.Can 150/70 rev2";
const char name_A10D[] PROGMEM = "SmartSolar MPPT VE.Can 150/85 rev2";
const char name_A10E[] PROGMEM = "SmartSolar MPPT VE.Can 150/100 rev2";
const char name_A10F[] PROGMEM = "BlueSolar MPPT VE.Can 150/100";
const char name_A112[] PROGMEM = "BlueSolar MPPT VE.Can 250/70";
const char name_A113[] PROGMEM = "BlueSolar MPPT VE.Can 250/100";
const char name_A114[] PROGMEM = "SmartSolar MPPT VE.Can 250/70 rev2";
const char name_A115[] PROGMEM = "SmartSolar MPPT VE.Can 250/100 rev2";
const char name_A116[] PROGMEM = "SmartSolar MPPT VE.Can 250/85 rev2";
const char name_A117[] PROGMEM = "BlueSolar MPPT VE.Can 150/100 rev2";
const char name_A201[] PROGMEM = "Phoenix Inverter 12V 250VA 230V";
const char name_A202[] PROGMEM = "Phoenix Inverter 24V 250VA 230V";
const char name_A204[] PROGMEM = "Phoenix Inverter 48V 250VA 230V";
const char name_A211[] PROGMEM = "Phoenix Inverter 12V 375VA 230V";
const char name_A212[] PROGMEM = "Phoenix Inverter 24V 375VA 230V";
const char name_A214[] PROGMEM = "Phoenix Inverter 48V 375VA 230V";
const char name_A221[] PROGMEM = "Phoenix Inverter 12V 500VA 230V";
const char name_A222[] PROGMEM = "Phoenix Inverter 24V 500VA 230V";
const char name_A224[] PROGMEM = "Phoenix Inverter 48V 500VA 230V";
const char name_A231[] PROGMEM = "Phoenix Inverter 12V 250VA 230V";
const char name_A232[] PROGMEM = "Phoenix Inverter 24V 250VA 230V";
const char name_A234[] PROGMEM = "Phoenix Inverter 48V 250VA 230V";
const char name_A239[] PROGMEM = "Phoenix Inverter 12V 250VA 120V";
const char name_A23A[] PROGMEM = "Phoenix Inverter 24V 250VA 120V";
const char name_A23C[] PROGMEM = "Phoenix Inverter 48V 250VA 120V";
const char name_A241[] PROGMEM = "Phoenix Inverter 12V 375VA 230V";
const char name_A242[] PROGMEM = "Phoenix Inverter 24V 375VA 230V";
const char name_A244[] PROGMEM = "Phoenix Inverter 48V 375VA 230V";
const char name_A249[] PROGMEM = "Phoenix Inverter 12V 375VA 120V";
const char name_A24A[] PROGMEM = "Phoenix Inverter 24V 375VA 120V";
const char name_A24C[] PROGMEM = "Phoenix Inverter 48V 375VA 120V";
const char name_A251[] PROGMEM = "Phoenix Inverter 12V 500VA 230V";
const char name_A252[] PROGMEM = "Phoenix Inverter 24V 500VA 230V";
const char name_A254[] PROGMEM = "Phoenix Inverter 48V 500VA 230V";
const char name_A259[] PROGMEM = "Phoenix Inverter 12V 500VA 120V";
const char name_A25A[] PROGMEM = "Phoenix Inverter 24V 500VA 120V";
const char name_A25C[] PROGMEM = "Phoenix Inverter 48V 500VA 120V";
const char name_A261[] PROGMEM = "Phoenix Inverter 12V 800VA 230V";
const char name_A262[] PROGMEM = "Phoenix Inverter 24V 800VA 230V";
const char name_A264[] PROGMEM = "Phoenix Inverter 48V 800VA 230V";
const char name_A269[] PROGMEM = "Phoenix Inverter 12V 800VA 120V";
const char name_A26A[] PROGMEM = "Phoenix Inverter 24V 800VA 120V";
const char name_A26C[] PROGMEM = "Phoenix Inverter 48V 800VA 120V";
const char name_A271[] PROGMEM = "Phoenix Inverter 12V 1200VA 230V";
const char name_A272[] PROGMEM = "Phoenix Inverter 24V 1200VA 230V";
const char name_A274[] PROGMEM = "Phoenix Inverter 48V 1200VA 230V";
const char name_A279[] PROGMEM = "Phoenix Inverter 12V 1200VA 120V";
const char name_A27A[] PROGMEM = "Phoenix Inverter 24V 1200VA 120V";
const char name_A27C[] PROGMEM = "Phoenix Inverter 48V 1200VA 120V";
const char name_A281[] PROGMEM = "Phoenix Inverter 12V 1600VA 230V";
const char name_A282[] PROGMEM = "Phoenix Inverter 24V 1600VA 230V";
const char name_A284[] PROGMEM = "Phoenix Inverter 48V 1600VA 230V";
const char name_A291[] PROGMEM = "Phoenix Inverter 12V 2000VA 230V";
const char name_A292[] PROGMEM = "Phoenix Inverter 24V 2000VA 230V";
const char name_A294[] PROGMEM = "Phoenix Inverter 48V 2000VA 230V";
const char name_A2A1[] PROGMEM = "Phoenix Inverter 12V 3000VA 230V";
const char name_A2A2[] PROGMEM = "Phoenix Inverter 24V 3000VA 230V";
const char name_A2A4[] PROGMEM = "Phoenix Inverter 48V 3000VA 230V";
const char name_A340[] PROGMEM = "Phoenix Smart IP43 Charger 12|50 (1+1)";
const char name_A341[] PROGMEM = "Phoenix Smart IP43 Charger 12|50 (3)";
const char name_A342[] PROGMEM = "Phoenix Smart IP43 Charger 24|25 (1+1)";
const char name_A343[] PROGMEM = "Phoenix Smart IP43 Charger 24|25 (3)";
const char name_A344[] PROGMEM = "Phoenix Smart IP43 Charger 12|30 (1+1)";
const char name_A345[] PROGMEM = "Phoenix Smart IP43 Charger 12|30 (3)";
const char name_A346[] PROGMEM = "Phoenix Smart IP43 Charger 24|16 (1+1)";
const char name_A347[] PROGMEM = "Phoenix Smart IP43 Charger 24|16 (3)";
const char name_A381[] PROGMEM = "BMV-712 Smart";
const char name_A382[] PROGMEM = "BMV-710H Smart";
const char name_A383[] PROGMEM = "BMV-712 Smart Rev2";
const char name_A389[] PROGMEM = "SmartShunt 500A/50mV";
const char name_A38A[] PROGMEM = "SmartShunt 1000A/50mV";
const char name_A38B[] PROGMEM = "SmartShunt 2000A/50mV";
const char name_A3F0[] PROGMEM = "Smart BuckBoost 12V/12V-50A";
const char name_C030[] PROGMEM = "SmartShunt 500A/50mV IP65";
const char name_9999[] PROGMEM = "The migthy simluator";

const VeDeviceEntry VeDeviceList[] PROGMEM = {
  {0x0203, name_203},
  {0x0204, name_204},
  {0x0205, name_205},
  {0x0300, name_0300},
  {0x9999, name_9999},
  {0xA040, name_A040},
  {0xA041, name_A041},
  {0xA042, name_A042},
  {0xA043, name_A043},
  {0xA044, name_A044},
  {0xA045, name_A045},
  {0xA046, name_A046},
  {0xA047, name_A047},
  {0xA049, name_A049},
  {0xA04A, name_A04A},
  {0xA04B, name_A04B},
  {0xA04C, name_A04C},
  {0xA04D, name_A04D},
  {0xA04E, name_A04E},
  {0xA04F, name_A04F},
  {0xA050, name_A050},
  {0xA051, name_A051},
  {0xA052, name_A052},
  {0xA053, name_A053},
  {0xA054, name_A054},
  {0xA055, name_A055},
  {0xA056, name_A056},
  {0xA057, name_A057},
  {0xA058, name_A058},
  {0xA059, name_A059},
  {0xA05A, name_A05A},
  {0xA05B, name_A05B},
  {0xA05C, name_A05C},
  {0xA05D, name_A05D},
  {0xA05E, name_A05E},
  {0xA05F, name_A05F},
  {0xA060, name_A060},
  {0xA061, name_A061},
  {0xA062, name_A062},
  {0xA063, name_A063},
  {0xA064, name_A064},
  {0xA065, name_A065},
  {0xA066, name_A066},
  {0xA067, name_A067},
  {0xA068, name_A068},
  {0xA069, name_A069},
  {0xA06A, name_A06A},
  {0xA06B, name_A06B},
  {0xA06C, name_A06C},
  {0xA06D, name_A06D},
  {0xA06E, name_A06E},
  {0xA06F, name_A06F},
  {0xA070, name_A070},
  {0xA071, name_A071},
  {0xA072, name_A072},
  {0xA073, name_A073},
  {0xA074, name_A074},
  {0xA075, name_A075},
  {0xA076, name_A076},
  {0xA077, name_A077},
  {0xA078, name_A078},
  {0xA079, name_A079},
  {0xA07A, name_A07A},
  {0xA07B, name_A07B},
  {0xA07C, name_A07C},
  {0xA07D, name_A07D},
  {0xA07E, name_A07E},
  {0xA07F, name_A07F},
  {0xA102, name_A102},
  {0xA103, name_A103},
  {0xA104, name_A104},
  {0xA105, name_A105},
  {0xA106, name_A106},
  {0xA107, name_A107},
  {0xA108, name_A108},
  {0xA109, name_A109},
  {0xA10A, name_A10A},
  {0xA10B, name_A10B},
  {0xA10C, name_A10C},
  {0xA10D, name_A10D},
  {0xA10E, name_A10E},
  {0xA10F, name_A10F},
  {0xA112, name_A112},
  {0xA113, name_A113},
  {0xA114, name_A114},
  {0xA115, name_A115},
  {0xA116, name_A116},
  {0xA117, name_A117},
  {0xA201, name_A201},
  {0xA202, name_A202},
  {0xA204, name_A204},
  {0xA211, name_A211},
  {0xA212, name_A212},
  {0xA214, name_A214},
  {0xA221, name_A221},
  {0xA222, name_A222},
  {0xA224, name_A224},
  {0xA231, name_A231},
  {0xA232, name_A232},
  {0xA234, name_A234},
  {0xA239, name_A239},
  {0xA23A, name_A23A},
  {0xA23C, name_A23C},
  {0xA241, name_A241},
  {0xA242, name_A242},
  {0xA244, name_A244},
  {0xA249, name_A249},
  {0xA24A, name_A24A},
  {0xA24C, name_A24C},
  {0xA251, name_A251},
  {0xA252, name_A252},
  {0xA254, name_A254},
  {0xA259, name_A259},
  {0xA25A, name_A25A},
  {0xA25C, name_A25C},
  {0xA261, name_A261},
  {0xA262, name_A262},
  {0xA264, name_A264},
  {0xA269, name_A269},
  {0xA26A, name_A26A},
  {0xA26C, name_A26C},
  {0xA271, name_A271},
  {0xA272, name_A272},
  {0xA274, name_A274},
  {0xA279, name_A279},
  {0xA27A, name_A27A},
  {0xA27C, name_A27C},
  {0xA281, name_A281},
  {0xA282, name_A282},
  {0xA284, name_A284},
  {0xA291, name_A291},
  {0xA292, name_A292},
  {0xA294, name_A294},
  {0xA2A1, name_A2A1},
  {0xA2A2, name_A2A2},
  {0xA2A4, name_A2A4},
  {0xA340, name_A340},
  {0xA341, name_A341},
  {0xA342, name_A342},
  {0xA343, name_A343},
  {0xA344, name_A344},
  {0xA345, name_A345},
  {0xA346, name_A346},
  {0xA347, name_A347},
  {0xA381, name_A381},
  {0xA382, name_A382},
  {0xA383, name_A383},
  {0xA389, name_A389},
  {0xA38A, name_A38A},
  {0xA38B, name_A38B},
  {0xA3F0, name_A3F0},
  {0xC030, name_C030},
};

const size_t VeDeviceListSize = sizeof(VeDeviceList) / sizeof(VeDeviceList[0]);


/* 
// The product id: PID

static const char *VeDirectDeviceList[][2] PROGMEM{
    {"0X203", "BMV-700"},
    {"0X204", "BMV-702"},
    {"0X205", "BMV-700H"},
    {"0X0300", "BlueSolar MPPT 70|15"},
    {"0XA040", "BlueSolar MPPT 75|50"},
    {"0XA041", "BlueSolar MPPT 150|35"},
    {"0XA042", "BlueSolar MPPT 75|15"},
    {"0XA043", "BlueSolar MPPT 100|15"},
    {"0XA044", "BlueSolar MPPT 100|30"},
    {"0XA045", "BlueSolar MPPT 100|50"},
    {"0XA046", "BlueSolar MPPT 150|70"},
    {"0XA047", "BlueSolar MPPT 150|100 "},
    {"0XA049", "BlueSolar MPPT 100|50 rev2"},
    {"0XA04A", "BlueSolar MPPT 100|30 rev2"},
    {"0XA04B", "BlueSolar MPPT 150|35"},
    {"0XA04C", "BlueSolar MPPT 75|10"},
    {"0XA04D", "BlueSolar MPPT 150|45"},
    {"0XA04E", "BlueSolar MPPT 150|60"},
    {"0XA04F", "BlueSolar MPPT 150|85"},
    {"0XA050", "SmartSolar MPPT 250|100"},
    {"0XA051", "SmartSolar MPPT 150|100"},
    {"0XA052", "SmartSolar MPPT 150|85"},
    {"0XA053", "SmartSolar MPPT 75|15"},
    {"0XA054", "SmartSolar MPPT 75|10"},
    {"0XA055", "SmartSolar MPPT 100|15"},
    {"0XA056", "SmartSolar MPPT 100|30"},
    {"0XA057", "SmartSolar MPPT 100|50"},
    {"0XA058", "SmartSolar MPPT 150|35"},
    {"0XA059", "SmartSolar MPPT 150|100 rev2"},
    {"0XA05A", "SmartSolar MPPT 150|85 rev2"},
    {"0XA05B", "SmartSolar MPPT 250|70"},
    {"0XA05C", "SmartSolar MPPT 250|85"},
    {"0XA05D", "SmartSolar MPPT 250|60"},
    {"0XA05E", "SmartSolar MPPT 250|45"},
    {"0XA05F", "SmartSolar MPPT 100|20"},
    {"0XA060", "SmartSolar MPPT 100|20 48V"},
    {"0XA061", "SmartSolar MPPT 150|45"},
    {"0XA062", "SmartSolar MPPT 150|60"},
    {"0XA063", "SmartSolar MPPT 150|70"},
    {"0XA064", "SmartSolar MPPT 250|85 rev2"},
    {"0XA065", "SmartSolar MPPT 250|100 rev2"},
    {"0XA066", "BlueSolar MPPT 100|20"},
    {"0XA067", "BlueSolar MPPT 100|20 48V"},
    {"0XA068", "SmartSolar MPPT 250|60 rev2"},
    {"0XA069", "SmartSolar MPPT 250|70 rev2"},
    {"0XA06A", "SmartSolar MPPT 150|45 rev2"},
    {"0XA06B", "SmartSolar MPPT 150|60 rev2"},
    {"0XA06C", "SmartSolar MPPT 150|70 rev2"},
    {"0XA06D", "SmartSolar MPPT 150|85 rev3"},
    {"0XA06E", "SmartSolar MPPT 150|100 rev3"},
    {"0XA06F", "BlueSolar MPPT 150|45 rev2"},
    {"0XA070", "BlueSolar MPPT 150|60 rev2"},
    {"0XA071", "BlueSolar MPPT 150|70 rev2"},
    {"0XA072", "BlueSolar MPPT 150/45 rev3"},
    {"0XA073", "SmartSolar MPPT 150/45 rev3"},
    {"0XA074", "SmartSolar MPPT 75/10 rev2"},
    {"0XA075", "SmartSolar MPPT 75/15 rev2"},
    {"0XA076", "BlueSolar MPPT 100/30 rev3"},
    {"0XA077", "BlueSolar MPPT 100/50 rev3"},
    {"0XA078", "BlueSolar MPPT 150/35 rev3"},
    {"0XA079", "BlueSolar MPPT 75/10 rev2"},
    {"0XA07A", "BlueSolar MPPT 75/15 rev2"},
    {"0XA07B", "BlueSolar MPPT 100/15 rev2"},
    {"0XA07C", "BlueSolar MPPT 75/10 rev3"},
    {"0XA07D", "BlueSolar MPPT 75/15 rev3"},
    {"0XA07E", "SmartSolar MPPT 100/30 12V"},
    {"0XA07F", "All-In-1 SmartSolar MPPT 75/15 12V"},
    {"0XA102", "SmartSolar MPPT VE.Can 150/70"},
    {"0XA103", "SmartSolar MPPT VE.Can 150/45"},
    {"0XA104", "SmartSolar MPPT VE.Can 150/60"},
    {"0XA105", "SmartSolar MPPT VE.Can 150/85"},
    {"0XA106", "SmartSolar MPPT VE.Can 150/100"},
    {"0XA107", "SmartSolar MPPT VE.Can 250/45"},
    {"0XA108", "SmartSolar MPPT VE.Can 250/60"},
    {"0XA109", "SmartSolar MPPT VE.Can 250/70"},
    {"0XA10A", "SmartSolar MPPT VE.Can 250/85"},
    {"0XA10B", "SmartSolar MPPT VE.Can 250/100"},
    {"0XA10C", "SmartSolar MPPT VE.Can 150/70 rev2"},
    {"0XA10D", "SmartSolar MPPT VE.Can 150/85 rev2"},
    {"0XA10E", "SmartSolar MPPT VE.Can 150/100 rev2"},
    {"0XA10F", "BlueSolar MPPT VE.Can 150/100"},
    {"0XA112", "BlueSolar MPPT VE.Can 250/70"},
    {"0XA113", "BlueSolar MPPT VE.Can 250/100"},
    {"0XA114", "SmartSolar MPPT VE.Can 250/70 rev2"},
    {"0XA115", "SmartSolar MPPT VE.Can 250/100 rev2"},
    {"0XA116", "SmartSolar MPPT VE.Can 250/85 rev2"},
    {"0XA117", "BlueSolar MPPT VE.Can 150/100 rev2"},
    {"0XA201", "Phoenix Inverter 12V 250VA 230V"},
    {"0XA202", "Phoenix Inverter 24V 250VA 230V"},
    {"0XA204", "Phoenix Inverter 48V 250VA 230V"},
    {"0XA211", "Phoenix Inverter 12V 375VA 230V"},
    {"0XA212", "Phoenix Inverter 24V 375VA 230V"},
    {"0XA214", "Phoenix Inverter 48V 375VA 230V"},
    {"0XA221", "Phoenix Inverter 12V 500VA 230V"},
    {"0XA222", "Phoenix Inverter 24V 500VA 230V"},
    {"0XA224", "Phoenix Inverter 48V 500VA 230V"},
    {"0XA231", "Phoenix Inverter 12V 250VA 230V"},
    {"0XA232", "Phoenix Inverter 24V 250VA 230V"},
    {"0XA234", "Phoenix Inverter 48V 250VA 230V"},
    {"0XA239", "Phoenix Inverter 12V 250VA 120V"},
    {"0XA23A", "Phoenix Inverter 24V 250VA 120V"},
    {"0XA23C", "Phoenix Inverter 48V 250VA 120V"},
    {"0XA241", "Phoenix Inverter 12V 375VA 230V"},
    {"0XA242", "Phoenix Inverter 24V 375VA 230V"},
    {"0XA244", "Phoenix Inverter 48V 375VA 230V"},
    {"0XA249", "Phoenix Inverter 12V 375VA 120V"},
    {"0XA24A", "Phoenix Inverter 24V 375VA 120V"},
    {"0XA24C", "Phoenix Inverter 48V 375VA 120V"},
    {"0XA251", "Phoenix Inverter 12V 500VA 230V"},
    {"0XA252", "Phoenix Inverter 24V 500VA 230V"},
    {"0XA254", "Phoenix Inverter 48V 500VA 230V"},
    {"0XA259", "Phoenix Inverter 12V 500VA 120V"},
    {"0XA25A", "Phoenix Inverter 24V 500VA 120V"},
    {"0XA25C", "Phoenix Inverter 48V 500VA 120V"},
    {"0XA261", "Phoenix Inverter 12V 800VA 230V"},
    {"0XA262", "Phoenix Inverter 24V 800VA 230V"},
    {"0XA264", "Phoenix Inverter 48V 800VA 230V"},
    {"0XA269", "Phoenix Inverter 12V 800VA 120V"},
    {"0XA26A", "Phoenix Inverter 24V 800VA 120V"},
    {"0XA26C", "Phoenix Inverter 48V 800VA 120V"},
    {"0XA271", "Phoenix Inverter 12V 1200VA 230V"},
    {"0XA272", "Phoenix Inverter 24V 1200VA 230V"},
    {"0XA274", "Phoenix Inverter 48V 1200VA 230V"},
    {"0XA279", "Phoenix Inverter 12V 1200VA 120V"},
    {"0XA27A", "Phoenix Inverter 24V 1200VA 120V"},
    {"0XA27C", "Phoenix Inverter 48V 1200VA 120V"},
    {"0XA281", "Phoenix Inverter 12V 1600VA 230V"},
    {"0XA282", "Phoenix Inverter 24V 1600VA 230V"},
    {"0XA284", "Phoenix Inverter 48V 1600VA 230V"},
    {"0XA291", "Phoenix Inverter 12V 2000VA 230V"},
    {"0XA292", "Phoenix Inverter 24V 2000VA 230V"},
    {"0XA294", "Phoenix Inverter 48V 2000VA 230V"},
    {"0XA2A1", "Phoenix Inverter 12V 3000VA 230V"},
    {"0XA2A2", "Phoenix Inverter 24V 3000VA 230V"},
    {"0XA2A4", "Phoenix Inverter 48V 3000VA 230V"},
    {"0XA340", "Phoenix Smart IP43 Charger 12|50 (1+1)"},
    {"0XA341", "Phoenix Smart IP43 Charger 12|50 (3)"},
    {"0XA342", "Phoenix Smart IP43 Charger 24|25 (1+1)"},
    {"0XA343", "Phoenix Smart IP43 Charger 24|25 (3)"},
    {"0XA344", "Phoenix Smart IP43 Charger 12|30 (1+1)"},
    {"0XA345", "Phoenix Smart IP43 Charger 12|30 (3)"},
    {"0XA346", "Phoenix Smart IP43 Charger 24|16 (1+1)"},
    {"0XA347", "Phoenix Smart IP43 Charger 24|16 (3)"},
    {"0XA381", "BMV-712 Smart"},
    {"0XA382", "BMV-710H Smart"},
    {"0XA383", "BMV-712 Smart Rev2"},
    {"0XA389", "SmartShunt 500A/50mV"},
    {"0XA38A", "SmartShunt 1000A/50mV"},
    {"0XA38B", "SmartShunt 2000A/50mV"},
    {"0XA3F0", "Smart BuckBoost 12V/12V-50A"},
    {"0XC030", "SmartShunt 500A/50mV IP65"},

    // data for the Ve Simulator
    {"0X9999", "The migthy simluator"}

}; */