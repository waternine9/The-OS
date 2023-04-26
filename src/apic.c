#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "mem.h"
#include "scheduler.h"

typedef struct
{
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__((packed)) acpi_sdt_header;

typedef struct
{
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    acpi_sdt_header *Rsdt;
} __attribute__((packed)) rsdp_descriptor;

extern rsdp_descriptor rsdp;

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
    asm volatile("cli\nhlt" :: "a"(&rsdp));
    uint32_t NumEntries = (rsdp.Rsdt->Length - 36) / 4;

    uint32_t *TablePtrs = (uint32_t *)((uintptr_t)rsdp.Rsdt + 36);
    for (uint32_t i = 0; i < NumEntries; i++)
    {
        acpi_sdt_header *TableHeader = (acpi_sdt_header *)(size_t)TablePtrs[i];
        if (CompareSignature(TableHeader->Signature, "APIC"))
        {
            Madt = (uint8_t *)(size_t)TablePtrs[i];
            return;
        }
    }

    asm volatile ("mov %%eax, 1\ncpuid\n": "=b"(bspid));
    bspid >>= 24;
    return; // MADT not found
}

void Timeout()
{
    for (int i = 0; i < 40000; i++)
    {
        IO_Wait();
    }
}

void InitCore(uint8_t id)
{                                                                  // clear APIC errors
	*((volatile uint32_t*)(0xFEE00000 + 0x310)) = (id << 24);      // select AP
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
    *(uint32_t*)(size_t)(0xFEE00000 + Reg) = Val;
}

uint32_t ReadReg(uint32_t Reg)
{
    return *(uint32_t*)(size_t)(0xFEE00000 + Reg);
}

void EnableApic() 
{
    WriteReg(0xF0, ReadReg(0xF0) | 0x100);
}

uint8_t LocalApicIDs[255] = {0};
uint8_t ProcessorCount = 0;
volatile uint8_t bspdone;

void InitCores()
{
    EnableApic();
    FindMadt();

    uint8_t *MadtEnd = Madt + 0x2C + *(uint32_t *)(Madt + 4);
    for (uint8_t *Ptr = Madt + 0x2C; Ptr < MadtEnd;Ptr += Ptr[1])
    {
        switch (*Ptr)
        {
            case 0: if (Ptr[4] & 1) LocalApicIDs[ProcessorCount++] = Ptr[3]; break;
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
