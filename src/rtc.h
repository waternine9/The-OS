#ifndef H_TOS_RTC
#define H_TOS_RTC
#include <stdint.h>


void GetRTC(uint8_t* second, uint8_t* minute, uint8_t* hour, uint8_t* day, uint8_t *weekday, uint8_t* month, uint8_t *year);
const char *MonthName(uint8_t month);
const char *WeekDayName(uint8_t weekday);

#endif
