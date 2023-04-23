#include <stdint.h>
#include <stddef.h>
#include "OS.h"
#include "fileman.h"
#include "mem.h"
#include "format.h"

window* CreateWindow(rect* Rectptr, void(*WinProc)(int, int, window*), uint8_t* Name, uint32_t *Events, uint32_t* Framebuffer);
void DestroyWindow(window*);
void DrawFontGlyphOnto(int x, int y, char character, int scale, uint32_t color, uint32_t* onto, uint32_t resX, uint32_t resY);
void ReadFile(uint8_t*, size_t*, uint32_t);
uint8_t ReadFileSize(size_t*, uint32_t);
void WriteFile(uint8_t*, size_t, uint32_t);
void HideWindow(window*);

uint32_t FileManFramebuff[FILEMAN_RES_X * FILEMAN_RES_Y];
uint32_t FileManEvents = 0;

uint32_t FileSizes[256] = { 0 };

rect FileManRect;

window* FileManCurrentInstance = 0;

extern struct _Resources ResourcesAt;

void FileManClearWinFramebuffer(window* Win, uint32_t Color)
{
    memset(FileManFramebuff, 0, FILEMAN_RES_X * FILEMAN_RES_Y * 4);
}

void FileManDrawString(uint32_t x, uint32_t y, uint8_t* string, uint32_t color)
{
    while (*string)
    {
        if (x > FILEMAN_RES_X - 20) return;
        DrawFontGlyphOnto(x, y, *string, 2, color, FileManFramebuff, FILEMAN_RES_X, FILEMAN_RES_Y);
        x += 14;
        string++;
    }
}

void FileManDraw(uint32_t color)
{
    FileManClearWinFramebuffer(FileManCurrentInstance, 0xFF000000);
    
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
            FileManDrawString(10, CurY, FileStr, color);
            FileManDrawString(FILEMAN_RES_X / 2 - 100, CurY, FileSizeStr, color);
            CurY += 30;
        }
    }
}

void FileManProc(int MouseX, int MouseY, window* Win)
{
    while (Win->ChQueueNum > 0)
    {
        Win->ChQueueNum--;
        uint16_t packet = Win->InCharacterQueue[Win->ChQueueNum];
        uint8_t C = packet & 0xFF;
        if (C)
        {
            if (C == 'm' && packet & (1 << 8))
            {
                HideWindow(Win);
            }
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
    FileManDraw(0xFFFFFFFF);
}

void FileManCreateWindow(int x, int y)
{
    if (FileManCurrentInstance)
    {
        DestroyWindow(FileManCurrentInstance);
    }
    FileManRect.X = x;
    FileManRect.Y = y;
    FileManRect.W = FILEMAN_RES_X;
    FileManRect.H = FILEMAN_RES_Y;
    
    FileManCurrentInstance = CreateWindow(&FileManRect, FileManProc, "fileman", &FileManEvents, FileManFramebuff);
    
}