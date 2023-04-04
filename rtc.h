#ifndef H_BOS_RTC
#define H_BOS_RTC
#include <stdint.h>


void GetRTC(uint8_t* second, uint8_t* minute, uint8_t* hour, uint8_t* day, uint8_t* month, uint8_t *year);
const char *MonthName(uint8_t month);

#endif
