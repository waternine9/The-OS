#include "pci.h"
#include "io.h"

#define PCI_COMMAND_PORT    0xCF8
#define PCI_DATA_PORT       0xCFC
#define PCI_SPECIALTY(dev, cl, sub, sp) if (dev.Class == cl && dev.Subclass == sub) { return sp; }

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
pci_device_header PCI_QueryDeviceHeader(pci_device_path Path)
{
    pci_device_header Result = { 0 };

    Result.Path = Path;

    Result.VendorId     = PCI_Read16(Path, 0);
    Result.DeviceId     = PCI_Read16(Path, 2);
    Result.Command      = PCI_Read16(Path, 4);
    Result.Status       = PCI_Read16(Path, 6);
    
    uint16_t Interface_RevisionId_Word = PCI_Read16(Path, 8);
    Result.Interface    = (Interface_RevisionId_Word >> 8) & 0xFF;
    Result.RevisionId   = (Interface_RevisionId_Word) & 0xFF;
    
    uint16_t Class_Subclass_Word = PCI_Read16(Path, 10);
    Result.Class        = (Class_Subclass_Word >> 8) & 0xFF;
    Result.Subclass     = (Class_Subclass_Word) & 0xFF;
    
    uint16_t LatencyTimer_CacheLineSize_Word = PCI_Read16(Path, 12);
    Result.LatencyTimer = (LatencyTimer_CacheLineSize_Word >> 8) & 0xFF;
    Result.CacheLineSize= (LatencyTimer_CacheLineSize_Word) & 0xFF;

    uint16_t BIST_HeaderType_Word = PCI_Read16(Path, 14);
    Result.BIST         = (BIST_HeaderType_Word >> 8) & 0xFF;
    Result.HeaderType   = (BIST_HeaderType_Word) & 0x7F;

    Result.MultiFunction= ((BIST_HeaderType_Word) & 0x80) != 0;

    return Result;
}
pci_device_specialty PCI_QueryDeviceSpecialty(pci_device_header Header)
{
    PCI_SPECIALTY(Header, 0x03, 0x00, PCI_DEVICE_VGA);
    PCI_SPECIALTY(Header, 0x02, 0x00, PCI_DEVICE_ETHERNET);
    PCI_SPECIALTY(Header, 0x01, 0x01, PCI_DEVICE_IDE);
    PCI_SPECIALTY(Header, 0x06, 0x00, PCI_DEVICE_HOST_BRIDGE);
    PCI_SPECIALTY(Header, 0x06, 0x01, PCI_DEVICE_ISA_BRIDGE);
    
    return PCI_DEVICE_UNKNOWN;
}