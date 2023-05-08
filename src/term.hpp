#ifndef H_TERM
#define H_TERM

#include <stdint.h>
#include <stddef.h>
#include "string.hpp"
#include "linked_list.hpp"

namespace Terminal
{
    void PushNormal(String str);
    void PushError(String str);
    void PushWarning(String str);
    void AttachCmdStr(String *str);
    void Render();
}

#endif // H_TERM