#include <stdint.h>
#include <stddef.h>
#include "OS.h"
#include "ide.h"
#include "mem.h"
#include "format.h"

#define TYPE_BF 0
#define TYPE_BEE 1
 
typedef struct
{
    uint32_t* BackBuff;
    uint8_t *Src;
    size_t SrcSize;
    size_t SrcCurSize;
    size_t SrcScroll;
    uint8_t Type;
} ide_reserve;

void IdeDestructor(window* Win)
{
    ide_reserve* Rsrv = (ide_reserve*)Win->Reserved;
    free(Rsrv->BackBuff, IDE_RES_X * IDE_RES_Y * 4);
    free(Rsrv->Src, Rsrv->SrcSize);
}

void IdeWinHostProc(window* Win)
{
    
}

void IdeClear(window* Win)
{
    ide_reserve* Rsrv = (ide_reserve*)Win->Reserved;
    memset(Rsrv->BackBuff, 33, IDE_RES_X * IDE_RES_Y * 4);
}

void IdeDrawSrc(uint32_t x, uint32_t y, uint32_t color, window *Win)
{
    uint32_t firstX = x;

    ide_reserve* Rsrv = (ide_reserve*)Win->Reserved;

    uint8_t *string = Rsrv->Src;

    size_t Newlines = 0;
    size_t PassedChar = 0;
    while (Newlines < Rsrv->SrcScroll)
    {
        if (*string == '\n') Newlines++;

        if (PassedChar == Rsrv->SrcSize - 1) 
        {
            Rsrv->SrcScroll = Newlines;
            break;
        }
        PassedChar++;
        string++;
    }

    while (*string)
    {
        if (x > IDE_RES_X - 16 || *string == '\n')
        {
            x = firstX;
            y += 20;
        }
        if (y > IDE_RES_Y - 16)
        {
            return;
        }
        if (*string != '\n') 
        {
            DrawFontGlyphOnto(x, y, *string, 2, color, Rsrv->BackBuff, IDE_RES_X, IDE_RES_Y);
            x += 16;
        }
        if (*string == '\t')
        {
            x += 16 * 4;
        }
        string++;
    }
    DrawFontGlyphOnto(x, y, '_', 2, color, Rsrv->BackBuff, IDE_RES_X, IDE_RES_Y);
}

void IdeDraw(uint32_t color, window* Win)
{
    IdeClear(Win);
    
    IdeDrawSrc(4, 4, 0xFFFFFFFF, Win);
}

void IdeAddChar(uint8_t c, window* Win)
{
    if (c == 0) return;
    ide_reserve* Rsrv = (ide_reserve*)Win->Reserved;
    if (Rsrv->SrcCurSize == Rsrv->SrcSize - 1)
    {
        uint8_t *NewSrc = malloc(Rsrv->SrcSize + 8);
        memset(NewSrc, 0, Rsrv->SrcSize + 8);
        memcpy(NewSrc, Rsrv->Src, Rsrv->SrcSize);
        free(Rsrv->Src, Rsrv->SrcSize);
        Rsrv->SrcSize += 8;
        Rsrv->Src = NewSrc;   
    }
    Rsrv->Src[Rsrv->SrcCurSize] = c;
    Rsrv->SrcCurSize++;
    Rsrv->Src[Rsrv->SrcCurSize] = 0;
}

void IdeBackspace(window* Win)
{
    ide_reserve* Rsrv = (ide_reserve*)Win->Reserved;
    if (!Rsrv->SrcCurSize) return;
    Rsrv->SrcCurSize--;
    Rsrv->Src[Rsrv->SrcCurSize] = 0;
}

extern int MouseX;
extern int MouseY;

void IdeProc(window* Win)
{
    ide_reserve* Rsrv = (ide_reserve*)Win->Reserved;
    size_t I = 0;
    while (I < Win->ChQueueNum)
    {
        uint16_t packet = Win->InCharacterQueue[I];
        uint8_t C = packet & 0xFF;
        if (C)
        {
            if (C == 'o' && packet & (1 << 8))
            {
                Rsrv->SrcScroll++;
            }
            else if (C == 'p' && packet & (1 << 8))
            {
                if (Rsrv->SrcScroll) Rsrv->SrcScroll--;
            }
            else
            {
                IdeAddChar(C, Win);
            }
            
        }
        else
        {
            uint16_t IsBackspace = packet & (1 << 8);
            if (IsBackspace) IdeBackspace(Win);
        }
        I++;
    }
    Win->ChQueueNum = 0;

    IdeDraw(0xFFFFFFFF, Win);

    memcpy(Win->Framebuffer, Rsrv->BackBuff, IDE_RES_X * IDE_RES_Y * 4);
}

void IdeCreateWindow(int x, int y)
{
    rect* Rect = (rect*)malloc(sizeof(rect));
    Rect->X = x;
    Rect->Y = y;
    Rect->W = IDE_RES_X;
    Rect->H = IDE_RES_Y;
    
    ide_reserve* Rsrv = (ide_reserve*)malloc(sizeof(ide_reserve));
    Rsrv->BackBuff = malloc(IDE_RES_X * IDE_RES_Y * 4);
    Rsrv->Src = malloc(8);
    memset(Rsrv->Src, 0, 8);
    Rsrv->SrcSize = 8;
    Rsrv->SrcCurSize = 0;
    Rsrv->SrcScroll = 0;
    CreateWindow(Rect, &IdeProc, &IdeWinHostProc, &IdeDestructor, "ide", malloc(4), malloc(IDE_RES_X * IDE_RES_Y * 4), (uint8_t*)Rsrv, sizeof(ide_reserve));
    
}