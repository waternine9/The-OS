#include "pci.h"
#include "io.h"

#define PCI_COMMAND_PORT    0xCF8
#define PCI_DATA_PORT       0xCFC


uint32_t PCI_PathToObject(pci_device_path Path, uint8_t RegOffset)
{
    return (uint32_t)
        1 << 31                        | // Enable bit
        ((Path.Bus & 0xFF) << 16       | // Bus number
        ((Path.Device & 0x1F) << 11)   | // Device ID
        ((Path.Function) & 0x07) << 8) | // Device function
        (RegOffset & 0xFC)             ; // Offset in the register
}
uint16_t PCI_Read16(pci_device_path Path, uint8_t RegOffset)
{
    uint32_t Object = PCI_PathToObject(Path, RegOffset);

    IO_Out32(PCI_COMMAND_PORT, Object);
    
    /* Take the 2nd bit of RegOffset to know what word to read */
    uint32_t Shift = ((RegOffset & 2) * 8);

    uint16_t Word = (IO_In32(PCI_DATA_PORT) >> Shift) & 0xFFFF;

    return Word;
}
void PCI_Write32(pci_device_path Path, uint8_t RegOffset, uint32_t Value32)
{
    uint32_t Object = PCI_PathToObject(Path, RegOffset);

    IO_Out32(PCI_COMMAND_PORT, Object);

    IO_Out32(PCI_DATA_PORT, Value32);
}
