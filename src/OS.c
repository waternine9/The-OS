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

typedef struct
{
    int X, Y, W, H;
} rect;



extern click_animation ClickAnimation;
extern uint8_t MousePointerBlack[8];
extern uint8_t MousePointerFull[8];
extern uint8_t KeyboardCharPressed;

extern int32_t MouseX, MouseY;
extern uint8_t MouseRmbClicked, MouseLmbClicked;

extern uint16_t VESA_RES_X;
extern uint16_t VESA_RES_Y;

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

uint32_t BackBuffer[1920 * 1080];
uint32_t StaticBackBuffer[1920 * 1080];

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
volatile void ClearScreen()
{
    uint32_t *FramebufferStep = BackBuffer;
    for (int i = 0; i < VESA_RES_Y * VESA_RES_X; i++)
    {
        *FramebufferStep++ = 0;
    }
}
volatile void ClearRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    uint32_t *FramebufferStep = BackBuffer + x + y * VESA_RES_X;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            *FramebufferStep++ = 0;
        }
        FramebufferStep += VESA_RES_X - w;
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
volatile void UpdateRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
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
        Framebuffer += (VbeModeInfo.width - w) * 3;
        BackBufferStep += (VbeModeInfo.width - w);
        StaticBackBufferStep += (VbeModeInfo.width - w);
    }
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

    rect A = { x, y, 8 * scale, 8 * scale };
    rect B = { LastMouseX, LastMouseY, 8 * scale, 8 * scale};
    rect CoverRect;
    CoverRect = CombineRect(A, B);
    ClearRect(CoverRect.X, CoverRect.Y, CoverRect.W, CoverRect.H);
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
                SetPixel((8 * scale - X) + x, Y + y, 0xFF000000);
            }
            else if (CurFull)
            {
                SetPixel((8 * scale - X) + x, Y + y, 0xFFFFFFFF);
            }
        }
    }
    UpdateRect(CoverRect.X, CoverRect.Y, CoverRect.W, CoverRect.H);


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
        for (int i = ClickAnimation.x - ClickAnimation.size / 4; i < ClickAnimation.x + ClickAnimation.size / 4; i++)
        {
            for (int j = ClickAnimation.y - ClickAnimation.size / 4; j < ClickAnimation.y + ClickAnimation.size / 4; j++)
            {
                if (i < 0)
                    continue;
                if (j < 0)
                    continue;
                if (i >= VESA_RES_X)
                    continue;
                if (j >= VESA_RES_Y)
                    continue;
                uint32_t Current = BackBuffer[i + j * VESA_RES_X];
                uint8_t CurrentR = (Current & 0xFF);
                uint8_t CurrentG = (Current & 0xFF00) >> 8;
                uint8_t CurrentB = (Current & 0xFF0000) >> 16;
                uint32_t NextR = 0xFF + T * (CurrentR - 0xFF);
                uint32_t NextG = 0xFF + T * (CurrentG - 0xFF);
                uint32_t NextB = 0xFF + T * (CurrentB - 0xFF);
                BackBuffer[i + j * VESA_RES_X] = NextR | (NextG << 8) | (NextB << 16);
            }
        }
        ClickAnimation.size++;
        if (ClickAnimation.size == 100)
            ClickAnimation.size = 0;
    }
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

void ClickHandler()
{
    if (MouseLmbClicked == 1)
    {
        MouseLmbClicked = 0;
        StartClickAnimation();
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
void DrawToolBar()
{
    DrawRect(0, 0, VESA_RES_X, 12, 0xFF000000);
    

    uint8_t second, minute, hour, day, weekday, month, year;
    GetRTC(&second, &minute, &hour, &day, &weekday, &month, &year);

    char ClockBuffer[128] = {0};
    FormatWriteString(ClockBuffer, sizeof ClockBuffer, "%s, %s %d%s %d %d:%02d:%02d", WeekDayName(weekday), MonthName(month), day, Numst(day), 2000 + year, hour, minute, second);

    int RightOffset = FormatCStringLength(ClockBuffer) * 8;
    DrawString(VESA_RES_X - RightOffset - 2, 2, ClockBuffer, 1, 0xFFFFFFFF);
    UpdateRect(0, 0, VESA_RES_X, 12);
}

int IsMouseInRect(rect R)
{
    return MouseX > R.X && MouseY > R.Y && MouseX <= (R.X + R.W) && MouseY <= (R.Y + R.H);
}

int DrawPaintProgram(uint8_t *PixBuf, int PX, int PY, uint32_t W, uint32_t H, uint32_t S)
{
    int Dirty = 0;
    for (int Y = 0; Y < H; Y++)
    {
        for (int X = 0; X < W; X++)
        {
            rect R = {PX + X * S, PY + Y * S, S, S};
            if (PixBuf[X + Y * W])
            {
                DrawRect(R.X, R.Y, R.W, R.H, 0xFF000000);
            }
            else
            {
                DrawRect(R.X, R.Y, R.W, R.H, 0xFFFFFFFF);
            }

            if (IsMouseInRect(R) && MouseLmbClicked)
            {
                Dirty = 1;
                PixBuf[X + Y * W] = !PixBuf[X + Y * W];
            }
        }
    }

    return Dirty;
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


void OS_Start()
{

    PIC_Init();
    PIC_SetMask(0xFFFF); // Disable all irqs

    MouseInstall();

    MouseX = 320;
    MouseY = 240;

    IDT_Init();
    PIC_SetMask(0x0000); // Enable all irqs

    ATASetPIO();

    KPrintf("Welcome to BananaOS\n-------------------\n");
    ProbeAllPCIDevices();

    batch_script Script = {};

    int ConsoleColor = 0xFF000000;
    int OffsetX = 0;

    uint32_t *Destination = (uint32_t *)0x5000000;
    uint8_t *Buf = (uint8_t *)0x4000000;
    for (int I = 0; I < 16200; I++)
    {
        ReadATASector(Buf + I * 512, I);
    }

    bmp_bitmap_info BMPInfo;
    BMP_Read(Buf, &BMPInfo, Destination);
    char CmdLine[129] = {0};
    int CmdLineLen = 0;

    InitCMD();
    DrawBackground(0, 0, 1920, 1080, VESA_RES_X, VESA_RES_Y, Destination);
    UpdateScreen();
    keyboard Kbd = {0};
    keyboard_key Keys[32];
    uint32_t KeysCount = 0;

    while (1)
    {
        
        DrawToolBar();

        
        CmdClear();
        CmdDraw(0xFFFFFFFF);
        DrawImage(VESA_RES_X - CONSOLE_RES_X, VESA_RES_Y - CONSOLE_RES_Y, CONSOLE_RES_X, CONSOLE_RES_Y, CmdDrawBuffer);

        UpdateRect(VESA_RES_X - CONSOLE_RES_X, VESA_RES_Y - CONSOLE_RES_Y, CONSOLE_RES_X, CONSOLE_RES_Y);

        Keyboard_CollectEvents(&Kbd, Keys, 32, &KeysCount);
        for (int I = 0; I < KeysCount; I++)
        {
            if (!Keys[I].Released)
            {
                if (Keys[I].Scancode == KEY_BACKSPACE)
                {
                    if (CmdLineLen > 0)
                    {
                        CmdBackspace();
                        CmdLineLen--;
                    }
                }
                if (Keys[I].ASCII)
                {
                    if (CmdLineLen < 128)
                    {
                        KPrintf("%c", Keys[I].ASCII);
                        CmdAddChar(Keys[I].ASCII);
                        CmdLine[CmdLineLen++] = Keys[I].ASCII;
                    }
                }
                if (Keys[I].ASCII == '\n')
                {
                    CmdLine[CmdLineLen] = 0;
                    Script.Source = CmdLine;
                    Bee_ExecuteBatchScript(&Script);
                    CmdLineLen = 0;
                }
            }
        }
        ClickHandler();
        ClickAnimationStep();
        KeepMouseInScreen();
        DrawPointerAt(MouseX, MouseY, 2);
        

        if (OffsetX > 400)
            OffsetX = 0;
    }
}
