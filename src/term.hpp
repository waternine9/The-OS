#ifndef H_TERM
#define H_TERM

#include <stdint.h>
#include <stddef.h>
#include "string.hpp"

class Terminal
{
private:
    
public:
    String CommandInput;
    int PushString(String Str);
    void PopBackString();
    void UpdateString(String Str, int I);
};

#endif // H_TERM