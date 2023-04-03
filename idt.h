#ifndef H_BOS_IDT
#define H_BOS_IDT

typedef struct {
    unsigned short int OffsetLow;
    unsigned short int Selector;
    unsigned char Zero;
    unsigned char TypeAttr;
    unsigned short int OffsetHi;
} __attribute__((packed)) idt_entry;

extern idt_entry IDT[256];

void IDT_Init();

#endif // H_BOS_IDT