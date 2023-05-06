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
    uint8_t FileNum;
    uint8_t IsGoto;
    uint8_t GotoNum[3];
    uint8_t GotoNumSize;
} ide_reserve;

size_t IdeGetFileNum(char* Buf, size_t BufSize)
{
    size_t Multiply = 1;
    size_t FileNum = 0;

    while (BufSize)
    {
        BufSize--;
        FileNum += (size_t)(Buf[BufSize] - '0') * Multiply;
        Multiply *= 10;
    }
    
    return FileNum;
}

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
    size_t StepSize = 1;
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
        if (*string == '\t')
        {
            x += 16 * 4;
        }
        
        if (*string != '\n') 
        {
            if (StepSize == Rsrv->SrcCurSize) DrawFontGlyphOnto(x + 8, y, '|', 2, color, Rsrv->BackBuff, IDE_RES_X, IDE_RES_Y);
            DrawFontGlyphOnto(x, y, *string, 2, color, Rsrv->BackBuff, IDE_RES_X, IDE_RES_Y);
            x += 16;
        }
        
        string++;
        StepSize++;
        
    }
    if (Rsrv->SrcCurSize > StepSize) Rsrv->SrcCurSize = StepSize;
    if (Rsrv->SrcCurSize < 0) Rsrv->SrcCurSize = 0;
}

void IdeDraw(uint32_t color, window* Win)
{
    IdeClear(Win);
    
    IdeDrawSrc(4, 4, 0xFFFFFFFF, Win);
}

void IdeDrawString(int X, int Y, uint32_t Color, char* String, window* Win)
{
    ide_reserve* Rsrv = (ide_reserve*)Win->Reserved;
    int StartX = X;
    while (*String)
    {
        DrawFontGlyphOnto(X, Y, *String, 2, Color, Rsrv->BackBuff, IDE_RES_X, IDE_RES_Y);
        X += 16;
        if (X > IDE_RES_X - 16 || *String == '\n')
        {
            X = StartX;
            Y += 20;
        }
        if (*String == '\t')
        {
            X += 16 * 4;
        }
        if (Y > IDE_RES_Y - 20) return;
        String++;
    }
}

void IdeGotoDraw(uint32_t Color, window* Win)
{
    ide_reserve* Rsrv = (ide_reserve*)Win->Reserved;
    IdeClear(Win);
    
    char Buf[256];
    memset(Buf, 0, 256);
    FormatWriteString(Buf, 256, "Go to file: %d", IdeGetFileNum(Rsrv->GotoNum, Rsrv->GotoNumSize));

    IdeDrawString(10, 10, Color, Buf, Win);
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
        if (!Rsrv->IsGoto)
        {
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
                else if (C == 'g' && packet & (1 << 8))
                {
                    Rsrv->IsGoto = 1;
                    Rsrv->GotoNumSize = 0;
                    memset(Rsrv->GotoNum, 0, 3);
                }
                else if (C == 's' && packet & (1 << 8))
                {
                    WriteFile(Rsrv->Src, Rsrv->SrcSize, Rsrv->FileNum);
                }
                else
                {
                    IdeAddChar(C, Win);
                }
                
            }
            else
            {
                uint16_t IsBackspace = packet & (1 << 8);
                uint16_t IsRight = packet & (1 << 9);
                uint16_t IsLeft = packet & (1 << 10);
                if (IsBackspace) IdeBackspace(Win);
                if (IsRight) Rsrv->SrcCurSize++;
                if (IsLeft) Rsrv->SrcCurSize--;
            }
        }
        else
        {
            if (C)
            {
                if (C >= '0' && C <= '9')
                {
                    if (Rsrv->GotoNumSize != 3) Rsrv->GotoNum[Rsrv->GotoNumSize++] = C;
                }
                if (C == '\n')
                {
                    size_t FileNum = IdeGetFileNum(Rsrv->GotoNum, Rsrv->GotoNumSize);
                    
                    size_t FileSize;
                    ReadFileSize(&FileSize, FileNum);
                    if (FileSize < 10000)
                    {
                        if (Rsrv->SrcSize != 0)
                        {
                            WriteFile(Rsrv->Src, Rsrv->SrcSize, Rsrv->FileNum);
                            free(Rsrv->Src, Rsrv->SrcSize);
                        }
                        Rsrv->FileNum = FileNum;
                                        
                        Rsrv->Src = malloc(FileSize + 508);
                        Rsrv->SrcCurSize = 0;
                        ReadFile(Rsrv->Src, &FileSize, Rsrv->FileNum);
                        while (Rsrv->Src[Rsrv->SrcCurSize]) Rsrv->SrcCurSize++;
                        Rsrv->SrcSize = FileSize + 508;
                        Rsrv->IsGoto = 0;
                    }
                }
            }
            else
            {
                uint16_t IsBackspace = packet & (1 << 8);
                if (IsBackspace)
                {
                    if (Rsrv->GotoNumSize > 0) Rsrv->GotoNum[Rsrv->GotoNumSize--] = 0;
                }
            }
        }
        I++;
    }
    Win->ChQueueNum = 0;

    if (!Rsrv->IsGoto) IdeDraw(0xFFFFFFFF, Win);
    else IdeGotoDraw(0xFFFFFFFF, Win);

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
    
    Rsrv->SrcCurSize = 0;
    Rsrv->SrcScroll = 0;
    Rsrv->FileNum = 0;
    size_t FirstFileSize;
    ReadFileSize(&FirstFileSize, Rsrv->FileNum);
    if (FirstFileSize > 10000)
    {
        Rsrv->IsGoto = 1;
        Rsrv->SrcSize = 0;
    }
    else
    {
        if (FirstFileSize == 0) CreateFile(Rsrv->FileNum);
        Rsrv->Src = malloc((FirstFileSize - (FirstFileSize % 8)) + 8);
        Rsrv->SrcSize = (FirstFileSize - (FirstFileSize % 8)) + 8;
        ReadFile(Rsrv->Src, &FirstFileSize, Rsrv->FileNum);
        Rsrv->IsGoto = 0;
    }
    memset(Rsrv->GotoNum, 0, 3);
    Rsrv->GotoNumSize = 0;
    CreateWindow(Rect, &IdeProc, &IdeWinHostProc, &IdeDestructor, "ide", malloc(4), malloc(IDE_RES_X * IDE_RES_Y * 4), (uint8_t*)Rsrv, sizeof(ide_reserve));
    
}