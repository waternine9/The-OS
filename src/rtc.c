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
        case  1: return "Sunday";
        case  2: return "Monday";
        case  3: return "Tuesday";
        case  4: return "Wednesday";
        case  5: return "Thursday";
        case  6: return "Friday";
        case  7: return "Saturday";
        default: return "???";
    }
}
const char *MonthName(uint8_t month) {
    switch (month) {
        case  1: return "Jan";
        case  2: return "Feb";
        case  3: return "Mar";
        case  4: return "Apr";
        case  5: return "May";
        case  6: return "Jun";
        case  7: return "Jul";
        case  8: return "Aug";
        case  9: return "Sep";
        case 10: return "Oct";
        case 11: return "Nov";
        case 12: return "Dec";
        default: return "???";
    }
}
