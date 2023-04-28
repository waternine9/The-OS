#include <stdint.h>
#include <stddef.h>
#include "mem.h"
#include "OS.h"

#define PNT_RES_X 400
#define PNT_RES_Y 600

#define SELECT_NUM 3
typedef struct
{
    uint32_t* ImgBuffer;
    uint8_t PntIsSelecting; // 0 default
    uint8_t PntSelectingNum[SELECT_NUM];
    int8_t PntSelectingNumSize; // 0 default
    uint32_t PntFileSelection; // 0 default
    int PntLastX; // -1 default
    int PntLastY; // -1 default
    uint32_t PntSelectedColor; // 0xFFFFFFFF default
} PntReserve;

extern struct _Resources ResourcesAt;

void PntDestructor(window* Win)
{
    PntReserve* Rsrv = (PntReserve*)Win->Reserved;
    WriteFile((uint8_t*)Rsrv->ImgBuffer, PNT_RES_X * PNT_RES_Y * 4, Rsrv->PntFileSelection);
    free(Rsrv->ImgBuffer, PNT_RES_X * PNT_RES_Y * 4);
}

void PntWinHostProc(window* Win)
{

}

void PntClearWinFramebuffer(window* Win, uint32_t Color)
{
    uint32_t* Framebuff = Win->Framebuffer;
    
    int FramebuffSize = Win->Rect->W * Win->Rect->H;
    
    while (FramebuffSize--)
    {
        *Framebuff++ = Color;
    }
}

uint32_t PntGetDigit(uint8_t x)
{
    if (x < '0' || x > '9') return 0xFFFFFFFF;
    return (uint32_t)(x - '0');
}

uint32_t PntGetFileNum(window* Win)
{
    PntReserve* Rsrv = (PntReserve*)Win->Reserved;
    if (!Rsrv->PntSelectingNumSize) return 0;
    uint32_t FileNum = 0;
    int Multiplier = 1;
    int I = Rsrv->PntSelectingNumSize - 1;
    while (I > -1)
    {
        uint32_t Digit = PntGetDigit(Rsrv->PntSelectingNum[I]);
        if (Digit == 0xFFFFFFFF) return 0;
        FileNum += Digit * Multiplier;
        Multiplier *= 10;
        I--;
    }
    return FileNum;
}
void PntDraw(uint32_t color, window *Win)
{
    PntReserve* Rsrv = (PntReserve*)Win->Reserved;
    memcpy(Win->Framebuffer, Rsrv->ImgBuffer, 4 * PNT_RES_X * PNT_RES_Y);
}

void PntSelectDraw(uint32_t color, window *Win)
{
    PntReserve* Rsrv = (PntReserve*)Win->Reserved;
    PntClearWinFramebuffer(Win, 0xFFFFFFFF);
    int CurX = 0;
    char* StartStr = "GOTO: ";
    do
    {
        DrawFontGlyphOnto(CurX, 0, *StartStr, 2, color, Win->Framebuffer, PNT_RES_X, PNT_RES_Y);
        CurX += 20;
    } while (*StartStr++);
    int I = 0;
    while (I < Rsrv->PntSelectingNumSize)
    {
        DrawFontGlyphOnto(CurX, 0, Rsrv->PntSelectingNum[I], 2, color, Win->Framebuffer, PNT_RES_X, PNT_RES_Y);
        CurX += 20;
        I++;
    }
}

void PntSwitchSelection(window *Win)
{
    PntReserve* Rsrv = (PntReserve*)Win->Reserved;
    if (!Rsrv->PntIsSelecting)
    {
        memset(Rsrv->PntSelectingNum, 0, SELECT_NUM);
        Rsrv->PntSelectingNumSize = 0;
        Rsrv->PntIsSelecting = 1;
    }
    else
    {
        WriteFile((uint8_t*)Rsrv->ImgBuffer, PNT_RES_X * PNT_RES_Y * 4, Rsrv->PntFileSelection);
        
        uint32_t FileNum = PntGetFileNum(Win);
        
        size_t size;
        ReadFile((uint8_t*)Rsrv->ImgBuffer, &size, FileNum);
        
        if (size < 952500)
        {
            memset((uint8_t*)Rsrv->ImgBuffer, 0, PNT_RES_X * PNT_RES_Y * 4);
        }

        Rsrv->PntFileSelection = FileNum;
        Rsrv->PntIsSelecting = 0;
    }
        
}

uint8_t IsDrawing = 0;


void DrawLine(int x1, int y1, int x2, int y2, window *Win)
{
    PntReserve* Rsrv = (PntReserve*)Win->Reserved;
    float StepX = x1;
    float StepY = y1;

    float dx = x2 - x1;
    float dy = y2 - y1;
    dx *= 0.01f;
    dy *= 0.01f;
    int CurStep = 100;
    while (CurStep--)
    {
        Rsrv->ImgBuffer[(int)StepX + (int)StepY * PNT_RES_X] = Rsrv->PntSelectedColor;
        StepX += dx;
        StepY += dy;
    }
}
extern int MouseX;
extern int MouseY;

void PntProc(window* Win)
{
    PntReserve* Rsrv = (PntReserve*)Win->Reserved;
    if (IsDrawing)
    {
        int RelX = MouseX - Win->Rect->X;
        int RelY = MouseY - Win->Rect->Y;

        if (RelX >= 0 && RelY >= 0 && RelX < PNT_RES_X && RelY < PNT_RES_Y)
        {

            if (Rsrv->PntLastX != -1)
            {
                DrawLine(Rsrv->PntLastX, Rsrv->PntLastY, RelX, RelY, Win);
            }
            Rsrv->PntLastX = RelX;
            Rsrv->PntLastY = RelY;
        }
    }
    else
    {
        Rsrv->PntLastX = -1;
        Rsrv->PntLastY = -1;
    }
    if (*Win->Events)
    {
        if (*Win->Events & 1)
        {
            *Win->Events &= ~1;
            if (IsDrawing) IsDrawing = 0;
            else IsDrawing = 1;
        }
    }
    
    if (!Rsrv->PntIsSelecting)
    {
        int I = 0;
        while (I < Win->ChQueueNum)
        {
            uint16_t packet = Win->InCharacterQueue[I];
            uint8_t C = packet & 0xFF;
            if (C)
            {
                if (C == 'd' && packet & (1 << 9))
                {
                    WriteFile((uint8_t*)Rsrv->ImgBuffer, PNT_RES_X * PNT_RES_Y * 4, Rsrv->PntFileSelection);
                    DestroyWindow(Win);
                    return;
                }
                if (C == 'm' && packet & (1 << 8))
                {
                    
                    HideWindow(Win);
                }
                if (C == 'g' && packet & (1 << 8))
                {

                    PntSwitchSelection(Win);
                }
                if (C == 's' && packet & (1 << 8))
                {

                    WriteFile((uint8_t*)Rsrv->ImgBuffer, PNT_RES_X * PNT_RES_Y * 4, Rsrv->PntFileSelection);
                    
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
        PntDraw(0, Win);
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
                    PntSwitchSelection(Win);
                }
                else
                {
                    Rsrv->PntSelectingNum[Rsrv->PntSelectingNumSize] = C;   
                    Rsrv->PntSelectingNumSize++;
                    if (Rsrv->PntSelectingNumSize > SELECT_NUM)
                    {
                        Rsrv->PntSelectingNumSize--;
                    }
                }
                
            }
            else
            {
                uint16_t IsBackspace = packet & (1 << 8);
                if (IsBackspace) Rsrv->PntSelectingNumSize--;
                if (Rsrv->PntSelectingNumSize < 0)
                {
                    Rsrv->PntSelectingNumSize = 0;
                }
            }
        
            
            I++;
        }
        Win->ChQueueNum = 0;
        PntSelectDraw(0, Win);
    }
}


void PntCreateWindow(int x, int y)
{
    rect *Rect = (rect*)malloc(sizeof(rect));
    Rect->X = x;
    Rect->Y = y;
    Rect->W = PNT_RES_X;
    Rect->H = PNT_RES_Y;
    PntReserve* Rsrv = (PntReserve*)malloc(sizeof(PntReserve));
    Rsrv->ImgBuffer = malloc(PNT_RES_X * PNT_RES_Y * 4);
    size_t junk;
    if (!ReadFileSize(&junk, 0))
    {
        CreateFile(0);
    }
    ReadFile((uint8_t*)Rsrv->ImgBuffer, &junk, 0);
    Rsrv->PntFileSelection = 0;
    Rsrv->PntIsSelecting = 0;
    Rsrv->PntSelectedColor = 0xFFFFFFFF;
    Rsrv->PntLastX = -1;
    Rsrv->PntLastY = -1;
    memset(Rsrv->PntSelectingNum, 0, 3);
    Rsrv->PntSelectingNumSize = 0;

    CreateWindow(Rect, &PntProc, &PntWinHostProc, &PntDestructor, "paint", malloc(4), malloc(PNT_RES_X * PNT_RES_Y * 4), (uint8_t*)Rsrv, sizeof(Rsrv));
    
}