#include "draw.hpp"
#include "mem.hpp"
#include "string.hpp"

uint16_t BackBuf[80 * 25];

namespace Draw
{
    void DrawCharacter(uint8_t c, size_t x, size_t y, uint8_t color)
    {
        if (x >= 80) return;
        if (y >= 25) return;

        BackBuf[x + y * 80] = ((uint16_t)color << 8) | c;
    }

    int DrawString(String s, size_t x, size_t y, uint8_t color)
    {
        if (x >= 80) return 0;
        if (y >= 25) return 0;
        int startX = x;
        int yStride = 0;
        for (int i = 0;i < s.size;i++)
        {
            uint8_t c = s[i];
            switch (c)
            {
                case '\n':
                    yStride++;
                    y++;
                    x = startX;
                    break;
                case '\t':
                    x += 4; // 4 is great.
                    break;
                default:
                    Draw::DrawCharacter(c, x, y, color);
                    x++;
            }
            if (x >= 80)
            {
                x = startX;
                yStride++;
                y++;
            }
            if (y >= 25) break;
        }

        return yStride;
    }
    void Clear(uint8_t with, uint8_t color)
    {

        for (int i = 0;i < 80 * 25;i++)
        {
            BackBuf[i] = ((uint16_t)color << 8) | with;
        }
    }
    void SwapBuffers()
    {
        kmemcpy((void*)0xB8000, BackBuf, 80 * 25 * 2);
    }
}