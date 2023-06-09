#include "pic.h"
#include "kernel.h"
#include "drivers/mouse/mouse.h"
#include "drivers/keyboard/keyboard.h"
#include "mutex.h"

int32_t MouseX, MouseY;
uint8_t MouseRmbClicked, MouseLmbClicked;

uint8_t KeyboardCharPressed = 0xFF;
mutex PS2Mutex;

extern int ConPrintf(const char *Fmt, ...);

extern _Atomic uint32_t SchedulerTick;

/* PIT Timer */
void CHandlerIRQ0()
{
    SchedulerTick++;
    PIC_EndOfInterrupt(0);
}
/* Keyboard Interrupt */
void CHandlerIRQ1()
{
    MutexLock(&PS2Mutex);
    Keyboard_HandleInterrupt();
    MutexRelease(&PS2Mutex);
    PIC_EndOfInterrupt(1);
}
/* Channel for Secondary PIC, don't use. */
void CHandlerIRQ2()
{
    PIC_EndOfInterrupt(2);
}
/* COM2 */
void CHandlerIRQ3()
{
    PIC_EndOfInterrupt(3);
}
/* COM1 */
void CHandlerIRQ4()
{
    PIC_EndOfInterrupt(4);
}
/* LPT2 */
void CHandlerIRQ5()
{
    PIC_EndOfInterrupt(5);
}
/* Floppy Disk */
void CHandlerIRQ6()
{
    PIC_EndOfInterrupt(6);
}
/* LPT1 (spurious) */
void CHandlerIRQ7()
{
    PIC_EndOfInterrupt(7);
}
/* CMOS Real time clock */
void CHandlerIRQ8()
{
    PIC_EndOfInterrupt(8);
}
/* Free for peripherals */
void CHandlerIRQ9()
{
    PIC_EndOfInterrupt(9);
}
/* Free for peripherals */
void CHandlerIRQ10()
{
    PIC_EndOfInterrupt(10);
}
/* Free for peripherals */
void CHandlerIRQ11()
{
    PIC_EndOfInterrupt(11);
}
static uint8_t IsFull() {
    return IO_In8(0x64)&1;
}
/* PS/2 Mouse */
void CHandlerIRQ12()
{
    MutexLock(&PS2Mutex);
    while (IsFull())
    {
        uint8_t Byte0 = IO_In8(0x60);
        int8_t Byte1 = IO_In8(0x60);
        int8_t Byte2 = IO_In8(0x60);
        if ((Byte0 & 1) && Byte1 == 0 && Byte2 == 0) MouseLmbClicked = 1;
        if ((Byte0 & 2) && Byte1 == 0 && Byte2 == 0) MouseRmbClicked = 1;
        MouseX += Byte1;
        MouseY -= Byte2;
    }
    MutexRelease(&PS2Mutex);

    PIC_EndOfInterrupt(12);
}
/* FPU */
void CHandlerIRQ13()
{
    PIC_EndOfInterrupt(13);
}
/* Primary ATA */
void CHandlerIRQ14()
{
    PIC_EndOfInterrupt(14);
}
/* Secondary ATA */
void CHandlerIRQ15()
{
    PIC_EndOfInterrupt(15);
}