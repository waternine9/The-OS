#include "io.h"

void ATASetPIO()
{
    while (IO_In8(0x1F7) & 0x80); // Wait for the BSY bit to be cleared   
    IO_Out8(0x1F7, 0xEF);
    IO_Out8(0x1F2, 0x0C); // Set PIO mode 4 (0x08 for mode 3, 0x0C for mode 4)
    while (IO_In8(0x1F7) & 0x80); // Wait for the BSY bit to be cleared
}
void ReadATASector(uint32_t amount, void* buffer, uint8_t primaryorslave, uint32_t lba)
{
    while (IO_In8(0x1F7) & 0x80); // Wait for the BSY bit to be cleared   
    IO_Out8(0x1F6, 0xE0 | ((primaryorslave & 1) << 4) | (lba >> 24 & 0x0F));
    IO_Out8(0x1F7, 0x20);
    while (!(IO_In8(0x1F7) & 0x08)); // Wait for the DRQ bit to be set
    int step = 256;
    while (step--)
    {
        *((uint8_t*)buffer + step) = IO_In8(0x1F0);
    }
}