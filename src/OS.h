#ifndef H_TOS_OS
#define H_TOS_OS
#include <stdint.h>
#include <stddef.h>

#define NUM_ICONS 3

typedef struct
{
    int X, Y, W, H;
} rect;

typedef struct _window
{
    rect *Rect;
    uint32_t *Framebuffer;
    uint8_t *Reserved;
    size_t ReservedSize;
    uint8_t *Name;
    uint32_t *Events;
    uint8_t Free;
    uint8_t Hidden;
    uint16_t InCharacterQueue[256];
    uint16_t ChQueueNum;
    void(*WinProc)(struct _window*);
    void(*WinHostProc)(struct _window*);
    void(*WinDestruc)(struct _window*);
    
} window;

typedef struct _mouse_hovering_anim
{
    
    int ticks;
    int win;
} mouse_hovering_anim;


struct _Resources
{
    uint8_t Font[32 * 32 * (127 - 32)];
    uint32_t Background[1280 * 720];
};


window* CreateWindow(rect* Rectptr, void(*WinProc)(window*), void(*WinHostProc)(window*), void(*WinDestruc)(window*), uint8_t *Name, uint32_t *Events, uint32_t* Framebuffer, uint8_t* Reserved, size_t ReservedSize);

void DrawFontGlyphOnto(int x, int y, char character, int scale, uint32_t color, uint32_t* onto, uint32_t resX, uint32_t resY);
void DrawRect(int X, int Y, int W, int H, uint32_t Color);
void DrawRectOnto(int X, int Y, int W, int H, uint32_t Color, uint32_t* onto, uint32_t resX, uint32_t resY);
void DrawTextOnto(int x, int y, const char *string, uint32_t color, uint32_t* onto, uint32_t resX, uint32_t resY);
void ReadFile(uint8_t*, size_t*, uint32_t);
void WriteFile(uint8_t*, size_t, uint32_t);
uint8_t CreateFile(uint32_t FileNum);
uint8_t ReadFileSize(size_t*, uint32_t);
void HideWindow(window*);
void DestroyWindow(window*);
void OSPrintf(const char *Format, ...);

#endif // H_TOS_OS
