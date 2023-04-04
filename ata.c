#include "io.h"
void ATASetPIO()
{
    while (IO_In8(0x1F7) & 0x80); // Wait for the BSY bit to be cleared   
    IO_Out8(0x1F7, 0xEF);
    IO_Out8(0x1F2, 0x0C); // Set PIO mode 4 (0x08 for mode 3, 0x0C for mode 4)
    while (IO_In8(0x1F7) & 0x80); // Wait for the BSY bit to be cleared
}

