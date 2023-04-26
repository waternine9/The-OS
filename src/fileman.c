#include <stdint.h>
#include <stddef.h>
#include "OS.h"
#include "fileman.h"
#include "mem.h"
#include "format.h"

uint32_t FileSizes[256] = { 0 };

extern struct _Resources ResourcesAt;

void FileManDestructor()
{
    
}

void FileManClearWinFramebuffer(window* Win, uint32_t Color)
{

    memset(Win->Framebuffer, 0, FILEMAN_RES_X * FILEMAN_RES_Y * 4);
}

void FileManDrawString(uint32_t x, uint32_t y, uint8_t* string, uint32_t color, window *Win)
{
    while (*string)
    {
        if (x > FILEMAN_RES_X - 20) return;
        DrawFontGlyphOnto(x, y, *string, 2, color, Win->Framebuffer, FILEMAN_RES_X, FILEMAN_RES_Y);
        x += 14;
        string++;
    }
}

void FileManDraw(uint32_t color, window* Win)
{
    FileManClearWinFramebuffer(Win, 0xFF000000);
    
    uint32_t CurY = 40;
    for (int i = 0;i < 256;i++)
    {
        if (FileSizes[i])
        {
            uint8_t FileStr[16] = { 0 };
            memset(FileStr, 0, 16);
            FormatWriteString(FileStr, 16, "F %d", i);
            uint8_t FileSizeStr[256] = { 0 };
            memset(FileSizeStr, 0, 256);
            FormatWriteString(FileSizeStr, 256, "SIZE %d B", FileSizes[i]);
            FileManDrawString(10, CurY, FileStr, color, Win);
            FileManDrawString(FILEMAN_RES_X / 2 - 100, CurY, FileSizeStr, color, Win);
            CurY += 30;
        }
    }
}

extern int MouseX;
extern int MouseY;

void FileManProc(window* Win)
{
    FileManClearWinFramebuffer(Win, 0);
    while (Win->ChQueueNum > 0)
    {
        Win->ChQueueNum--;
        uint16_t packet = Win->InCharacterQueue[Win->ChQueueNum];
        uint8_t C = packet & 0xFF;
        if (C)
        {
            if (C == 'r' && packet & (1 << 8))
            {
                for (int i = 0;i < 256;i++)
                {
                    size_t FileSize;
                    if (ReadFileSize(&FileSize, i))
                    {
                        FileSizes[i] = FileSize; 
                    }
                }
            }
        }
        else
        {
            uint16_t IsBackspace = packet & (1 << 8);
            if (IsBackspace)
            {
                // backspace pressed
            }
        }
    }
    FileManDraw(0xFFFFFFFF, Win);
}

void FileManCreateWindow(int x, int y)
{
    rect* Rect = (rect*)malloc(sizeof(rect));
    Rect->X = x;
    Rect->Y = y;
    Rect->W = FILEMAN_RES_X;
    Rect->H = FILEMAN_RES_Y;
    
    CreateWindow(Rect, &FileManProc, &FileManDestructor, "fileman", malloc(4), malloc(FILEMAN_RES_X * FILEMAN_RES_Y * 4), 0, 0);
    
}