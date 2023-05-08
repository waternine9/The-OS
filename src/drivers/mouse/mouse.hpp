#ifndef H_MOUSE
#define H_MOUSE
#include "../../io.hpp"

void MouseWait(uint8_t AType);
void MouseWrite(uint8_t AWrite);
uint8_t MouseRead();
void MouseInstall();

typedef struct
{
    int size;
    int x, y;
} click_animation;

#endif // H_MOUSE