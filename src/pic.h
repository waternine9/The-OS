#ifndef H_TOS_PIC
#define H_TOS_PIC

#include <stdint.h>

void PIC_Init();
void PIC_SetMask(uint16_t Mask);
uint16_t PIC_GetMask();
void PIC_EndOfInterrupt(uint8_t Interrupt);

#endif // H_TOS_PIC