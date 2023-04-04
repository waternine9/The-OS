#include "rtc.h"
#include "io.h"

uint8_t RtcRead(uint8_t reg)
{
    IO_Out8(0x70, reg);
    return IO_In8(0x71);
}

void GetRTC(uint8_t *second, uint8_t *minute, uint8_t *hour, uint8_t *day, uint8_t *month, uint8_t *year)
{
    *second = RtcRead(0x00);
    *minute = RtcRead(0x02);
    *hour = RtcRead(0x04);
    *day = RtcRead(0x07);
    *month = RtcRead(0x08);
    *year = RtcRead(0x09);
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
