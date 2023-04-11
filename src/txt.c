#include <stdint.h>
#include "OS.h"

#define TXT_RES_X 400
#define TXT_RES_Y 200

extern window* CreateWindow(rect* Rectptr, void(*WinProc)(int, int, window*), uint32_t* Icon32, uint32_t *Events, uint32_t* Framebuffer);
extern void DestroyWindow(window*);
extern void DrawFontGlyphOnto(int x, int y, char character, int scale, uint32_t color, uint32_t* onto, uint32_t resX, uint32_t resY);

uint8_t IsOpened = 0;

uint32_t TxtFramebuff[TXT_RES_X * TXT_RES_Y];
uint32_t TxtEvents = 0;

rect TxtRect;

window* CurrentInstance = 0;

extern struct _Resources ResourcesAt;

void ClearWinFramebuffer(window* Win, uint32_t Color)
{
    uint32_t* Framebuff = Win->Framebuffer;
    
    int FramebuffSize = Win->Rect->W * Win->Rect->H;
    
    while (FramebuffSize--)
    {
        *Framebuff++ = Color;
    }
}

int CurX = 0;
int CurY = 0;

void TxtProc(int MouseX, int MouseY, window* Win)
{
    
    CurrentInstance = Win;
    while (Win->ChQueueNum > 0)
    {
        Win->ChQueueNum--;
        uint16_t packet = Win->InCharacterQueue[Win->ChQueueNum];
        uint8_t C = packet & 0xFF;
        if (C)
        {
            switch (C)
            {
                case '\n':
                    CurX = 0;
                    CurY += 30;
                    break;
                case '\t':
                    DestroyWindow(Win);
                    CurrentInstance = 0;
                    break;
                default:
                    DrawFontGlyphOnto(CurX, TXT_RES_Y - CurY - 20, C, 2, 0, TxtFramebuff, TXT_RES_X, TXT_RES_Y);
                    CurX += 20;
                    break;
            }
            if (CurX > TXT_RES_X - 20)
            {

                CurX = 0;
                CurY += 30;
            }
            if (CurY > TXT_RES_Y - 30)
            {
                ClearWinFramebuffer(Win, 0xFFFFFFFF);
                CurX = 0;
                CurY = 0;

            }
        }
        else
        {
                
            uint16_t IsBackspace = packet & (1 << 8);
            if (IsBackspace) CurX -= 20;
            if (CurX < 0)
            {
                CurX = 0;
            }
        }
    }
}

void TxtCreateWindow(int x, int y)
{
    if (CurrentInstance)
    {
        DestroyWindow(CurrentInstance);
    }
    CurX = 0;
    CurY = 0;
    TxtRect.X = x;
    TxtRect.Y = y;
    TxtRect.W = TXT_RES_X;
    TxtRect.H = TXT_RES_Y;
    
    CurrentInstance = CreateWindow(&TxtRect, TxtProc, ResourcesAt.Icons + 32 * 32 * 4, &TxtEvents, TxtFramebuff);
    ClearWinFramebuffer(CurrentInstance, 0xFFFFFFFF);
}