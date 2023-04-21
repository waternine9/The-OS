#include <stdint.h>
#include <stddef.h>
#include "mem.h"
#include "OS.h"

#define PNT_RES_X 400
#define PNT_RES_Y 600

window* CreateWindow(rect* Rectptr, void(*WinProc)(int, int, window*), uint8_t* Name, uint32_t *Events, uint32_t* Framebuffer);
void DestroyWindow(window*);
void DrawFontGlyphOnto(int x, int y, char character, int scale, uint32_t color, uint32_t* onto, uint32_t resX, uint32_t resY);
void ReadFile(uint8_t*, size_t*, uint32_t);
void WriteFile(uint8_t*, size_t, uint32_t);


uint32_t ImgBuffer[PNT_RES_X * PNT_RES_Y];

uint8_t PntFileSelection = 0;

uint8_t PntIsOpened = 0;

uint32_t PntFramebuff[PNT_RES_X * PNT_RES_Y];
uint32_t PntEvents = 0;

#define SELECT_NUM 3
uint8_t PntIsSelecting = 0;
uint8_t PntSelectingNum[SELECT_NUM];
int8_t PntSelectingNumSize = 0;

rect PntRect;

window* PntCurrentInstance = 0;

extern struct _Resources ResourcesAt;
void PntClearWinFramebuffer(window* Win, uint32_t Color)
{
    uint32_t* Framebuff = Win->Framebuffer;
    
    int FramebuffSize = Win->Rect->W * Win->Rect->H;
    
    while (FramebuffSize--)
    {
        *Framebuff++ = Color;
    }
}

void PntInit()
{
    if (PntFileSelection > 0) WriteFile((uint8_t*)ImgBuffer, PNT_RES_X * PNT_RES_Y * 4, PntFileSelection);
    PntFileSelection = 0;
    PntIsSelecting = 0;
    size_t junk;
    ReadFile((uint8_t*)ImgBuffer, &junk, PntFileSelection);
}

uint32_t PntGetDigit(uint8_t x)
{
    if (x < '0' || x > '9') return 0xFFFFFFFF;
    return (uint32_t)(x - '0');
}

uint32_t PntGetFileNum()
{
    if (!PntSelectingNumSize) return 0;
    uint32_t FileNum = 0;
    int Multiplier = 1;
    int I = PntSelectingNumSize - 1;
    while (I > -1)
    {
        uint32_t Digit = PntGetDigit(PntSelectingNum[I]);
        if (Digit == 0xFFFFFFFF) return 0;
        FileNum += Digit * Multiplier;
        Multiplier *= 10;
        I--;
    }
    return FileNum;
}
void PntDraw(uint32_t color)
{
    memcpy(PntFramebuff, ImgBuffer, 4 * PNT_RES_X * PNT_RES_Y);
}

void PntSelectDraw(uint32_t color)
{
    PntClearWinFramebuffer(PntCurrentInstance, 0xFFFFFFFF);
    int CurX = 0;
    char* StartStr = "GOTO: ";
    do
    {
        DrawFontGlyphOnto(CurX, 0, *StartStr, 2, color, PntFramebuff, PNT_RES_X, PNT_RES_Y);
        CurX += 20;
    } while (*StartStr++);
    int I = 0;
    while (I < PntSelectingNumSize)
    {
        DrawFontGlyphOnto(CurX, 0, PntSelectingNum[I], 2, color, PntFramebuff, PNT_RES_X, PNT_RES_Y);
        CurX += 20;
        I++;
    }
}

void PntSwitchSelection()
{
    if (!PntIsSelecting)
    {
        memset(PntSelectingNum, 0, SELECT_NUM);
        PntSelectingNumSize = 0;
        PntIsSelecting = 1;
    }
    else
    {
        WriteFile((uint8_t*)ImgBuffer, PNT_RES_X * PNT_RES_Y * 4, PntFileSelection);
        
        uint32_t FileNum = PntGetFileNum();
        
        size_t junk;
        ReadFile((uint8_t*)ImgBuffer, &junk, FileNum);
        int I = 0;

        PntFileSelection = FileNum;
        PntIsSelecting = 0;
    }
        
}

uint8_t IsDrawing = 0;

void PntProc(int MouseX, int MouseY, window* Win)
{
    if (IsDrawing)
    {
        int RelX = MouseX - Win->Rect->X;
        int RelY = MouseY - Win->Rect->Y;

        if (RelX >= 0 && RelY >= 0 && RelX < PNT_RES_X && RelY < PNT_RES_Y)
        {
            ImgBuffer[RelX + RelY * PNT_RES_X] = 0xFFFFFFFF;
        }
    }
    if (PntEvents)
    {
        if (PntEvents & 1)
        {
            PntEvents &= ~1;
            if (IsDrawing) IsDrawing = 0;
            else IsDrawing = 1;
        }
    }
    
    PntCurrentInstance = Win;
    if (!PntIsSelecting)
    {
        int I = 0;
        while (I < Win->ChQueueNum)
        {
            uint16_t packet = Win->InCharacterQueue[I];
            uint8_t C = packet & 0xFF;
            if (C)
            {
                if (C == 'g' && packet & (1 << 8))
                {

                    PntSwitchSelection();
                }
                if (C == 's' && packet & (1 << 8))
                {

                    WriteFile((uint8_t*)ImgBuffer, PNT_RES_X * PNT_RES_Y * 4, PntFileSelection);
                    
                }
            }
            else
            {
                uint16_t IsBackspace = packet & (1 << 8);
                if (IsBackspace);
            }
            I++;
        }
        Win->ChQueueNum = 0;
        PntDraw(0);
    }
    else
    {
        int I = 0;
        while (I < Win->ChQueueNum)
        {
            uint16_t packet = Win->InCharacterQueue[I];
            uint8_t C = packet & 0xFF;
            if (C)
            {
                
                if (C == 'g' && packet & (1 << 8))
                {
                    PntSwitchSelection();
                }
                else
                {
                    PntSelectingNum[PntSelectingNumSize] = C;   
                    PntSelectingNumSize++;
                    if (PntSelectingNumSize > SELECT_NUM)
                    {
                        PntSelectingNumSize--;
                    }
                }
                
            }
            else
            {
                uint16_t IsBackspace = packet & (1 << 8);
                if (IsBackspace) PntSelectingNumSize--;
                if (PntSelectingNumSize < 0)
                {
                    PntSelectingNumSize = 0;
                }
            }
        
            
            I++;
        }
        Win->ChQueueNum = 0;
        PntSelectDraw(0);
    }
}


void PntCreateWindow(int x, int y)
{
    PntInit();
    if (PntCurrentInstance)
    {
        DestroyWindow(PntCurrentInstance);
    }
    PntRect.X = x;
    PntRect.Y = y;
    PntRect.W = PNT_RES_X;
    PntRect.H = PNT_RES_Y;
    
    PntCurrentInstance = CreateWindow(&PntRect, PntProc, "paint", &PntEvents, PntFramebuff);
    
}