#include <stdint.h>
#include "sysfont.h"
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

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
} __attribute__ ((packed)) VesaVbeModeInfo;

extern VesaVbeModeInfo VbeModeInfo;

const uint32_t OUT_RES_X = 620;
const uint32_t OUT_RES_Y = 480;

uint32_t BackBuffer[620 * 480];

void SetPixel(uint32_t x, uint32_t y, uint32_t color)
{
  if (color == 0) return;
  BackBuffer[x + y * OUT_RES_X] = color;
}
void DrawGlyph(int x, int y, uint8_t* glyph, int scale, uint32_t color)
{
  
  for (int i = 0;i < 5 * scale;i++)
  {
    for (int j = 0;j < 5 * scale;j++)
    {
      SetPixel(i + x, j + y, (uint32_t)((glyph[j / scale] >> (4 - i / scale)) & 0b1) * color); 
    }
  }
}
void DrawString(int x, int y, const char* s, int scale, uint32_t color)
{
  int InitX = x;
  for (int i = 0;;i++)
  {
    if (!s[i]) return;
    DrawGlyph(x, y, SysFont_GetGlyph(s[i]), scale, color);
    x += 5 * scale + 1 * scale;
    if (x + 5 * scale > OUT_RES_X)
    {
      x = InitX;
      y += 8 * scale + 2 * scale; 
    }
  }
}
void ClearScreen()
{

  for (int i = 0;i < OUT_RES_Y;i++)
  {
    uint32_t* FramebufferStep = BackBuffer + i * OUT_RES_X;
    uint32_t StepValue = i / 4 * 0x010101;
    for (int j = 0;j < OUT_RES_X;j++)
    {
      FramebufferStep[j] = StepValue;
    }
  }
}
void UpdateScreen()
{

  for (int i = 0;i < OUT_RES_Y;i++)
  {
    uint32_t* FramebufferStep = (uint32_t*)((uint8_t*)VbeModeInfo.framebuffer + i * VbeModeInfo.pitch);
    for (int j = 0;j < OUT_RES_X;j++)
    {
      FramebufferStep[j] = BackBuffer[j + i * OUT_RES_X];
    }
  }
}
void OS_Start()
{
  int Color = 0xFFFFFF;
  int OffsetX = 0;
  while (1)
  {
    ClearScreen();
    DrawString(10 + (OffsetX++), 10, "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG", 4, Color);
    if (OffsetX > 400) OffsetX = 0;
    UpdateScreen();
    
  }
}
