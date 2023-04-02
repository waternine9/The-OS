#include "idt.h"

#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define INTERRUPT_GATE             0x8E

void IDT_Init()
{
  extern int LoadIDT();
  /* Set up IRQ's */
  extern int HandlerIRQ0(),  int HandlerIRQ1(),  int HandlerIRQ2(),  int HandlerIRQ3();
  extern int HandlerIRQ4(),  int HandlerIRQ5(),  int HandlerIRQ6(),  int HandlerIRQ7();
  extern int HandlerIRQ8(),  int HandlerIRQ9(),  int HandlerIRQ10(), int HandlerIRQ11();
  extern int HandlerIRQ12(), int HandlerIRQ13(), int HandlerIRQ14(), int HandlerIRQ15();

  int (*Handlers)()[16] = {
    HandlerIRQ0,  HandlerIRQ1,  HandlerIRQ2,  HandlerIRQ3,
    HandlerIRQ4,  HandlerIRQ5,  HandlerIRQ6,  HandlerIRQ7,
    HandlerIRQ8,  HandlerIRQ9,  HandlerIRQ10, HandlerIRQ11,
    HandlerIRQ12, HandlerIRQ13, HandlerIRQ14, HandlerIRQ15,
  };

  for (int I = 0; I < 16; I++) {
    unsigned long HandlerAddr = (unsigned long)Handlers[I];

    idt_entry Entry = { 0 };
    Entry.OffsetLow = HandlerAddr & 0xFFFF;
    Entry.Selector  = KERNEL_CODE_SEGMENT_OFFSET;
    Entry.TypeAttr  = INTERRUPT_GATE;
    Entry.OffsetHi  = (HandlerAddr >> 16) & 0xFFFF;

    IDT[I + 32] = Entry;
  }

  unsigned long IDTAddr = (unsigned long)IDT;
  unsigned long IDTDesc[2];
  IDTDesc[0] = (sizeof (idt_entry) * 256 - 1) + ((IDTAddr & 0xFFFF) << 16);
  IDTDesc[1] = IDTAddr >> 16 & 0xFFFF;

  LoadIDT();
}