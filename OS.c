#include <stdint.h>
#include "sysfont.h"
#include "kernel.h"
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

const uint32_t OUT_RES_X = 640;
const uint32_t OUT_RES_Y = 480;

uint32_t BackBuffer[640 * 480];

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
  while (1)
  {
    ClearScreen();
    char c = kernel_GetKeyPressed();
    if (c != -1)
    {
      DrawGlyph(10, 10, SysFont_GetGlyph(ps2tochar(c)), 4, Color);
    }
    UpdateScreen();
    
  }
}
