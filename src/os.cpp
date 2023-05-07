#include <stdint.h>
#include "draw.hpp"
#include "string.hpp"
#include "mem.hpp"

extern "C" void CoreStart()
{
    // This is some startup
    while (1)
    {
        // Whatever
    }
}

extern "C" void kmain()
{
    String string = StrFromCStr("Hello world!");
    DrawString(string, 0, 0, WHITE_FG);
}