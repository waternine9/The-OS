#ifndef H_TOS_COMMAND
#define H_TOS_COMMAND

#include <stdint.h>
#include <stddef.h>

#define COMMAND_IDE 0
#define COMMAND_SETTINGS 1
#define COMMAND_DEMO 2
#define COMMAND_FILEMAN 3
#define COMMAND_BF 4
#define COMMAND_PAINT 5
#define COMMAND_NOT_FOUND 6

typedef struct
{
    size_t Val;
} command_arg;

typedef struct
{
    uint8_t Type;
    command_arg* Args;
    size_t ArgCount;
} command;

command CommandGet(char* Str, size_t Count);

#endif // H_TOS_COMMAND