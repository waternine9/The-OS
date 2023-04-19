#include <stdint.h>
#include "fonts/sysfont.h"
#include "cmd.h"
#include "OS.h"
#include "txt.h"

uint8_t CmdTextBufferArray[100000];

uint8_t* CmdTextBuffer = CmdTextBufferArray;
uint8_t* CmdBufferEnd = CmdTextBufferArray;

int CmdBlinker = 0;

extern uint16_t VESA_RES_X;
extern uint16_t VESA_RES_Y;

uint32_t CmdDrawBuffer[1920 * 1080];

extern uint8_t Font[32 * 32 * (127 - 32)];

extern uint8_t Icons[32 * 32 * 4 * NUM_ICONS];

extern window* CreateWindow(rect* Rectptr, void(*WinProc)(int, int, window*), uint32_t* Icon32, uint32_t *Events, uint32_t* Framebuffer);
extern void DrawFontGlyphOnto(int x, int y, char character, int scale, uint32_t color, uint32_t* onto, uint32_t resX, uint32_t resY);

void CmdProc(int MouseX, int MouseY, window* Win)
{
    while (Win->ChQueueNum > 0)
    {
        Win->ChQueueNum--;
        uint16_t packet = Win->InCharacterQueue[Win->ChQueueNum];
        uint8_t C = packet & 0xFF;
        if (C) 
        {
            CmdAddChar(C);
            if (C == '\n')
            {
                TxtCreateWindow(100, 100);
                TxtInit();
            }
        }
        uint16_t IsBackspace = packet & (1 << 8);
        if (IsBackspace) CmdBackspace();
    }

    CmdClear();
    CmdDraw(0xFFFFFFFF);
}

volatile void CmdSetPixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x < 0)
        return;
    if (y < 0)
        return;
    if (x >= CONSOLE_RES_X)
        return;
    if (y >= CONSOLE_RES_Y)
        return;
    if ((color >> 24) == 0)
        return;
    CmdDrawBuffer[x + y * CONSOLE_RES_X] = color;
}


void InitCMD()
{
    *CmdTextBuffer = 0;
}


void CmdAddChar(uint8_t thechar)
{
    if (thechar == 0) return;
    *CmdBufferEnd = thechar;
    CmdBufferEnd++;
    *CmdBufferEnd = 0;
    CmdBlinker = 0;
}

void CmdBackspace()
{
    if (*(CmdBufferEnd - 1) == '\n' || CmdBufferEnd == CmdTextBuffer) return;
    CmdBufferEnd--;

    *CmdBufferEnd = 0;
    if (CmdBufferEnd < CmdTextBuffer) CmdBufferEnd = CmdTextBuffer;
    CmdBlinker = 0;
}

void CmdClear()
{
    for (int i = 0;i < CONSOLE_RES_X * CONSOLE_RES_Y;i++)
    {
        CmdDrawBuffer[i] = 0;
    }
}

void CmdDraw(uint32_t color)
{
    DrawFontGlyphOnto(0, 20, '>', 2, color, CmdDrawBuffer, CONSOLE_RES_X, CONSOLE_RES_Y);

    int CurX = 20;
    int CurY = 20;

    char* StepPtr = CmdTextBuffer;
    while (*StepPtr)
    {
        switch (*StepPtr)
        {
            case '\n':
                CurX = 10;
                CurY += 25;
                DrawFontGlyphOnto(CurX - 10, CurY, '>', 2, color, CmdDrawBuffer, CONSOLE_RES_X, CONSOLE_RES_Y);
                CurX += 10;
                if (CurY > (CONSOLE_RES_Y - 20))
                {
                    while (1)
                    {
                        char C = *CmdTextBuffer;
                        CmdTextBuffer++;
                        if (C == '\n') break;
                    }
                }
                break;
            case '\t':
                CurX += 80;
                if (CurX > (CONSOLE_RES_X - 20))
                {
                    CurX = 20;
                    CurY += 25;
                }
                break;
            default:
                DrawFontGlyphOnto(CurX, CurY, *StepPtr, 2, color, CmdDrawBuffer, CONSOLE_RES_X, CONSOLE_RES_Y);
                CurX += 15;
                if (CurX > (CONSOLE_RES_X - 20))
                {
                    CurX = 10;
                    CurY += 25;
                }
                if (CurY > (CONSOLE_RES_Y - 20))
                {
                    while (1)
                    {
                        char C = *CmdTextBuffer;
                        CmdTextBuffer++;
                        if (C == '\n') break;
                        if (C == 0) break;
                    }
                }
                break;
        }   
        StepPtr++;
    }
    CmdBlinker++;
    if (CmdBlinker > 40) CmdBlinker = 0;
    if (CmdBlinker < 20)
    {
        DrawFontGlyphOnto(CurX, CurY, '_', 2, color, CmdDrawBuffer, CONSOLE_RES_X, CONSOLE_RES_Y);
    }
}