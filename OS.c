#include <stdint.h>
#include "sysfont.h"
#include "console.h"
#include "kernel.h"
#include "format.h"
#include "logo.h"
#include "pic.h"
#include "idt.h"
#include "io.h"
#include "rtc.h"
#include "mouse.h"
#include "ata.h"

extern click_animation ClickAnimation;
extern uint8_t MousePointerBlack[8];
extern uint8_t MousePointerFull[8];
extern uint8_t KeyboardCharPressed;

extern int32_t MouseX, MouseY;
extern uint8_t MouseRmbClicked, MouseLmbClicked;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

const uint32_t OUT_RES_X = 640;
const uint32_t OUT_RES_Y = 480;

uint32_t BackBuffer[640 * 480];

void SetPixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x >= OUT_RES_X) return;
    if (y >= OUT_RES_Y) return;
    if (color == 0) return;
    BackBuffer[x + y * OUT_RES_X] = color;
}
void DrawGlyph(int x, int y, char character, int scale, uint32_t color)
{
    const uint8_t* glyph = SysFont[character];
    for (int i = 0; i < 8 * scale; i++)
    {
        for (int j = 0; j < 8 * scale; j++)
        {
            SetPixel(i + x, j + y, (uint32_t)((glyph[j / scale] >> (i / scale)) & 0b1) * color);
        }
    }
}
/* Returns Y stride */
uint32_t DrawString(int x, int y, const char* s, int scale, uint32_t color)
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
void ClearScreen()
{
    for (int i = 0; i < OUT_RES_Y; i++)
    {
        uint32_t* FramebufferStep = BackBuffer + i * OUT_RES_X;
        uint32_t StepValue = 0xFFFFFF - (i / 4 * 0x010102);
        for (int j = 0; j < OUT_RES_X; j++)
        {
            FramebufferStep[j] = StepValue;
        }
    }
}
void UpdateScreen()
{
    uint8_t* Framebuffer = ((uint8_t*)VbeModeInfo.framebuffer);

    for (int i = 0; i < OUT_RES_X*OUT_RES_Y; i++)
    {
        // NOTE: BackBuffer stores ARGB, with little endian its BGRA byte order.
        *Framebuffer++ = BackBuffer[i];
        *Framebuffer++ = BackBuffer[i]>> 8;
        *Framebuffer++ = BackBuffer[i]>> 16;
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

void DrawLogoAt(uint32_t x, uint32_t y, int scale)
{
    for (int Y = 0; Y < 8 * scale; Y++)
    {
        uint8_t BlackRow = BananaLogoBlack[Y / scale];
        uint8_t YellowRow = BananaLogoYellow[Y / scale];
        uint8_t BrownRow = BananaLogoBrown[Y / scale];
        for (int X = 0; X < 8 * scale; X++)
        {
            uint8_t CurBlack = (BlackRow >> (X / scale)) & 0b1;
            uint8_t CurYellow = (YellowRow >> (X / scale)) & 0b1;
            uint8_t CurBrown = (BrownRow >> (X / scale)) & 0b1;
            if (CurBrown)
            {
                SetPixel((8 * scale - X) + x, Y + y, 0x773300);
            }
            else if (CurBlack)
            {
                SetPixel((8 * scale - X) + x, Y + y, 0x000001);
            }
            else if (CurYellow)
            {
                SetPixel((8 * scale - X) + x, Y + y, 0xFFFF00);
            }
        }
    }
}

void DrawPointerAt(uint32_t x, uint32_t y, int scale)
{
    for (int Y = 0; Y < 8 * scale; Y++)
    {
        uint8_t BlackRow = MousePointerBlack[Y / scale];
        uint8_t FullRow = MousePointerFull[Y / scale];
        for (int X = 0; X < 8 * scale; X++)
        {
            uint8_t CurBlack = (BlackRow >> (X / scale)) & 0b1;
            uint8_t CurFull = (FullRow >> (X / scale)) & 0b1;
            if (CurBlack)
            {
                SetPixel((8 * scale - X) + x, Y + y, 0x000001);
            }
            else if (CurFull)
            {
                SetPixel((8 * scale - X) + x, Y + y, 0xFFFFFF);
            }
        }
    }
}

void Lockscreen()
{
    int Count = 100;
    while (Count--)
    {
        ClearScreen();
        DrawLogoAt(240, 100, 20);
        DrawString(180, 300, "BananaOS", 4, 0x000001);
        UpdateScreen();
    }
}
void StartClickAnimation()
{
    ClickAnimation.x = MouseX;
    ClickAnimation.y = MouseY;
    ClickAnimation.size = 1;
}
void ClickAnimationStep()
{
    if (ClickAnimation.size > 0)
    {
        float T = (float)ClickAnimation.size / 100.0f;
        for (int i = ClickAnimation.x - ClickAnimation.size / 4;i < ClickAnimation.x + ClickAnimation.size / 4;i++)
        {
            for (int j = ClickAnimation.y - ClickAnimation.size / 4;j < ClickAnimation.y + ClickAnimation.size / 4;j++)
            {
                if (i < 0) continue;
                if (j < 0) continue;
                if (i >= OUT_RES_X) continue;
                if (j >= OUT_RES_Y) continue;
                uint32_t Current = BackBuffer[i + j * OUT_RES_X];
                uint8_t CurrentR = (Current & 0xFF);
                uint8_t CurrentG = (Current & 0xFF00) >> 8;
                uint8_t CurrentB = (Current & 0xFF0000) >> 16;
                uint32_t NextR = 0xFF + T * (CurrentR - 0xFF);
                uint32_t NextG = 0xFF + T * (CurrentG - 0xFF);
                uint32_t NextB = 0xFF + T * (CurrentB - 0xFF);
                BackBuffer[i + j * OUT_RES_X] = NextR | (NextG << 8) | (NextB << 16);
            }   
        }
        ClickAnimation.size++;
        if (ClickAnimation.size == 100) ClickAnimation.size = 0;
    }
}
void ClickHandler()
{
    if (MouseLmbClicked == 1) 
    {
        KPrintf("LMB CLICKED\n");

        MouseLmbClicked = 0;
        StartClickAnimation();
    }
    if (MouseRmbClicked == 1) 
    {
        KPrintf("RMB CLICKED\n");
        MouseRmbClicked = 0;
    }
}
void KeyboardHandler()
{
    if (KeyboardCharPressed != 0xFF)
    {
        char B[2];
        B[0] = ps2tochar(KeyboardCharPressed);
        B[1] = 0;
        KPrintf(B);
        KeyboardCharPressed = 0xFF;
    }
}
void KeepMouseInScreen()
{
    if (MouseX < 0) MouseX = 0;
    if (MouseX > OUT_RES_X) MouseX = OUT_RES_X;
    if (MouseY < 0) MouseY = 0;
    if (MouseY > OUT_RES_Y) MouseY = OUT_RES_Y;
}
void HandleTime()
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    GetRTC(&second, &minute, &hour, &day, &month, &year);

}
void OS_Start()
{
    PIC_Init();
    PIC_SetMask(0xFFFF); // Disable all irqs

    MouseInstall();

    MouseX = 320;
    MouseY = 240;

    IDT_Init();
    PIC_SetMask(0x0000); // Enable all irqs
  
    Lockscreen();

    KPrintf("Welcome to BananaOS\n-------------------\n");

    int Color = 0x000001;
    int OffsetX = 0;
    while (1)
    {
        ClearScreen();
  
        KeyboardHandler();
        ClickHandler();

        DrawConsole(&Console, 12, 12, Color);
        ClickAnimationStep();
        KeepMouseInScreen();
        DrawPointerAt(MouseX, MouseY, 1);
        
        
        UpdateScreen();

        if (OffsetX > 400) OffsetX = 0;
    
    }
}
