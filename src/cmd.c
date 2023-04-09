#include <stdint.h>
#include "fonts/sysfont.h"
#include "cmd.h"
char CmdTextBufferArray[100000];

char* CmdTextBuffer = CmdTextBufferArray;
char* CmdBufferEnd = CmdTextBufferArray;

int CmdBlinker = 0;

extern uint16_t VESA_RES_X;
extern uint16_t VESA_RES_Y;

uint32_t CmdDrawBuffer[1920 * 1080];


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

void CmdDrawGlyph(int x, int y, char character, int scale, uint32_t color)
{
    const uint8_t *glyph = SysFont[character];
    for (int i = 0; i < 8 * scale; i++)
    {
        for (int j = 0; j < 8 * scale; j++)
        {
            CmdSetPixel(i + x, -j + y, ((uint32_t)((glyph[j / scale] >> (i / scale)) & 0b1) * color));
        }
    }
}

void InitCMD()
{
    *CmdTextBuffer = 0;
}


void CmdAddChar(char thechar)
{
    if (thechar == 0) return;
    *CmdBufferEnd = thechar;
    CmdBufferEnd++;
    *CmdBufferEnd = 0;
    CmdBlinker = 0;
}
void CmdBackspace()
{
    if (*(CmdBufferEnd - 1) == '\n') return;
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
    CmdDrawGlyph(10, CONSOLE_RES_Y - 10, '>', 1, color);

    int CurX = 20;
    int CurY = 10;

    char* StepPtr = CmdTextBuffer;
    while (*StepPtr)
    {
        switch (*StepPtr)
        {
            case '\n':
                CurX = 10;
                CurY += 14;
                CmdDrawGlyph(CurX, CONSOLE_RES_Y - CurY, '>', 1, color);
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
                CurX += 40;
                if (CurX > (CONSOLE_RES_X - 20))
                {
                    CurX = 10;
                    CurY += 14;
                }
                break;
            default:
                CmdDrawGlyph(CurX, CONSOLE_RES_Y - CurY, *StepPtr, 1, color);
                CurX += 10;
                if (CurX > (CONSOLE_RES_X - 20))
                {
                    CurX = 10;
                    CurY += 14;
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
        CmdDrawGlyph(CurX, CONSOLE_RES_Y - CurY, '_', 1, color);
    }
}