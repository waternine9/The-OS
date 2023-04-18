#include <stdint.h>
#include "fonts/sysfont.h"
#include "console.h"
#include "kernel.h"
#include "format.h"
#include "logo.h"
#include "pic.h"
#include "idt.h"
#include "io.h"
#include "rtc.h"
#include "drivers/ata/ata.h"
#include "pci.h"
#include "bmp.h"
#include "beescript.h"
#include "cmd.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/mouse/mouse.h"
#include "ps2help.h"
#include "OS.h"

extern click_animation ClickAnimation;
extern uint8_t MousePointerBlack[8];
extern uint8_t MousePointerFull[8];
extern uint8_t KeyboardCharPressed;

extern int32_t MouseX, MouseY;
extern uint8_t MouseRmbClicked, MouseLmbClicked;

extern uint16_t VESA_RES_X;
extern uint16_t VESA_RES_Y;

int32_t IsMouseMovingWin = -1;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

rect CombineRect(rect a, rect b)
{
    rect out;
    out.X = MIN(a.X, b.X);
    out.Y = MIN(a.Y, b.Y);
    out.W = MAX(a.X + a.W, b.X + b.W) - out.X + 1;
    out.H = MAX(a.Y + a.H, b.Y + b.H) - out.Y + 1;
    return out;
}

uint8_t IsInRect(rect _rect, int x, int y)
{
    if (x > _rect.X && y > _rect.Y && x < _rect.X + _rect.W && y < _rect.Y + _rect.H)
        return 1;
    return 0;
}

keyboard_key Keys[32];
keyboard Kbd = { 0 };

uint32_t BackBuffer[1920 * 1080];
uint32_t StaticBackBuffer[1920 * 1080];

uint32_t Destination[1920 * 1080];

extern struct _Resources ResourcesAt;

uint32_t WinBuffer0[100 * 100] = {0xFFFFFFFF};

rect RegisterRectArray[256];
rect *RegisterRectPtr = RegisterRectArray;

window RegisteredWinsArray[256];
uint32_t RegisteredWinsNum = 0;

window CmdWindow;
rect CmdWindowRect;

extern void CmdProc(int, int, struct _window*);

extern uint8_t IsFirstTime;

void RegisterRect(int x, int y, int w, int h)
{
    rect Rect = {x, y, w, h};
    *RegisterRectPtr = Rect;
    RegisterRectPtr++;
}

window* CreateWindow(rect* Rectptr, void(*WinProc)(int, int, window*), uint32_t* Icon32, uint32_t *Events, uint32_t* Framebuffer)
{
    window Win;
    Win.Free = 0;
    Win.Hidden = 0;
    Win.Rect = Rectptr;
    Win.WinProc = WinProc;
    Win.Icon32 = Icon32;
    Win.Events = Events;
    Win.Framebuffer = Framebuffer;
    return &RegisteredWinsArray[RegisterWindow(Win)];
}

int RegisterWindow(window _Window)
{
    _Window.ChQueueNum = 0;
    for (int i = 0;i < 256;i++)
    {
        _Window.InCharacterQueue[i] = 0;
    }

    uint32_t WinsNum = 0;
    while (WinsNum < RegisteredWinsNum)
    {
        if (RegisteredWinsArray[WinsNum].Free)
        {
            RegisteredWinsArray[WinsNum] = _Window;
            return WinsNum;
        }
        WinsNum++;
    }

    RegisteredWinsArray[RegisteredWinsNum] = _Window;
    RegisteredWinsNum++;
    return RegisteredWinsNum - 1;
}

void DestroyWindow(window* _window)
{
    uint32_t WinsNum = 0;
    while (WinsNum < RegisteredWinsNum)
    {
        if (&RegisteredWinsArray[WinsNum] == _window)
        {
            rect WinRect = *_window->Rect;
            RegisterRect(WinRect.X - 10, WinRect.Y - 40, WinRect.W + 20, WinRect.H + 80);
            RegisteredWinsArray[WinsNum].Free = 1;
            if (WinsNum == RegisteredWinsNum - 1)
            {
                RegisteredWinsNum--;
            }
            return;
        }
        WinsNum++;
    }
}

uint8_t Locked = 1;

volatile void SetPixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x < 0)
        return;
    if (y < 0)
        return;
    if (x >= VESA_RES_X)
        return;
    if (y >= VESA_RES_Y)
        return;
    if ((color >> 24) == 0)
        return;
    BackBuffer[x + y * VESA_RES_X] = color;
}

volatile void SetAlphaPixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x < 0)
        return;
    if (y < 0)
        return;
    if (x >= VESA_RES_X)
        return;
    if (y >= VESA_RES_Y)
        return;
    if ((color >> 24) == 0)
        return;
    uint32_t t = ((color >> 24)) << 8;
    t /= 255;
    uint32_t CurColor = (BackBuffer[x + y * VESA_RES_X] >= 0x01000000) ? BackBuffer[x + y * VESA_RES_X] : StaticBackBuffer[x + y * VESA_RES_X];
    int32_t CurR = ((CurColor & 0xFF) << 8);
    int32_t CurG = (((CurColor >> 8) & 0xFF) << 8);
    int32_t CurB = (((CurColor >> 16) & 0xFF) << 8);
    int32_t R = ((color & 0xFF) << 8);
    int32_t G = (((color >> 8) & 0xFF) << 8);
    int32_t B = (((color >> 16) & 0xFF) << 8);
    int32_t addR = t * (R - CurR);
    int32_t addG = t * (G - CurG);
    int32_t addB = t * (B - CurB);
    addR >>= 8;
    addG >>= 8;
    addB >>= 8;
    int32_t InterpR = CurR + addR;
    int32_t InterpG = CurG + addG;
    int32_t InterpB = CurB + addB;
    InterpR >>= 8;
    InterpG >>= 8;
    InterpB >>= 8;
    BackBuffer[x + y * VESA_RES_X] = 0xFF000000 | (InterpB << 16) | (InterpG << 8) | InterpR;
}

volatile void SetAlphaPixelOnto(uint32_t x, uint32_t y, uint32_t color, uint32_t* onto, uint32_t resX, uint32_t resY)
{
    if (x < 0)
        return;
    if (y < 0)
        return;
    if (x >= VESA_RES_X)
        return;
    if (y >= VESA_RES_Y)
        return;
    if ((color >> 24) == 0)
        return;
    uint32_t t = ((color >> 24)) << 8;
    t /= 255;
    uint32_t CurColor = onto[x + y * resX];
    int32_t CurR = ((CurColor & 0xFF) << 8);
    int32_t CurG = (((CurColor >> 8) & 0xFF) << 8);
    int32_t CurB = (((CurColor >> 16) & 0xFF) << 8);
    int32_t R = ((color & 0xFF) << 8);
    int32_t G = (((color >> 8) & 0xFF) << 8);
    int32_t B = (((color >> 16) & 0xFF) << 8);
    int32_t addR = t * (R - CurR);
    int32_t addG = t * (G - CurG);
    int32_t addB = t * (B - CurB);
    addR >>= 8;
    addG >>= 8;
    addB >>= 8;
    int32_t InterpR = CurR + addR;
    int32_t InterpG = CurG + addG;
    int32_t InterpB = CurB + addB;
    InterpR >>= 8;
    InterpG >>= 8;
    InterpB >>= 8;
    onto[x + y * resX] = 0xFF000000 | (InterpB << 16) | (InterpG << 8) | InterpR;
}

volatile void StaticSetPixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x < 0)
        return;
    if (y < 0)
        return;
    if (x >= VESA_RES_X)
        return;
    if (y >= VESA_RES_Y)
        return;
    if ((color >> 24) == 0)
        return;
    StaticBackBuffer[x + y * VESA_RES_X] = color;
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
void DrawFontGlyph(int x, int y, char character, int scale, uint32_t color)
{
    color &= 0x00FFFFFF;
    const uint8_t *glyph = ResourcesAt.Font + (character - 32) * 32 * 32;
    for (int i = 0; i < 8 * scale; i++)
    {
        for (int j = 0; j < 8 * scale; j++)
        {
            SetAlphaPixel(i + x, j + y, ((uint32_t)glyph[i * 4 / scale + j * 4 / scale * 32] << 24) | color);
        }
    }
}
void DrawFontGlyphOnto(int x, int y, char character, int scale, uint32_t color, uint32_t* onto, uint32_t resX, uint32_t resY)
{
    color &= 0x00FFFFFF;
    const uint8_t *glyph = ResourcesAt.Font + (character - 32) * 32 * 32;
    for (int i = 0; i < 8 * scale; i++)
    {
        for (int j = 0; j < 8 * scale; j++)
        {
            SetAlphaPixelOnto(i + x, 8 * scale - j + y, ((uint32_t)glyph[i * 4 / scale + j * 4 / scale * 32] << 24) | color, onto, resX, resY);
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
        if (x + 8 * scale > VESA_RES_X)
        {
            x = InitX;
            y += 8 * scale + 4 * scale;
        }
    }
    y += 8 * scale + 4 * scale;

    return y - InitY;
}
/* Returns Y stride */
uint32_t DrawFontString(int x, int y, const char *s, int scale, uint32_t color)
{
    int InitX = x, InitY = y;
    for (int i = 0; s[i]; i++)
    {
        DrawFontGlyph(x, y, s[i], scale, color);
        x += 7 * scale;
        if (x + 7 * scale > VESA_RES_X)
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

void DrawDragBar(int X, int Y, int W, int H)
{
    for (int _Y = Y; _Y < Y + H; _Y++)
    {
        uint8_t Counter = (_Y % 2) ? 1 : 0;
        for (int _X = X; _X < X + W; _X++)
        {
            if (Counter == 2)
            {
                SetPixel(_X, _Y, 0xFF222222);
                Counter = 0;
            }
            else
            {
                SetPixel(_X, _Y, 0xFFFFFFFF);
                Counter++;
            }
        }
    }
    RegisterRect(X, Y, W, H);
}

void DrawAlphaRect(int X, int Y, int W, int H, uint32_t Color)
{
    int InitX = X;
    int X2 = X + W, Y2 = Y + H;
    for (; Y < Y2; Y++)
    {
        for (; X < X2; X++)
        {
            SetAlphaPixel(X, Y, Color);
        }
        X = InitX;
    }
}
void DrawOutline(int X, int Y, int W, int H, int thickness)
{
    while (thickness--)
    {
        for (int _X = X - thickness; _X < X + W + thickness; _X++)
        {
            SetAlphaPixel(_X, Y - thickness, 0xFFFFFFFF);
            SetAlphaPixel(_X, Y + H + thickness, 0xFFFFFFFF);
        }
        for (int _Y = Y - thickness; _Y < Y + H + thickness; _Y++)
        {
            SetAlphaPixel(X - thickness, _Y, 0xFFFFFFFF);
            SetAlphaPixel(X + W + thickness, _Y, 0xFFFFFFFF);
        }
    }
}
volatile void ClearScreen()
{
    uint32_t *FramebufferStep = BackBuffer;
    for (int i = 0; i < VESA_RES_Y * VESA_RES_X; i++)
    {
        *FramebufferStep++ = 0;
    }
}
volatile void UpdateScreen()
{
    uint8_t *Framebuffer = ((uint8_t *)VbeModeInfo.framebuffer);

    for (int i = 0; i < VESA_RES_X * VESA_RES_Y; i++)
    {
        // NOTE: BackBuffer stores ARGB, with little endian its BGRA byte order.
        *Framebuffer++ = (BackBuffer[i] >= 0x01000000) ? BackBuffer[i] : StaticBackBuffer[i];
        *Framebuffer++ = (BackBuffer[i] >= 0x01000000) ? BackBuffer[i] >> 8 : StaticBackBuffer[i] >> 8;
        *Framebuffer++ = (BackBuffer[i] >= 0x01000000) ? BackBuffer[i] >> 16 : StaticBackBuffer[i] >> 16;
    }
}

uint32_t CountWindows()
{
    uint32_t Num = 0;
    uint32_t WinsNum = 0;
    while (WinsNum < RegisteredWinsNum)
    {
        if (!RegisteredWinsArray[WinsNum].Free)
            Num++;
        WinsNum++;
    }
    return Num;
}

volatile void RenderDynamic()
{
    // First, blit all windows
    uint32_t WinsNum = 0;
    uint32_t TaskbarLen = CountWindows() * 32;
    while (WinsNum < RegisteredWinsNum)
    {
        window Win = RegisteredWinsArray[WinsNum];
        if (Win.Free)
        {
            WinsNum++;
            continue;
        }
        DrawImage(VESA_RES_X / 2 - TaskbarLen / 2 + WinsNum * 32 + 32, VESA_RES_Y - 40, 32, 32, Win.Icon32);
        DrawOutline(VESA_RES_X / 2 - TaskbarLen / 2 + WinsNum * 32 + 32 - 1, VESA_RES_Y - 40 - 1, 34, 34, 1);
        if (WinsNum == RegisteredWinsNum - 1)
        {
            DrawRect(VESA_RES_X / 2 - TaskbarLen / 2 + WinsNum * 32 + 32, VESA_RES_Y - 3, 32, 2, 0xFFFFFFFF);
        }

        if (Win.Hidden)
        {
            WinsNum++;
            continue;
        }

        DrawOutline(Win.Rect->X - 1, Win.Rect->Y - 10, Win.Rect->W + 1, Win.Rect->H + 10, 1);
        DrawDragBar(Win.Rect->X, Win.Rect->Y - 10, Win.Rect->W, 10);
        RegisterRect(Win.Rect->X - 4, Win.Rect->Y - 14, Win.Rect->W + 8, Win.Rect->H + 18);
        DrawImage(Win.Rect->X, Win.Rect->Y, Win.Rect->W, Win.Rect->H, Win.Framebuffer);

        WinsNum++;
    }

    KeepMouseInScreen();
    DrawPointerAt(MouseX, MouseY, 1);

    rect *RegRectPtr = RegisterRectArray;
    while (RegRectPtr < RegisterRectPtr)
    {
        rect CurRect = *RegRectPtr;
        int x = CurRect.X;
        int y = CurRect.Y;
        int w = CurRect.W;
        int h = CurRect.H;
        uint8_t *Framebuffer = ((uint8_t *)VbeModeInfo.framebuffer) + (x + y * VESA_RES_X) * 3;

        uint32_t *BackBufferStep = BackBuffer + (x + y * VESA_RES_X);
        uint32_t *StaticBackBufferStep = StaticBackBuffer + (x + y * VESA_RES_X);

        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                // NOTE: BackBuffer stores ARGB, with little endian its BGRA byte order.

                *Framebuffer++ = (*BackBufferStep >= 0x01000000) ? *BackBufferStep : *StaticBackBufferStep;
                *Framebuffer++ = (*BackBufferStep >= 0x01000000) ? *BackBufferStep >> 8 : *StaticBackBufferStep >> 8;
                *Framebuffer++ = (*BackBufferStep >= 0x01000000) ? *BackBufferStep >> 16 : *StaticBackBufferStep >> 16;
                BackBufferStep++;
                StaticBackBufferStep++;
            }
            Framebuffer += (VESA_RES_X - w) * 3;
            BackBufferStep += (VESA_RES_X - w);
            StaticBackBufferStep += (VESA_RES_X - w);
        }
        RegRectPtr++;
    }
    RegisterRectPtr = RegisterRectArray;
}
void DrawConsole(console *Console, int X, int Y, int Color)
{
    for (uint32_t i = 0; i < CONSOLE_MAX_LINES; i++)
    {
        Y += DrawString(X, Y, Console->Lines[i], 1, Color);
    }
}
console Console;

int KPrintf(const char *Fmt, ...)
{
    va_list Args;
    va_start(Args, Fmt);
    char DestStr[256];
    FormatWriteStringVa(DestStr, 256, Fmt, Args);
    ConsoleWrite(&Console, DestStr);
    va_end(Args);
}
int ConPrintf(const char *Fmt, ...)
{
    va_list Args;
    va_start(Args, Fmt);
    char DestStr[256];
    FormatWriteStringVa(DestStr, 256, Fmt, Args);
    for (int I = 0; DestStr[I]; I++)
        CmdAddChar(DestStr[I]);
    ConsoleWrite(&Console, DestStr);
    va_end(Args);
}

void DrawImage(uint32_t x, uint32_t y, uint32_t resX, uint32_t resY, uint32_t *data)
{
    for (int32_t Y = resY - 1; Y >= 0; Y--)
    {
        for (uint32_t X = 0; X < resX; X++)
        {
            SetPixel(X + x, Y + y, 0xFF000000 | *data++);
        }
    }
    RegisterRect(x, y, resX, resY);
}
void DrawAlphaImage(uint32_t x, uint32_t y, uint32_t resX, uint32_t resY, uint32_t *data)
{
    for (int32_t Y = resY - 1; Y >= 0; Y--)
    {
        for (uint32_t X = 0; X < resX; X++)
        {
            SetAlphaPixel(X + x, Y + y, *data++);
        }
    }
}

void DrawBackground(uint32_t x, uint32_t y, uint32_t resX, uint32_t resY, uint32_t targetresX, uint32_t targetresY, uint32_t *data)
{
    for (int32_t Y = targetresY - 1; Y >= 0; Y--)
    {

        for (uint32_t X = 0; X < targetresX; X++)
        {

            StaticSetPixel(X + x, Y + y, 0xFF000000 | *data++);
        }

        data += resX - targetresX;
    }
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
                SetPixel((8 * scale - X) + x, Y + y, 0xFF773300);
            }
            else if (CurBlack)
            {
                SetPixel((8 * scale - X) + x, Y + y, 0xFF000000);
            }
            else if (CurYellow)
            {
                SetPixel((8 * scale - X) + x, Y + y, 0xFFFFFF00);
            }
        }
    }
}
uint32_t LastMouseX = 0;
uint32_t LastMouseY = 0;
void DrawPointerAt(uint32_t x, uint32_t y, int scale)
{

    rect A = {x, y, 8 * scale, 8 * scale};
    rect B = {LastMouseX, LastMouseY, 8 * scale, 8 * scale};
    rect CoverRect;
    CoverRect = CombineRect(A, B);
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
                SetPixel((8 * scale - X) + x, Y + y, 0xFFFFFFFF);
            }
            else if (CurFull)
            {
                SetPixel((8 * scale - X) + x, Y + y, 0xFF000000);
            }
        }
    }
    RegisterRect(CoverRect.X, CoverRect.Y, CoverRect.W, CoverRect.H);

    LastMouseX = x;
    LastMouseY = y;
}

void Lockscreen()
{
    if (Locked == 0xFF)
        ClearScreen();
    DrawString(250, 200, "Start", 4, 0xFF000000);
    UpdateScreen();
}

void ProbeAllPCIDevices()
{
    for (int Bus = 0; Bus < 256; Bus++)
    {
        for (int Device = 0; Device < 32; Device++)
        {
            pci_device_path Path;
            Path.Bus = Bus;
            Path.Device = Device;
            Path.Function = 0;

            int F = 1;
            if (PCI_QueryDeviceHeader(Path).MultiFunction)
            {
                F = 8;
            }

            for (int I = 0; I < F; I++)
            {
                Path.Function = I;

                pci_device_header Header = PCI_QueryDeviceHeader(Path);
                if (Header.VendorId == 0xFFFF || Header.VendorId == 0x0000)
                {
                    continue;
                }

                KPrintf("TYP ");
                switch (PCI_QueryDeviceSpecialty(Header))
                {
                case PCI_DEVICE_UNKNOWN:
                    KPrintf("Unknown device");
                    break;
                case PCI_DEVICE_VGA:
                    KPrintf("VGA-compatible");
                    break;
                case PCI_DEVICE_ETHERNET:
                    KPrintf("Ethernet-compatible");
                    break;
                case PCI_DEVICE_IDE:
                    KPrintf("IDE-compatible");
                    break;
                case PCI_DEVICE_HOST_BRIDGE:
                    KPrintf("Host bridge");
                    break;
                case PCI_DEVICE_ISA_BRIDGE:
                    KPrintf("ISA bridge");
                    break;
                }
                KPrintf("\nBUS %d | DEV %d | FUN %d | VEN %d | CLS %d | SUB %d | HED %d | +(",
                        Bus, Device, I, Header.VendorId, Header.Class, Header.Subclass, Header.HeaderType);
                if (Header.MultiFunction || I != 0)
                {
                    KPrintf("MUF");
                }
                KPrintf(")\n");
            }
        }
    }
}

void BringWindowToFront(uint32_t WinId)
{
    window temp = RegisteredWinsArray[RegisteredWinsNum - 1];
    RegisteredWinsArray[RegisteredWinsNum - 1] = RegisteredWinsArray[WinId];
    RegisteredWinsArray[WinId] = temp;
}

void WinLmbHandler()
{
    if (IsMouseMovingWin != -1)
    {
        IsMouseMovingWin = -1;
        return;
    }
    uint32_t TaskbarLen = CountWindows() * 32;
    int32_t WinsNum = RegisteredWinsNum - 1;
    while (WinsNum >= 0)
    {
        window Win = RegisteredWinsArray[WinsNum];
        rect WinRect = *Win.Rect;
        if (Win.Free)
        {
            WinsNum--;
            continue;
        }
        
        rect WinTaskRect = {VESA_RES_X / 2 - TaskbarLen / 2 + WinsNum * 32 + 32, VESA_RES_Y - 40, 32, 32};
        if (IsInRect(WinTaskRect, MouseX, MouseY))
        {
            BringWindowToFront(WinsNum);
            RegisteredWinsArray[WinsNum].Hidden = 0;
        }
        if (Win.Hidden)
        {
            WinsNum--;
            continue;
        }
        if (IsInRect(WinRect, MouseX, MouseY))
        {

            BringWindowToFront(WinsNum);
            *Win.Events |= 1;
            return;
        }
        WinRect.Y -= 14;
        WinRect.H = 14;


        if (IsInRect(WinRect, MouseX, MouseY))
        {
            IsMouseMovingWin = WinsNum;
            return;
        }

        WinsNum--;
    }
}
int32_t LastMoveMouseX, LastMoveMouseY;
void MoveWinHandler()
{
    if (IsMouseMovingWin != -1)
    {

        rect OldRect = *RegisteredWinsArray[IsMouseMovingWin].Rect;
        int32_t DiffX = MouseX - LastMoveMouseX;
        int32_t DiffY = MouseY - LastMoveMouseY;
        RegisteredWinsArray[IsMouseMovingWin].Rect->X += DiffX;
        RegisteredWinsArray[IsMouseMovingWin].Rect->Y += DiffY;
        rect NewRect = *RegisteredWinsArray[IsMouseMovingWin].Rect;
        rect Combined;
        OldRect.Y -= 10;
        OldRect.H += 10;
        Combined = CombineRect(OldRect, NewRect);
        RegisterRect(Combined.X - 5, Combined.Y - 5, Combined.W + 10, Combined.H + 10);
    }
    LastMoveMouseX = MouseX;
    LastMoveMouseY = MouseY;
}
void ClickHandler()
{
    if (MouseLmbClicked == 1)
    {
        WinLmbHandler();
        MouseLmbClicked = 0;
    }
    if (MouseRmbClicked == 1)
    {
        MouseRmbClicked = 0;
    }
}
void KeepMouseInScreen()
{
    if (MouseX < 0)
        MouseX = 0;
    if (MouseX > VESA_RES_X)
        MouseX = VESA_RES_X;
    if (MouseY < 0)
        MouseY = 0;
    if (MouseY > VESA_RES_Y)
        MouseY = VESA_RES_Y;
}
const char *Numst(int num)
{
    if (num == 1)
        return "st";
    if (num == 2)
        return "nd";
    if (num == 3)
        return "rd";
    return "th";
}
const char *Num2Str[100] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99"};
void DrawToolBar(int scale)
{
    DrawAlphaRect(0, VESA_RES_Y - 25 * scale, VESA_RES_X, 25 * scale, 0x77000000);
    RegisterRect(0, VESA_RES_Y - 25 * scale, VESA_RES_X, 25 * scale);
}

int IsMouseInRect(rect R)
{
    return MouseX > R.X && MouseY > R.Y && MouseX <= (R.X + R.W) && MouseY <= (R.Y + R.H);
}

void UpdateWinProcs()
{
    for (int i = 0;i < RegisteredWinsNum;i++)
    {
        (*RegisteredWinsArray[i].WinProc)(MouseX, MouseY, &RegisteredWinsArray[i]);
    }
}

void Format()
{
    uint8_t EmptyBuff[512] = { 0 };
    int SectorCount = 256;
    while (SectorCount--) WriteATASector(EmptyBuff, 17000 + SectorCount);
    WriteATASector(EmptyBuff, (uint32_t)(&IsFirstTime - 0x7C00) / 512);
}

void FirstTimeSetup()
{
    Format();
}

void ReadFile(uint8_t* Dest, uint32_t FileNum)
{
    uint32_t LBA = 17000 + (1 * FileNum);
    int SectorCount = 0;
    while (SectorCount < 1)
    {
        ReadATASector(Dest + SectorCount * 512, LBA + SectorCount);
        SectorCount++;
    }
}

void WriteFile(uint8_t* Source, uint32_t FileNum)
{
    uint32_t LBA = 17000 + (1 * FileNum);
    int SectorCount = 0;
    while (SectorCount < 1)
    {
        WriteATASector(Source + SectorCount * 512, LBA + SectorCount);
        SectorCount++;
    }
}

char Fmt[256];

void OS_Start()
{
    PIC_Init();
    PIC_SetMask(0xFFFF); // Disable all irqs

    MouseInstall();

    MouseX = 320;
    MouseY = 240;

    IDT_Init();
    PIC_SetMask(0x0000); // Enable all irqs
    ProbeAllPCIDevices();

    batch_script Script = {};

    int ConsoleColor = 0xFF000000;  
    int OffsetX = 0;


    bmp_bitmap_info BMPInfo;
    BMP_Read(ResourcesAt.Buf, &BMPInfo, Destination);
    char CmdLine[129] = {0};
    int CmdLineLen = 0;

    InitCMD();

    FormatWriteString(Fmt, 256, "test %u %u %u %u %u %u", 100, 200, 300, 400, 500, 600);
    ConPrintf(Fmt);

    DrawBackground(0, 0, 1920, 1080, VESA_RES_X, VESA_RES_Y, Destination);
    UpdateScreen();
    uint32_t KeysCount = 0;
    rect LastRect = {0};

    CmdWindowRect.X = 400;
    CmdWindowRect.Y = 400;
    CmdWindowRect.W = CONSOLE_RES_X;
    CmdWindowRect.H = CONSOLE_RES_Y;
    CmdWindow.Rect = &CmdWindowRect;

    CmdWindow.Framebuffer = CmdDrawBuffer;
    CmdWindow.Free = 0;
    CmdWindow.Hidden = 0;
    CmdWindow.Icon32 = ResourcesAt.Icons;
    CmdWindow.WinProc = &CmdProc;

    RegisterWindow(CmdWindow);

    if (IsFirstTime)
    {
        DrawAlphaRect(0, 0, 1920, 40, 0x77000000);
        DrawFontString(1920 / 2 - FormatCStringLength("Please wait") * 16 + 32, 0, "Please wait", 4, 0xFFFFFFFF);
        UpdateScreen();
        FirstTimeSetup();
    }

    DrawBackground(0, 0, 1920, 1080, VESA_RES_X, VESA_RES_Y, Destination);

    while (1)
    {
        ClearScreen();
        DrawToolBar(2);

        Keyboard_CollectEvents(&Kbd, Keys, 32, &KeysCount);
        for (int I = 0; I < KeysCount; I++)
        {
            if (!Keys[I].Released)
            {
                if (RegisteredWinsNum != 0)
                {
                    if (Keys[I].Scancode == KEY_BACKSPACE)
                    {
                        window* Win = &RegisteredWinsArray[RegisteredWinsNum - 1];
                        if (Win->ChQueueNum < 256)
                        {
                            Win->InCharacterQueue[Win->ChQueueNum] = 1 << 8;
                            Win->ChQueueNum++;
                        }
                    }
                    if (Keys[I].ASCII)
                    {
                        
                        window* Win = &RegisteredWinsArray[RegisteredWinsNum - 1];
                        if (Win->ChQueueNum < 256)
                        {
                            Win->InCharacterQueue[Win->ChQueueNum] = Keys[I].ASCII;
                            Win->ChQueueNum++;
                        }
                    }
                }
            }
            
        }
        KeepMouseInScreen();
        MoveWinHandler();
        UpdateWinProcs();
        ClickHandler();

        if (OffsetX > 400)
            OffsetX = 0;
        RenderDynamic();
    }
}
