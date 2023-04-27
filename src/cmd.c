#include <stdint.h>
#include "fonts/sysfont.h"
#include "cmd.h"
#include "OS.h"
#include "paint.h"
#include "mem.h"
#include "fileman.h"
#include "settings.h"
#include "mutex.h"

typedef struct 
{
    uint8_t *TxtBuff;
    size_t TxtBuffSize;
    int Blinker;
    uint8_t InitFileman;
    uint8_t InitPnt;
    uint8_t InitSettings;
} CmdReserve;

extern uint16_t VESA_RES_X;
extern uint16_t VESA_RES_Y;

void CmdDestructor(window* Win)
{
    CmdReserve rsrv = *(CmdReserve*)Win->Reserved;
    free((uint32_t*)rsrv.TxtBuff, 40000);
}

void CmdAddChar(uint8_t thechar, window* Win)
{
    CmdReserve *rsrv = (CmdReserve*)Win->Reserved;
    if (thechar == 0) return;
    rsrv->TxtBuff[rsrv->TxtBuffSize] = thechar;
    rsrv->TxtBuffSize++;
    rsrv->Blinker = 0;
}

void CmdBackspace(window* Win)
{
    CmdReserve *rsrv = (CmdReserve*)Win->Reserved;
    if (rsrv->TxtBuff[rsrv->TxtBuffSize - 1] == '\n' || rsrv->TxtBuffSize == 0) return;
    rsrv->TxtBuffSize--;
    rsrv->Blinker = 0;
}

void CmdClear(window* Win)
{
    for (int i = 0;i < CONSOLE_RES_X * CONSOLE_RES_Y;i++)
    {
        Win->Framebuffer[i] = 0;
    }
}

void CmdDraw(uint32_t Color, window* Win)
{
    CmdReserve *rsrv = (CmdReserve*)Win->Reserved;
    DrawFontGlyphOnto(0, 20, '>', 2, Color, Win->Framebuffer, CONSOLE_RES_X, CONSOLE_RES_Y);

    int CurX = 20;
    int CurY = 20;

    char* StepPtr = rsrv->TxtBuff;
    int StepCount = 0;
    while (StepCount < rsrv->TxtBuffSize)
    {
        StepCount++;
        switch (*StepPtr)
        {
            case '\n':
                CurX = 10;
                CurY += 25;
                DrawFontGlyphOnto(CurX - 10, CurY, '>', 2, Color, Win->Framebuffer, CONSOLE_RES_X, CONSOLE_RES_Y);
                CurX += 10;
                if (CurY > (CONSOLE_RES_Y - 20))
                {
                    while (1)
                    {
                        char C = *rsrv->TxtBuff++;
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
                DrawFontGlyphOnto(CurX, CurY, *StepPtr, 2, Color, Win->Framebuffer, CONSOLE_RES_X, CONSOLE_RES_Y);
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
                        char C = *rsrv->TxtBuff++;
                        if (C == '\n') break;
                        if (C == 0) break;
                    }
                }
                break;
        }   
        StepPtr++;
    }
    rsrv->Blinker++;
    if (rsrv->Blinker > 40) rsrv->Blinker = 0;
    if (rsrv->Blinker < 20)
    {
        DrawFontGlyphOnto(CurX, CurY, '_', 2, Color, Win->Framebuffer, CONSOLE_RES_X, CONSOLE_RES_Y);
    }
}

void CmdWinHostProc(window *Win)
{
    CmdReserve* rsrv = (CmdReserve*)Win->Reserved;
    if (rsrv->InitFileman)
    {
        rsrv->InitFileman = 0;
        FileManCreateWindow(100, 100);
    }
    if (rsrv->InitPnt)
    {
        rsrv->InitPnt = 0;
        PntCreateWindow(100, 100);
    }
    if (rsrv->InitSettings)
    {
        rsrv->InitSettings = 0;
        SettingsCreateWindow(100, 100);
    }
}

extern int MouseX;
extern int MouseY;

void CmdProc(window* Win)
{
    CmdReserve* rsrv = (CmdReserve*)Win->Reserved;
    int I = 0;
    while (I < Win->ChQueueNum)
    {
        uint16_t packet = Win->InCharacterQueue[I];
        uint8_t C = packet & 0xFF;
        if (C) 
        {
            CmdAddChar(C, Win);
            if (C == '\n')
            {
                if (rsrv->TxtBuff[rsrv->TxtBuffSize - 1] == 'd')
                {
                    rsrv->InitPnt = 1;
                }
                if (rsrv->TxtBuff[rsrv->TxtBuffSize - 1] == 'f')
                {
                    rsrv->InitFileman = 1;
                }
                if (rsrv->TxtBuff[rsrv->TxtBuffSize - 1] == 's')
                {
                    rsrv->InitSettings = 1;
                }
            }
            
        }
        uint16_t IsBackspace = packet & (1 << 8);
        if (IsBackspace) CmdBackspace(Win);
        I++;
    }
    Win->ChQueueNum = 0;

    CmdClear(Win);
    CmdDraw(0xFFFFFFFF, Win);
}

void CmdCreateWindow(int X, int Y)
{
    rect* Rect = (rect*)malloc(sizeof(rect));
    Rect->X = X;
    Rect->Y = Y;
    Rect->W = CONSOLE_RES_X;
    Rect->H = CONSOLE_RES_Y;
    CmdReserve* rsrv = (CmdReserve*)malloc(sizeof(CmdReserve));
    rsrv->TxtBuff = (uint8_t*)malloc(40000);
    memset(rsrv->TxtBuff, 0, 40000);
    rsrv->TxtBuffSize = 0;
    rsrv->Blinker = 0;
    rsrv->InitFileman = 0;
    rsrv->InitPnt = 1;
    rsrv->InitSettings = 0;
    CreateWindow(Rect, &CmdProc, &CmdWinHostProc, &CmdDestructor, "cmd", malloc(4), malloc(CONSOLE_RES_X * CONSOLE_RES_Y * 4), (uint8_t*)rsrv, sizeof(CmdReserve));
}