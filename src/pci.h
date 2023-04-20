#ifndef H_TOS_PCI
#define H_TOS_PCI

#include <stdint.h>

typedef struct {
    uint8_t Bus;        // Which bus device is on
    uint8_t Device;     // Which device
    uint8_t Function;   // Which function of the device
} pci_device_path;

typedef enum {
    PCI_GENERAL_DEVICE,
    PCI_TO_PCI_BRIDGE,
    PCI_TO_CARDBUS_BRIDGE
} pci_header_type;

typedef enum {
    PCI_DEVICE_UNKNOWN,
    PCI_DEVICE_VGA,
    PCI_DEVICE_ETHERNET,
    PCI_DEVICE_IDE,
    PCI_DEVICE_HOST_BRIDGE,
    PCI_DEVICE_ISA_BRIDGE,
} pci_device_specialty;

typedef struct {
    pci_device_path  Path;
    uint16_t         VendorId;
    uint16_t         DeviceId;
    uint16_t         Command;
    uint16_t         Status;
    uint8_t          RevisionId;
    uint8_t          Interface;
    uint8_t          Subclass;
    uint8_t          Class;
    uint8_t          CacheLineSize;
    uint8_t          LatencyTimer;
    pci_header_type  HeaderType;
    uint8_t          BIST;      
    uint32_t         BAR0;
    uint8_t          MultiFunction;
} pci_device_header;

uint32_t PCI_PathToObject(pci_device_path Path, uint8_t RegOffset);
uint16_t PCI_Read16(pci_device_path Path, uint8_t RegOffset);
void PCI_Write32(pci_device_path Path, uint8_t RegOffset, uint32_t Value32);
pci_device_header PCI_QueryDeviceHeader(pci_device_path Path);
pci_device_specialty PCI_QueryDeviceSpecialty(pci_device_header Header);


#endif // H_TOS_PCI