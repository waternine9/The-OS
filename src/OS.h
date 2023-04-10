#ifndef H_BOS_OS
#define H_BOS_OS
#include <stdint.h>

#define NUM_ICONS 2

typedef struct
{
    int X, Y, W, H;
} rect;

typedef struct _window
{
    rect *Rect;
    uint32_t *Framebuffer;
    uint32_t *Icon32;
    uint32_t *Events;
    uint8_t Free;
    uint8_t Hidden;

    uint16_t InCharacterQueue[256];
    uint16_t ChQueueNum;
    void(*WinProc)(int, int, struct _window*);
} window;
#endif // H_BOS_OS