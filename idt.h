#ifndef H_BOS_IDT
#define H_BOS_IDT

typedef struct {
  unsigned short OffsetLow;
  unsigned short Selector;
  unsigned char Zero;
  unsigned char TypeAttr;
  unsigned short OffsetHi;
} idt_entry __attribute__((packed));

idt_entry IDT[256];

void IDT_Init();

#endif // H_BOS_IDT