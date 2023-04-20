#include "../../io.h"

extern uint8_t DriveNumber;

volatile void ReadATASector(void* buff, uint32_t lba)
{
    if (DriveNumber < 0x82)
    {
        IO_Out8(0x1F6, 0b11100000 | (lba >> 24) | (DriveNumber << 4)); // Set the drive and head number
        IO_Out8(0x1F2, 1); // Read 1 sector
        IO_Out8(0x1F3, lba & 0xFF); // Send first 8 bits of LBA
        IO_Out8(0x1F4, (lba >> 8) & 0xFF); // Send second 8 bits of LBA
        IO_Out8(0x1F5, (lba >> 16) & 0xFF); // Send third 8 bits of LBA
        IO_Out8(0x1F7, 0x20); // Read with retry command, 0x20 = read with retry, 0x21 = read without retry
        while (!(IO_In8(0x1F7) & 8)); // The buffer requires servicing
        for (int i = 0;i < 256;i++) // Read into the buffer
        {
            *((uint16_t*)buff + i) = IO_In16(0x1F0);
        }
    }
    else
    {
        IO_Out8(0x176, 0b11100000 | (lba >> 24) | (DriveNumber << 4)); // Set the drive and head number
        IO_Out8(0x172, 1); // Read 1 sector
        IO_Out8(0x173, lba & 0xFF); // Send first 8 bits of LBA
        IO_Out8(0x174, (lba >> 8) & 0xFF); // Send second 8 bits of LBA
        IO_Out8(0x175, (lba >> 16) & 0xFF); // Send third 8 bits of LBA
        IO_Out8(0x177, 0x20); // Read with retry command, 0x20 = read with retry, 0x21 = read without retry
        while (!(IO_In8(0x177) & 8)); // The buffer requires servicing
        for (int i = 0;i < 256;i++) // Read into the buffer
        {
            *((uint16_t*)buff + i) = IO_In16(0x170);
        }
    }
}
volatile void WriteATASector(void* buff, uint32_t lba)
{
    if (DriveNumber < 0x82)
    {
        IO_Out8(0x1F6, 0b11100000 | (lba >> 24) | (DriveNumber << 4)); // Set the drive and head number
        IO_Out8(0x1F2, 1); // Read 1 sector
        IO_Out8(0x1F3, lba & 0xFF); // Send first 8 bits of LBA
        IO_Out8(0x1F4, (lba >> 8) & 0xFF); // Send second 8 bits of LBA
        IO_Out8(0x1F5, (lba >> 16) & 0xFF); // Send third 8 bits of LBA
        IO_Out8(0x1F7, 0x30); // Write with retry command, 0x30 = write with retry
        for (int i = 0;i < 256;i++) // Write from buffer into disk
        {
            
            IO_Out16(0x1F0, *((uint16_t*)buff + i));
        }
        while (IO_In8(0x1F7) & 0x80); // Wait for the BSY bit to be cleared
    }
    else
    {
        IO_Out8(0x176, 0b11100000 | (lba >> 24) | (DriveNumber << 4)); // Set the drive and head number
        IO_Out8(0x172, 1); // Read 1 sector
        IO_Out8(0x173, lba & 0xFF); // Send first 8 bits of LBA
        IO_Out8(0x174, (lba >> 8) & 0xFF); // Send second 8 bits of LBA
        IO_Out8(0x175, (lba >> 16) & 0xFF); // Send third 8 bits of LBA
        IO_Out8(0x177, 0x30); // Write with retry command, 0x30 = write with retry
        for (int i = 0;i < 256;i++) // Write from buffer into disk
        {
            
            IO_Out16(0x170, *((uint16_t*)buff + i));
        }
        while (IO_In8(0x177) & 0x80); // Wait for the BSY bit to be cleared
    }
}

