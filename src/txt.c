#include <stdint.h>
#include <stddef.h>
#include "OS.h"

#define TXT_RES_X 400
#define TXT_RES_Y 200

extern window* CreateWindow(rect* Rectptr, void(*WinProc)(int, int, window*), uint8_t* Icon32, uint32_t *Events, uint32_t* Framebuffer);
extern void DestroyWindow(window*);
extern void DrawFontGlyphOnto(int x, int y, char character, int scale, uint32_t color, uint32_t* onto, uint32_t resX, uint32_t resY);
extern void ReadFile(uint8_t*, size_t*, uint32_t);
extern void WriteFile(uint8_t*, size_t, uint32_t);


uint8_t TextBuffer[512 * 16];

int32_t TextSize = 0;

uint32_t TextViewOffset = 0;

uint8_t FileSelection = 0;

uint32_t SelectX = 0;
uint32_t SelectY = 0;
uint8_t* SelectPtr;

uint8_t IsOpened = 0;

uint32_t TxtFramebuff[TXT_RES_X * TXT_RES_Y];
uint32_t TxtEvents = 0;

#define SELECT_NUM 3
uint8_t IsSelecting = 0;
uint8_t SelectingNum[SELECT_NUM];
int8_t SelectingNumSize = 0;

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

uint8_t SaveCounter = 0;
void TxtInit()
{
    if (FileSelection > 0) WriteFile(TextBuffer, 512, FileSelection);
    FileSelection = 0;
    IsSelecting = 0;
    size_t junk;
    ReadFile(TextBuffer, &junk, FileSelection);
    int I = 0;

    while (TextBuffer[I] > 0)

    {
        I++;
    }

    TextSize = I;

}

uint32_t GetDigit(uint8_t x)
{
    if (x < '0' || x > '9') return 0xFFFFFFFF;
    return (uint32_t)(x - '0');
}

uint32_t GetFileNum()
{
    if (!SelectingNumSize) return 4;
    uint32_t FileNum = 0;
    int Multiplier = 1;
    int I = SelectingNumSize - 1;
    while (I > -1)
    {
        uint32_t Digit = GetDigit(SelectingNum[I]);
        if (Digit == 0xFFFFFFFF) return 0;
        FileNum += Digit * Multiplier;
        Multiplier *= 10;
        I--;
    }
    return FileNum;
}

void TxtProc(int MouseX, int MouseY, window* Win)
{

    
    if (TxtEvents)
    {
        if (TxtEvents & 1) // LMB clicked
        {
            TxtEvents &= ~1;
            if (!IsSelecting)
            {
                IsSelecting = 1;
            }
            else
            {
                WriteFile(TextBuffer, TextSize + 512, FileSelection);
                
                
                uint32_t FileNum = GetFileNum();
                
                size_t junk;
                ReadFile(TextBuffer, &junk, FileNum);
                int I = 0;

                while (TextBuffer[I] > 0)

                {
                    I++;
                }

                TextSize = I;
                FileSelection = FileNum;
                IsSelecting = 0;
            }
        }
    }
    CurrentInstance = Win;
    if (!IsSelecting)
    {
        while (Win->ChQueueNum > 0)
        {
            Win->ChQueueNum--;
            uint16_t packet = Win->InCharacterQueue[Win->ChQueueNum];
            uint8_t C = packet & 0xFF;
            if (C)
            {

                TextBuffer[TextSize] = C;   
                TextSize++;
                if (C == '\n')
                {
                    TextViewOffset++;
                }
                
            }
            else
            {
                uint16_t IsBackspace = packet & (1 << 8);
                if (IsBackspace) TextSize--;
                if (TextSize < 0)
                {
                    TextSize = 0;
                }
            }
        }
        TxtDraw(0);
    }
    else
    {
        while (Win->ChQueueNum > 0)
        {
            Win->ChQueueNum--;
            uint16_t packet = Win->InCharacterQueue[Win->ChQueueNum];
            uint8_t C = packet & 0xFF;
            if (C)
            {

                SelectingNum[SelectingNumSize] = C;   
                SelectingNumSize++;
                if (SelectingNumSize > SELECT_NUM)
                {
                    SelectingNumSize--;
                }
                
            }
            else
            {
                uint16_t IsBackspace = packet & (1 << 8);
                if (IsBackspace) SelectingNumSize--;
                if (SelectingNumSize < 0)
                {
                    SelectingNumSize = 0;
                }
            }
        }
        SelectDraw(0);
    }
}

void SelectDraw(uint32_t color)
{
    ClearWinFramebuffer(CurrentInstance, 0xFFFFFFFF);
    int CurX = 0;
    char* StartStr = "GOTO: ";
    do
    {
        DrawFontGlyphOnto(CurX, 0, *StartStr, 2, color, TxtFramebuff, TXT_RES_X, TXT_RES_Y);
        CurX += 20;
    } while (*StartStr++);
    int I = 0;
    while (I < SelectingNumSize)
    {
        DrawFontGlyphOnto(CurX, 0, SelectingNum[I], 2, color, TxtFramebuff, TXT_RES_X, TXT_RES_Y);
        CurX += 20;
        I++;
    }
}
void TxtDraw(uint32_t color)
{
    ClearWinFramebuffer(CurrentInstance, 0xFFFFFFFF);
    int I = 0;
    int CurX = 0;
    int CurY = 0;
    while (I < TextSize)
    {
        char C = TextBuffer[I]; 
        switch (C)
        {
            case '\n':
                CurX = 0;
                CurY += 20;
                break;
            case '\t':
                CurX += 30;
                if (CurX > TXT_RES_X - 20)
                {
                    CurX = 0;
                    CurY += 30;
                }
                break;
            default:
                DrawFontGlyphOnto(CurX, CurY + 20, C, 2, 0, TxtFramebuff, TXT_RES_X, TXT_RES_Y);
                CurX += 14;
                if (CurX > TXT_RES_X - 20)
                {
                    CurX = 0;
                    CurY += 30;
                }
                break;
        }
        I++;
    }
    DrawFontGlyphOnto(CurX, CurY + 20, '_', 2, 0, TxtFramebuff, TXT_RES_X, TXT_RES_Y);
}

void TxtCreateWindow(int x, int y)
{
    if (CurrentInstance)
    {
        DestroyWindow(CurrentInstance);
    }
    TxtRect.X = x;
    TxtRect.Y = y;
    TxtRect.W = TXT_RES_X;
    TxtRect.H = TXT_RES_Y;
    
    CurrentInstance = CreateWindow(&TxtRect, TxtProc, ResourcesAt.Icons + 32 * 32 * 4, &TxtEvents, TxtFramebuff);
    
}