#include "string.hpp"
#include <cstdarg>

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

bool StrStartsWith(String str, String startswith)
{
    if (startswith.size > str.size) return false;
    for (int i = 0;i < str.size;i++)
    {
        if (str[i] == ' ') return true;
        if (str[i] != startswith[i]) return false;
    }
    return true;
}

String StrFormat(const char* format, ...)
{
    va_list Args;
    va_start(Args, format);
    String out = String();
    while (*format)
    {
        if (*format == '%')
        {
            format++;
            if (*format == 'd')
            {

                int val = va_arg(Args, int);
                bool isSign = val < 0;
                String temp = String();
                while (val != 0)
                {
                    temp.PushBack((isSign ? -(val % 10) : (val % 10)) + '0');
                    val /= 10;
                }
                if (isSign) temp.PushBack('-');
                for (int i = temp.size - 1; i >= 0; i--)
                {
                    out.PushBack(temp[i]);
                }
            }
            if (*format == 'x')
            {
                int val = va_arg(Args, int);
                bool isSign = val < 0;
                String temp = String();
                while (val != 0)
                {
                    temp.PushBack((isSign ? -(val % 16) : (val % 16)) < 10 ? (isSign ? -(val % 16) : (val % 16)) + '0' : ((isSign ? -(val % 16) : (val % 16)) - 10) + 'A');
                    val /= 16;
                }
                temp.PushBack('x');
                temp.PushBack('0');
                if (isSign) temp.PushBack('-');
                for (int i = temp.size - 1; i >= 0; i--)
                {
                    out.PushBack(temp[i]);
                }
            }
        }
        else
        {
            out.PushBack(*format);
        }
        format++;
    }
    va_end(Args);
    return out;
}