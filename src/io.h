#ifndef H_TOS_IO
#define H_TOS_IO

#include <stdint.h>

uint8_t  IO_In8 (uint16_t Port);
uint16_t IO_In16(uint16_t Port);
uint32_t IO_In32(uint16_t Port);

void IO_Out8 (uint16_t Port, uint8_t Value);
void IO_Out16(uint16_t Port, uint16_t Value);
void IO_Out32(uint16_t Port, uint32_t Value);

void IO_Wait();

#endif // H_TOS_IO