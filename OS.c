#include <stdint.h>
#include "sysfont.h"
#include "console.h"
#include "kernel.h"
#include "format.h"

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
void DrawGlyph(int x, int y, char character, int scale, uint32_t color)
{
  uint8_t* glyph = SysFont[character];
  for (int i = 0;i < 8 * scale;i++)
  {
    for (int j = 0;j < 8 * scale;j++)
    {
      SetPixel(i + x, j + y, (uint32_t)((glyph[j / scale] >> (i / scale)) & 0b1) * color); 
    }
  }
}
/* Returns Y stride */
uint32_t DrawString(int x, int y, const char* s, int scale, uint32_t color)
{
  int InitX = x, InitY = y;
  for (int i = 0;s[i];i++)
  {
    DrawGlyph(x, y, s[i], scale, color);
    x += 8 * scale + 2 * scale;
    if (x + 8 * scale > OUT_RES_X)
    {
      x = InitX;
      y += 8 * scale + 4 * scale; 
    }
  }
  y += 8 * scale + 4 * scale; 

  return y - InitY;
}
void ClearScreen()
{

  for (int i = 0;i < OUT_RES_Y;i++)
  {
    uint32_t* FramebufferStep = BackBuffer + i * OUT_RES_X;
    uint32_t StepValue = 0xFFFFFF - (i / 4 * 0x010102);
    for (int j = 0;j < OUT_RES_X;j++)
    {
      FramebufferStep[j] = StepValue;
    }
  }
}
void UpdateScreen()
{
  uint8_t* Framebuffer = ((uint8_t*)VbeModeInfo.framebuffer);

  for (int i=0;i < OUT_RES_X*OUT_RES_Y;i++)
  {
    // NOTE: BackBuffer stores ARGB, with little endian its BGRA byte order.
    *Framebuffer++ = BackBuffer[i];
    *Framebuffer++ = BackBuffer[i]>>8;
    *Framebuffer++ = BackBuffer[i]>>16;
  }
}
void DrawConsole(console *Console, int X, int Y, int Color)
{
  for (uint32_t i = 0; i < CONSOLE_MAX_LINES; i++) {
    Y += DrawString(X, Y, Console->Lines[i], 1, Color);
  }
}
console Console;

int KPrintf(const char *Fmt, ...) {
  va_list Args;
  va_start(Args, Fmt);
  char DestStr[256];
  FormatWriteStringVa(DestStr, 256, Fmt, Args);
  ConsoleWrite(&Console, DestStr);
  va_end(Args);
}


void OS_Start()
{
  KPrintf("HELLO WORLD! %s %d\n", "SUBSTRING", 123);

  int Color = 0x000001;
  int OffsetX = 0;
  while (1)
  {
    ClearScreen();
  
    DrawConsole(&Console, 10, 10, Color);

    if (OffsetX > 400) OffsetX = 0;
    UpdateScreen();
    
    char B[2] = { 0 };
    B[0] = ps2tochar(kernel_WaitForKeyNoRepeat());
    ConsoleWrite(&Console, B);
  }
}
