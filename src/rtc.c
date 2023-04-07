#include "rtc.h"
#include "io.h"

volatile uint8_t RtcRead(uint8_t reg)
{
    IO_Out8(0x70, reg);
    return IO_In8(0x71);
}
static uint8_t BCDToInt(uint8_t BCD)
{
    return ((BCD >> 4) & 0xF) * 10 + (BCD & 0xF);
}  
volatile void GetRTC(uint8_t *second, uint8_t *minute, uint8_t *hour, uint8_t *day, uint8_t *weekday, uint8_t *month, uint8_t *year)
{
    *second     = BCDToInt(RtcRead(0x00));
    *minute     = BCDToInt(RtcRead(0x02));
    *hour       = BCDToInt(RtcRead(0x04));
    *day        = BCDToInt(RtcRead(0x07));
    *weekday    = BCDToInt(RtcRead(0x06));
    *month      = BCDToInt(RtcRead(0x08));
    *year       = BCDToInt(RtcRead(0x09));
}
const char *WeekDayName(uint8_t weekday) {
    switch (weekday) {
        case  1: return "SUN";
        case  2: return "MON";
        case  3: return "TUE";
        case  4: return "WED";
        case  5: return "THU";
        case  6: return "FRI";
        case  7: return "SAT";
        default: return "???";
    }
}
const char *MonthName(uint8_t month) {
    switch (month) {
        case  1: return "JAN";
        case  2: return "FEB";
        case  3: return "MAR";
        case  4: return "APR";
        case  5: return "MAY";
        case  6: return "JUN";
        case  7: return "JUL";
        case  8: return "AUG";
        case  9: return "SEP";
        case 10: return "OCT";
        case 11: return "NOV";
        case 12: return "DEC";
        default: return "???";
    }
}
