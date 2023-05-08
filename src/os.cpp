#include <stdint.h>
#include "draw.hpp"
#include "string.hpp"
#include "mem.hpp"
#include "pic.hpp"
#include "idt.hpp"
#include "drivers/mouse/mouse.hpp"
#include "drivers/keyboard/keyboard.hpp"
#include "term.hpp"
#include "apic.h"

extern int32_t MouseX;
extern int32_t MouseY;

Keyboard::KeyboardKey keys[32];
Keyboard::Keyboard kbd = { 0 };

extern "C" void CoreStart()
{
    Terminal::PushNormal(StrFromCStr("This core has started"));

    // This is some startup
    while (1)
    {

    }
}

extern "C" void kmain()
{
    PIC_Init();
    IDT_Init();

    MouseInstall();

    Terminal::PushNormal(StrFromCStr("Starting cores..."));

    InitCores();

    String commandStr = String();
    Terminal::AttachCmdStr(&commandStr);



    while (1)
    {
        uint32_t keysCount;
        Keyboard::CollectEvents(&kbd, keys, 32, &keysCount);
        for (int I = 0; I < keysCount; I++)
        {
            if (!keys[I].Released)
            {
                if (keys[I].ASCII)
                {
                    commandStr.PushBack(keys[I].ASCII);
                }
                if (keys[I].Scancode == KEY_BACKSPACE)
                {
                    commandStr.PopBack();
                }
            }
        }
        Terminal::Render();
    }
}