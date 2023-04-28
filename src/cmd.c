#include <stdint.h>
#include "fonts/sysfont.h"
#include "cmd.h"
#include "OS.h"
#include "paint.h"
#include "mem.h"
#include "fileman.h"
#include "settings.h"
#include "ide.h"
#include "mutex.h"

typedef struct 
{
    uint8_t *TxtBuff;
    uint32_t *BackBuff;
    size_t TxtBuffSize;
    int Blinker;
    _Atomic uint8_t InitFileman;
    _Atomic uint8_t InitPnt;
    _Atomic uint8_t InitIde;
    _Atomic uint8_t InitSettings;
} cmd_reserve;

extern uint16_t VESA_RES_X;
extern uint16_t VESA_RES_Y;

void CmdDestructor(window* Win)
{
    cmd_reserve Rsrv = *(cmd_reserve*)Win->Reserved;
    free((uint32_t*)Rsrv.TxtBuff, 40000);
    free((uint32_t*)Rsrv.BackBuff, CONSOLE_RES_X * CONSOLE_RES_Y * 4);
}

void CmdAddChar(uint8_t thechar, window* Win)
{
    cmd_reserve *Rsrv = (cmd_reserve*)Win->Reserved;
    if (thechar == 0) return;
    Rsrv->TxtBuff[Rsrv->TxtBuffSize] = thechar;
    Rsrv->TxtBuffSize++;
    Rsrv->Blinker = 0;
}

void CmdBackspace(window* Win)
{
    cmd_reserve *Rsrv = (cmd_reserve*)Win->Reserved;
    if (Rsrv->TxtBuff[Rsrv->TxtBuffSize - 1] == '\n' || Rsrv->TxtBuffSize == 0) return;
    Rsrv->TxtBuffSize--;
    Rsrv->Blinker = 0;
}

void CmdClear(window* Win)
{
    cmd_reserve *Rsrv = (cmd_reserve*)Win->Reserved;
    for (int i = 0;i < CONSOLE_RES_X * CONSOLE_RES_Y;i++)
    {
        Rsrv->BackBuff[i] = 0;
    }
}

void CmdDraw(uint32_t Color, window* Win)
{
    cmd_reserve *Rsrv = (cmd_reserve*)Win->Reserved;
    DrawFontGlyphOnto(0, 20, '>', 2, Color, Rsrv->BackBuff, CONSOLE_RES_X, CONSOLE_RES_Y);

    int CurX = 20;
    int CurY = 20;

    char* StepPtr = Rsrv->TxtBuff;
    int StepCount = 0;
    while (StepCount < Rsrv->TxtBuffSize)
    {
        StepCount++;
        switch (*StepPtr)
        {
            case '\n':
                CurX = 10;
                CurY += 25;
                DrawFontGlyphOnto(CurX - 10, CurY, '>', 2, Color, Rsrv->BackBuff, CONSOLE_RES_X, CONSOLE_RES_Y);
                CurX += 10;
                if (CurY > (CONSOLE_RES_Y - 20))
                {
                    while (1)
                    {
                        char C = *Rsrv->TxtBuff++;
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
                DrawFontGlyphOnto(CurX, CurY, *StepPtr, 2, Color, Rsrv->BackBuff, CONSOLE_RES_X, CONSOLE_RES_Y);
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
                        char C = *Rsrv->TxtBuff++;
                        if (C == '\n') break;
                        if (C == 0) break;
                    }
                }
                break;
        }   
        StepPtr++;
    }
    Rsrv->Blinker++;
    if (Rsrv->Blinker > 40) Rsrv->Blinker = 0;
    if (Rsrv->Blinker < 20)
    {
        DrawFontGlyphOnto(CurX, CurY, '_', 2, Color, Rsrv->BackBuff, CONSOLE_RES_X, CONSOLE_RES_Y);
    }
}

void CmdWinHostProc(window *Win)
{
    cmd_reserve* Rsrv = (cmd_reserve*)Win->Reserved;
    if (Rsrv->InitFileman)
    {
        Rsrv->InitFileman = 0;
        FileManCreateWindow(100, 100);
    }
    if (Rsrv->InitPnt)
    {
        Rsrv->InitPnt = 0;
        PntCreateWindow(100, 100);
    }
    if (Rsrv->InitIde)
    {
        Rsrv->InitIde = 0;
        IdeCreateWindow(100, 100);
    }
    if (Rsrv->InitSettings)
    {
        Rsrv->InitSettings = 0;
        SettingsCreateWindow(100, 100);
    }
}

extern int MouseX;
extern int MouseY;

void CmdProc(window* Win)
{
    cmd_reserve* Rsrv = (cmd_reserve*)Win->Reserved;
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
                if (Rsrv->TxtBuff[Rsrv->TxtBuffSize - 2] == 'd')
                {
                    Rsrv->InitPnt = 1;
                }
                if (Rsrv->TxtBuff[Rsrv->TxtBuffSize - 2] == 'i')
                {
                    Rsrv->InitIde = 1;
                }
                if (Rsrv->TxtBuff[Rsrv->TxtBuffSize - 2] == 'f')
                {
                    Rsrv->InitFileman = 1;
                }
                if (Rsrv->TxtBuff[Rsrv->TxtBuffSize - 2] == 's')
                {
                    Rsrv->InitSettings = 1;
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

    memcpy(Win->Framebuffer, Rsrv->BackBuff, CONSOLE_RES_X * CONSOLE_RES_Y * 4);
}

void CmdCreateWindow(int X, int Y)
{
    rect* Rect = (rect*)malloc(sizeof(rect));
    Rect->X = X;
    Rect->Y = Y;
    Rect->W = CONSOLE_RES_X;
    Rect->H = CONSOLE_RES_Y;
    cmd_reserve* Rsrv = (cmd_reserve*)malloc(sizeof(cmd_reserve));
    memset(Rsrv, 0, sizeof(cmd_reserve));
    Rsrv->TxtBuff = (uint8_t*)malloc(40000);
    memset(Rsrv->TxtBuff, 0, 40000);
    Rsrv->BackBuff = malloc(CONSOLE_RES_X * CONSOLE_RES_Y * 4);
    CreateWindow(Rect, &CmdProc, &CmdWinHostProc, &CmdDestructor, "cmd", malloc(4), malloc(CONSOLE_RES_X * CONSOLE_RES_Y * 4), (uint8_t*)Rsrv, sizeof(cmd_reserve));
}