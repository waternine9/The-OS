#ifndef H_PIC
#define H_PIC

#include <stdint.h>

volatile void PIC_Init();
volatile void PIC_SetMask(uint16_t Mask);
volatile uint16_t PIC_GetMask();
volatile void PIC_EndOfInterrupt(uint8_t Interrupt);

#endif // H_PIC
