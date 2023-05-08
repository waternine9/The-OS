#include "io.hpp"

uint8_t IO_In8(uint16_t Port)
{
    uint8_t Result = 0;
    asm volatile ("inb %1, %0" : "=a" (Result) : "Nd" (Port));
    return Result;
}
uint16_t IO_In16(uint16_t Port)
{
    uint16_t Result = 0;
    asm volatile ("inw %1, %0" : "=a" (Result) : "Nd" (Port));
    return Result;
}
uint32_t IO_In32(uint16_t Port)
{
    uint32_t Result = 0;
    asm volatile ("inl %1, %0" : "=a" (Result) : "Nd" (Port));
    return Result;
}

void IO_Out8 (uint16_t Port, uint8_t Value)
{
    asm volatile ("outb %0, %1" :: "a" (Value), "Nd" (Port));
}
void IO_Out16(uint16_t Port, uint16_t Value)
{
    asm volatile ("outw %0, %1" :: "a" (Value), "Nd" (Port));
}
void IO_Out32(uint16_t Port, uint32_t Value)
{
    asm volatile ("outl %0, %1" :: "a" (Value), "Nd" (Port));
}

void IO_Wait()
{
    /* Output into an unused port */
    IO_Out8(0x80, 0);
}