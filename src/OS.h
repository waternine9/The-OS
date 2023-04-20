#ifndef H_BOS_OS
#define H_BOS_OS
#include <stdint.h>

#define NUM_ICONS 3

typedef struct
{
    int X, Y, W, H;
} rect;

typedef struct _window
{
    rect *Rect;
    uint32_t *Framebuffer;
    uint8_t *Name;
    uint32_t *Events;
    uint8_t Free;
    uint8_t Hidden;

    uint16_t InCharacterQueue[256];
    uint16_t ChQueueNum;
    void(*WinProc)(int, int, struct _window*);
} window;

struct _Resources
{
    uint8_t Font[32 * 32 * (127 - 32)];
    uint32_t Background[1920 * 1080];  
};

#endif // H_BOS_OS