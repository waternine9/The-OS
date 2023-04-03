#ifndef H_BOS_PCI
#define H_BOS_PCI

#include <stdint.h>

typedef struct {
    uint8_t Bus;        // Which bus device is on
    uint8_t Device;     // Which device
    uint8_t Function;   // Which function of the device
} pci_device_path;

typedef struct {
    pci_device_path Path;
    uint16_t VendorId;
    uint16_t DeviceId;
    uint16_t Status;
} pci_device_desc;

uint32_t PCI_PathToObject(pci_device_path Path, uint8_t RegOffset);
uint16_t PCI_Read16(pci_device_path Path, uint8_t RegOffset);
void PCI_Write32(pci_device_path Path, uint8_t RegOffset, uint32_t Value32);

#endif // H_BOS_PCI