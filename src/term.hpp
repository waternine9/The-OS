#ifndef H_TERM
#define H_TERM

#include <stdint.h>
#include <stddef.h>
#include "string.hpp"
#include "linked_list.hpp"

namespace Terminal
{
    struct CommandArg
    {
        bool isVal;
        int val;
        String str;
    };

    void Init();
    void PushNormal(String str);
    void PushSuccess(String str);
    void PushError(String str);
    void PushWarning(String str);
    void PushCommand(void(*callback)(LinkedList<Terminal::CommandArg>), String name);
    void RunCommand();
    void AttachCmdStr(String *str);
    void Render();
}

#endif // H_TERM