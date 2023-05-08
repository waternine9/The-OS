#include "pic.hpp"
#include "io.hpp"

#define PRIM_COMMAND 0x20
#define PRIM_DATA    0x21
#define SECN_COMMAND 0xA0
#define SECN_DATA    0xA1

#define WORD_INIT    0x10
#define WORD_ENV     0x01
#define WORD_M8086   0x01
#define WORD_EOI     0x20

#define PRIM_IDT     32
#define SECN_IDT     32 + 8

volatile void PIC_Init()
{
    /* Save masks */
    uint8_t Mask1, Mask2;
    Mask1 = IO_In8(PRIM_DATA);
    Mask2 = IO_In8(SECN_DATA);

    /* Start initialization */
    IO_Out8(PRIM_COMMAND, WORD_INIT | WORD_ENV);
    IO_Wait();
    IO_Out8(SECN_COMMAND, WORD_INIT | WORD_ENV);
    IO_Wait();

    /* Assign IDT entries to the IRQ's */
    IO_Out8(PRIM_DATA, PRIM_IDT);
    IO_Wait();
    IO_Out8(SECN_DATA, SECN_IDT);
    IO_Wait();

    /* Connect Secondary PIC to Primary PIC on line 2 */
    IO_Out8(PRIM_DATA, 1 << 2);
    IO_Wait();
    IO_Out8(SECN_DATA, 2);
    IO_Wait();

    /* 8086 Mode */
    IO_Out8(PRIM_DATA, WORD_M8086);
    IO_Wait();
    IO_Out8(SECN_DATA, WORD_M8086);
    IO_Wait();

    /* Restore masks */
    IO_Out8(PRIM_DATA, Mask1);
    IO_Wait();
    IO_Out8(SECN_DATA, Mask2);
    IO_Wait();
}
volatile void PIC_SetMask(uint16_t Mask)
{
    IO_Out8(PRIM_DATA, Mask & 0xFF);
    IO_Wait();
    IO_Out8(SECN_DATA, (Mask >> 8) & 0xFF);
    IO_Wait();
}
volatile uint16_t PIC_GetMask()
{
    uint16_t Lo = IO_In8(PRIM_DATA);
    IO_Wait();
    uint16_t Hi = IO_In8(SECN_DATA);
    IO_Wait();
    return Lo | (Hi << 8);
}
volatile void PIC_EndOfInterrupt(uint8_t Interrupt)
{
    if (Interrupt >= 8) {
        IO_Out8(SECN_COMMAND, WORD_EOI);
    }
    IO_Out8(PRIM_COMMAND, WORD_EOI);
}