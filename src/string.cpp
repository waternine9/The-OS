#include "string.hpp"

String StrFromCStr(const char* str)
{
    String out = String();
    while (*str)
    {
        out.PushBack(*str);
        str++;
    }
    return out;
}