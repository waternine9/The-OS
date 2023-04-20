#ifndef H_TOS_IDT
#define H_TOS_IDT

typedef struct {
    unsigned short int OffsetLow;
    unsigned short int Selector;
    unsigned char Zero;
    unsigned char TypeAttr;
    unsigned short int OffsetHi;
} __attribute__((packed)) idt_entry;

extern idt_entry IDT[256];

void IDT_Init();

#endif // H_TOS_IDT