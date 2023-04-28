#include "../../io.h"
#include <stdbool.h>
#include <stdint.h>

bool SB16Initialized = false;

void SB16Timeout()
{
    for (int i = 0; i < 10;i++)
    {
        IO_Wait();
    }
}

bool SB16ResetDSP()
{
    IO_Out8(0x226, 1);
    SB16Timeout();
    IO_Out8(0x226, 0);
    return IO_In8(0x22A) == 0xAA;
}

void SB16SetIRQ()
{
    IO_Out8(0x224, 0x80);
    IO_Out8(0x225, 0x08); // IRQ 10
}


void SB16SetBuff(uint32_t Addr, uint16_t Len)
{
    Addr &= 0x00FFFFFF;
    IO_Out8(0xD4, 5); // Disable channel
    IO_Out8(0xD8, 1); // Flip flop
    IO_Out8(0xD6, 0x59); // (0x48 for single mode, 0x58 for auto mode) + channel number
    IO_Out8(0x8B, Addr >> 12); // Send page number of addr
    IO_Out8(0xC4, Addr & 0xFF); // Send low bits of addr
    IO_Out8(0xC4, (Addr >> 8) & 0xFF); // Send middle bits of addr
    IO_Out8(0xC6, Len & 0xFF); // Send low bits of len
    IO_Out8(0xC6, (Len >> 8) & 0xFF); // Send high bits of len
    IO_Out8(0xD4, 1); // Finally, enable channel
}

void SB16Program()
{
    IO_Out8(0x22C, 0x40); // Set time constant
    IO_Out8(0x22C, 255); // 10989 Hz
    IO_Out8(0x22C, 0xB0); // 16-bit sound
    IO_Out8(0x22C, 0b10000); // Mono and signed sound data
    IO_Out8(0x22C, 0xFE); // Count low bits
    IO_Out8(0x22C, 0xFF); // Count high bits
}

bool SB16Init()
{
    if (!SB16ResetDSP())
    {
        SB16Initialized = false;
        return false;
    }

    SB16SetIRQ();

    IO_Out8(0x22C, 0xD1); // Turn speaker on

    SB16Program();
}