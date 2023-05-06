#include "command.h"
#include <stdbool.h>
#include "mem.h"

bool CommandStrCmp(char* X, size_t XSize, char* Y, size_t YSize)
{
    if (XSize != YSize) return false;
    while (XSize--)
    {
        if (*X != *Y) return false;
        X++;
        Y++;
    }
    return true;
}

uint8_t CommandGetType(char* Str, size_t Count, size_t *StepCount)
{
    char* StrStep = Str;
    while ((StrStep - Str) <= Count && *StrStep != ' ') StrStep++;
    *StepCount = StrStep - Str;
    if (CommandStrCmp(Str, StrStep - Str, "bf", 2)) return COMMAND_BF;
    if (CommandStrCmp(Str, StrStep - Str, "demo", 4)) return COMMAND_DEMO;
    if (CommandStrCmp(Str, StrStep - Str, "files", 5)) return COMMAND_FILEMAN;
    if (CommandStrCmp(Str, StrStep - Str, "ide", 3)) return COMMAND_IDE;
    if (CommandStrCmp(Str, StrStep - Str, "paint", 5)) return COMMAND_PAINT;
    if (CommandStrCmp(Str, StrStep - Str, "settings", 8)) return COMMAND_SETTINGS;
    return COMMAND_NOT_FOUND;
}

size_t CommandGetVal(char* Str, size_t Count, size_t *ValCount)
{
    char* StrStep = Str;
    while ((StrStep - Str) <= Count && *StrStep != ' ') StrStep++;
    *ValCount = StrStep - Str;

    StrStep--;
    
    size_t Multiply = 1;
    size_t Val = 0;

    while (*StrStep != ' ')
    {
        if (*StrStep < '0' || *StrStep > '9') return 0xFFFFFFFF;
        Val += (size_t)(*StrStep - '0') * Multiply;
        Multiply *= 10;
        StrStep--;
    }
    return Val;
}

command CommandGet(char* Str, size_t Count)
{
    command Command;
    size_t Step;
    Command.Type = CommandGetType(Str, Count, &Step);
    if (Command.Type == COMMAND_NOT_FOUND) return Command;
    Str += Step + 1;
    Count -= Step + 1;
    size_t CurVal = CommandGetVal(Str, Count, &Step);
    while (CurVal != 0xFFFFFFFF)
    {
        Str += Step + 1;
        Count -= Step + 1;
        Command.ArgCount++;
        command_arg* NewArgs = malloc(Command.ArgCount * sizeof(command_arg));
        memcpy(NewArgs, Command.Args, Command.ArgCount - 1);
        NewArgs[Command.ArgCount - 1].Val = CurVal;
        Command.Args = NewArgs;
        CurVal = CommandGetVal(Str, Count, &Step);
    }
    return Command;
}