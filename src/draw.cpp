#include "draw.hpp"
#include "mem.hpp"
#include "string.hpp"

void DrawCharacter(uint8_t c, size_t x, size_t y, uint8_t color)
{
    if (x >= 80) return;
    if (y >= 25) return;

    *(volatile uint16_t*)(0xB8000 + ((y * 80) + x) * 2) = ((uint16_t)color << 8) | c;
}

void DrawString(String s, size_t x, size_t y, uint8_t color)
{
    if (x >= 80) return;
    if (y >= 25) return;
    int startX = x;
    for (int i = 0;i < s.size;i++)
    {
        uint8_t c = s[i];
        switch (c)
        {
            case '\n':
                y++;
                x = startX;
                break;
            case '\t':
                x += 4; // 4 is great.
                break;
            default:
                DrawCharacter(c, x, y, color);
                x++;
        }
        if (x >= 80)
        {
            x = startX;
            y++;
        }
        if (y >= 25) return;
    }
}