#include <stdint.h>
#include "draw.hpp"
#include "string.hpp"
#include "mem.hpp"
#include "pic.hpp"
#include "idt.hpp"
#include "drivers/mouse/mouse.hpp"
#include "drivers/keyboard/keyboard.hpp"
#include "term.hpp"

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

void EchoHandler(LinkedList<Terminal::CommandArg> args)
{
    for (int i = 0;i < args.size;i++)
    {
        if (!args[i].isVal)
        {
            Terminal::PushNormal(args[i].str);
        }
        else
        {
            Terminal::PushNormal(StrFormat("%x", args[i].val));
        }
    }
}

extern "C" void kmain()
{
    kmemset((void*)0x1000000, 0, 70000);

    Terminal::Init();

    *(uint16_t*)0xB8000 = 0x0F49;

    PIC_Init();
    IDT_Init();

    //MouseInstall();
    String commandStr = String();
    Terminal::AttachCmdStr(&commandStr);

    Terminal::PushSuccess(StrFromCStr("Welcome to TheOS v0.1.0!"));
    Terminal::PushNormal(StrFromCStr("Starting cores..."));
    Terminal::PushCommand(EchoHandler, StrFromCStr("echo"));
    Terminal::Render();


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
                    if (keys[I].ASCII == '\n')
                    {
                        Terminal::RunCommand();
                    }
                    else
                    {
                        commandStr.PushBack(keys[I].ASCII);
                    }
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