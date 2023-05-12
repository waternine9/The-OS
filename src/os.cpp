#include <stdint.h>
#include "draw.hpp"
#include "string.hpp"
#include "mem.hpp"
#include "pic.hpp"
#include "idt.hpp"
#include "drivers/mouse/mouse.hpp"
#include "drivers/keyboard/keyboard.hpp"
#include "term.hpp"
#include "apic.hpp"
#include "bflang/bflang.hpp"

extern int32_t MouseX;
extern int32_t MouseY;

extern int MouseSensitivity;

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

void EvalHandler(LinkedList<Terminal::CommandArg> args)
{
    if (args.size != 1)
    {
        Terminal::PushError(StrFromCStr("eval: Expected 1 string argument"));
    }
    if (args[0].isVal)
    {
        Terminal::PushError(StrFromCStr("eval: Expected 1 string argument"));
    }
    Terminal::PushNormal(StrFormat("Result: %d", BFRunSource(args[0].str)));
}

void SetmsensHandler(LinkedList<Terminal::CommandArg> args)
{
    if (args.size != 1)
    {
        Terminal::PushError(StrFromCStr("setmsens: Expected 1 value argument"));
    }
    if (!args[0].isVal)
    {
        Terminal::PushError(StrFromCStr("setmsens: Expected 1 value argument"));
    }
    MouseSensitivity = args[0].val;
}

extern "C" void kmain()
{
    kmemset((void*)0x1000000 + 0x7C00, 0, 70000);

    Terminal::Init();

    PIC_Init();
    IDT_Init();

    MouseInstall();
    String commandStr = String();
    Terminal::AttachCmdStr(&commandStr);

    Terminal::PushSuccess(StrFromCStr("Welcome to TheOS v0.1.0!"));
    Terminal::PushNormal(StrFromCStr("Starting cores..."));
    Terminal::PushCommand(EchoHandler, StrFromCStr("echo"));
    Terminal::PushCommand(EvalHandler, StrFromCStr("eval"));
    Terminal::PushCommand(EchoHandler, StrFromCStr("help"));
    Terminal::PushCommand(SetmsensHandler, StrFromCStr("setmsens"));
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