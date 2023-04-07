#include "../fonts/sysfont.h"
#include "../drivers/ata/ata.h"
#include <stdint.h>

// TODO: CHANGE SNAKE CASE TO PASCAL CASE!

typedef struct {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed)) vesa_vbe_mode_info;

extern vesa_vbe_mode_info VbeModeInfo;

const uint32_t OUT_RES_X = 640;
const uint32_t OUT_RES_Y = 480;

uint32_t BackBuffer[640 * 480];

volatile void SetPixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x < 0)
        return;
    if (y < 0)
        return;
    if (x >= 640)
        return;
    if (y >= 480)
        return;
    if ((color >> 24) == 0)
        return;
    BackBuffer[x + y * OUT_RES_X] = color;
}
void DrawGlyph(int x, int y, char character, int scale, uint32_t color)
{
    const uint8_t *glyph = SysFont[character];
    for (int i = 0; i < 8 * scale; i++)
    {
        for (int j = 0; j < 8 * scale; j++)
        {
            SetPixel(i + x, j + y, ((uint32_t)((glyph[j / scale] >> (i / scale)) & 0b1) * color));
        }
    }
}
/* Returns Y stride */
uint32_t DrawString(int x, int y, const char *s, int scale, uint32_t color)
{
    int InitX = x, InitY = y;
    for (int i = 0; s[i]; i++)
    {
        DrawGlyph(x, y, s[i], scale, color);
        x += 8 * scale;
        if (x + 8 * scale > OUT_RES_X)
        {
            x = InitX;
            y += 8 * scale + 4 * scale;
        }
    }
    y += 8 * scale + 4 * scale;

    return y - InitY;
}
void DrawRect(int X, int Y, int W, int H, uint32_t Color)
{
    int InitX = X;
    int X2 = X + W, Y2 = Y + H;
    for (; Y < Y2; Y++)
    {
        for (; X < X2; X++)
        {
            SetPixel(X, Y, Color);
        }
        X = InitX;
    }
}
void ClearScreen()
{
    for (int i = 0; i < OUT_RES_Y; i++)
    {
        uint32_t *FramebufferStep = BackBuffer + i * OUT_RES_X;
        uint32_t StepValue = 0xFFFFFF - (i / 4 * 0x010102);
        for (int j = 0; j < OUT_RES_X; j++)
        {
            FramebufferStep[j] = StepValue;
        }
    }
}
void UpdateScreen()
{
    uint8_t *Framebuffer = ((uint8_t *)VbeModeInfo.framebuffer);

    for (int i = 0; i < OUT_RES_X * OUT_RES_Y; i++)
    {
        // NOTE: BackBuffer stores ARGB, with little endian its BGRA byte order.
        *Framebuffer++ = BackBuffer[i];
        *Framebuffer++ = BackBuffer[i] >> 8;
        *Framebuffer++ = BackBuffer[i] >> 16;
    }
}
void OS_Start()
{
    
}