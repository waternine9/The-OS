#include <stdint.h>
#include <stdbool.h>
#include "mem.h"

typedef struct
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) AcpiSdtHeader;

typedef struct
{
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    AcpiSdtHeader *Rsdt;
} __attribute__((packed)) RSDPDescriptor;

extern RSDPDescriptor *rsdp;

bool CompareSignature(const char *a, const char *b)
{
    for (int i = 0; i < 4; i++)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

uint8_t *Madt;
uint32_t bspid;

void FindMadt()
{

    uint32_t NumEntries = (rsdp->Rsdt->length - 36) / 4;

    uint32_t *TablePtrs = (uint32_t *)((uintptr_t)rsdp->Rsdt + 36);
    for (uint32_t i = 0; i < NumEntries; i++)
    {
        AcpiSdtHeader *table_header = (AcpiSdtHeader *)TablePtrs[i];

        if (CompareSignature(table_header->signature, "APIC"))
        {

            Madt = (uint32_t *)TablePtrs[i];
            return;
        }
    }
    asm volatile ("mov %%eax, 1\ncpuid\n": "=b"(bspid));
    bspid >>= 24;
    return; // MADT not found
}

volatile void Timeout()
{
    for (int i = 0;i < 1000;i++)
    {
        for (int j = 0x7C00;i < 0x7E00;j++)
        {
            *(volatile uint8_t*)j = i;
        }
    }
}

void InitCore(uint8_t id)
{                                                                           // clear APIC errors
	*((volatile uint32_t*)(0xFEE00000 + 0x310)) = (id << 24);         // select AP
    *((volatile uint32_t*)(0xFEE00000 + 0x300)) = 0x4500;          // trigger INIT IPI
    while (*((volatile uint32_t*)(0xFEE00000 + 0x300)) & (1 << 12));
    
	for(int j = 0; j < 1; j++) {
		*((volatile uint32_t*)(0xFEE00000 + 0x310)) = (id << 24); // select AP
		*((volatile uint32_t*)(0xFEE00000 + 0x300)) = 0x4608;  // trigger STARTUP IPI for F000:0000
        while (*((volatile uint32_t*)(0xFEE00000 + 0x300)) & (1 << 12));
                                                       // wait 200 usec
	}                                                                                                                 // wait 10 msec
}


 
void WriteReg(uint32_t Reg, uint32_t Val)
{
    *(uint32_t*)(0xFEE00000 + Reg) = Val;
}

uint32_t ReadReg(uint32_t Reg)
{
    return *(uint32_t*)(0xFEE00000 + Reg);
}

void EnableApic() 
{
    WriteReg(0xF0, ReadReg(0xF0) | 0x100);
}

uint8_t LocalApicIDs[255] = {0};
uint8_t ProcessorCount = 0;
volatile uint8_t bspdone;

uint8_t NumProcessors = 0;

void InitCores()
{
    EnableApic();
    FindMadt();
    
    uint32_t MadtEnd = Madt + 0x2C + *(uint32_t *)(Madt + 4);
    for (uint8_t *ptr = Madt + 0x2C; ptr < MadtEnd;ptr += ptr[1])
    {
        switch (*ptr)
        {
            case 0: if (ptr[4] & 1) LocalApicIDs[ProcessorCount++] = ptr[3]; break;
        }
    }
    bspdone = 0;
    for (int i = 1;i < ProcessorCount;i++)
    {
        if (LocalApicIDs[i] == bspid) continue;
        InitCore(LocalApicIDs[i]);
    }
    bspdone = 1;
    
}

volatile void CoreStart()
{
    // THIS IS THE STARTUP CODE FOR ALL PROCESSORS EXCEPT THE BOOT PROCESSOR
    NumProcessors++;
    while (1)
    {
        // TODO: SKE, ADD SCHEDULER HANDLING HERE.
    };
}