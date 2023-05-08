#include "idt.hpp"

#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define INTERRUPT_GATE             0x8E

idt_entry IDT[256];

extern "C" int HandlerIRQ0();
extern "C" int HandlerIRQ1();
extern "C" int HandlerIRQ2();
extern "C" int HandlerIRQ3();
extern "C" int HandlerIRQ4();
extern "C" int HandlerIRQ5();
extern "C" int HandlerIRQ6();
extern "C" int HandlerIRQ7();
extern "C" int HandlerIRQ8();
extern "C" int HandlerIRQ9();
extern "C" int HandlerIRQ10();
extern "C" int HandlerIRQ11();
extern "C" int HandlerIRQ12();
extern "C" int HandlerIRQ13();
extern "C" int HandlerIRQ14();
extern "C" int HandlerIRQ15();
extern "C" int LoadIDT(unsigned long *);

void IDT_Init()
{

    int (*Handlers[16])() = {
            HandlerIRQ0,  HandlerIRQ1,  HandlerIRQ2,  HandlerIRQ3,
            HandlerIRQ4,  HandlerIRQ5,  HandlerIRQ6,  HandlerIRQ7,
            HandlerIRQ8,  HandlerIRQ9,  HandlerIRQ10, HandlerIRQ11,
            HandlerIRQ12, HandlerIRQ13, HandlerIRQ14, HandlerIRQ15,
    };

    for (int I = 0; I < 16; I++) {
        unsigned long HandlerAddr = (unsigned long)Handlers[I];

        idt_entry Entry = { 0 };
        Entry.OffsetLow = HandlerAddr & 0xFFFF;
        Entry.Selector = KERNEL_CODE_SEGMENT_OFFSET;
        Entry.TypeAttr = INTERRUPT_GATE;
        Entry.OffsetHi = (HandlerAddr >> 16) & 0xFFFF;

        IDT[I + 32] = Entry;
    }

    unsigned long IDTAddr = (unsigned long)IDT;
    unsigned long IDTDesc[2];
    IDTDesc[0] = (sizeof (idt_entry) * 256) + ((IDTAddr & 0xFFFF) << 16);
    IDTDesc[1] = (IDTAddr >> 16) & 0xFFFF;

    LoadIDT(IDTDesc);
}