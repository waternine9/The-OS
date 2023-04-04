#include "io.h"
#include "format.h"
void ATASetPIO()
{
    while (IO_In8(0x1F7) & 0x80); // Wait for the BSY bit to be cleared   
    IO_Out8(0x1F7, 0xEF);
    IO_Out8(0x1F2, 0x0C); // Set PIO mode 4 (0x08 for mode 3, 0x0C for mode 4)
    while (IO_In8(0x1F7) & 0x80); // Wait for the BSY bit to be cleared
}
void ReadATASector(uint32_t amount, void* buffer, uint8_t primaryorslave, uint32_t lba)
{
    // Wait for the BSY bit to be cleared
    while (IO_In8(0x1F7) & 0x80);

    // Select the drive
    IO_Out8(0x1F6, 0xE0 | ((primaryorslave & 1) << 4) | (lba >> 24 & 0x0F));

    // Small delay
    for (int i = 0; i < 4; i++) {
        IO_In8(0x1F7);
    }

    // Set up the remaining parameters (sector count, LBA low, mid, and high ports)
    IO_Out8(0x1F2, 1);
    IO_Out8(0x1F3, lba & 0xFF);
    IO_Out8(0x1F4, (lba >> 8) & 0xFF);
    IO_Out8(0x1F5, (lba >> 16) & 0xFF);

    // Send the read command
    IO_Out8(0x1F7, 0x20);

    // Wait for BSY to be cleared and DRQ to be set, or ERR to be set
    uint8_t status;
    do {
        status = IO_In8(0x1F7);
    } while ((status & 0x80) && !(status & 0x08) && !(status & 0x01));

    // Check for errors
    if (status & 0x01) {
        // Handle error (e.g., print an error message or return an error code)
        return;
    }

}