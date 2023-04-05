#ifndef H_BOS_MOUSE
#define H_BOS_MOUSE
#include "../../io.h"
#include "../../format.h"

void MouseWait(uint8_t AType);
void MouseWrite(uint8_t AWrite);
uint8_t MouseRead();
void MouseInstall();

typedef struct
{
    int size;
    int x, y;
} click_animation;

#endif // H_BOS_MOUSE
