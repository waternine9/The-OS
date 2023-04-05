#include <stdint.h>
#include "fonts/sysfont.h"
#include "include/console.h"
#include "include/kernel.h"
#include "include/format.h"
#include "include/logo.h"
#include "include/pic.h"
#include "include/idt.h"
#include "include/io.h"
#include "include/rtc.h"
#include "include/mouse.h"
#include "drivers/ata/ata.h"
#include "include/pci.h"
#include "include/bmp.h"
#include "include/cmd.h"

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

uint8_t Locked = 1;

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
                SetPixel((8 * scale - X) + x, Y + y, 0xFF000000);
            }
            else if (CurFull)
            {
                SetPixel((8 * scale - X) + x, Y + y, 0xFFFFFFFF);
            }
        }
    }
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
                if (i >= OUT_RES_X)
                    continue;
                if (j >= OUT_RES_Y)
                    continue;
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
        KPrintf("LMB CLICKED\n");
        MouseLmbClicked = 0;
        StartClickAnimation();
        CmdAddChar('D');
    }
    if (MouseRmbClicked == 1)
    {
        KPrintf("RMB CLICKED\n");
        MouseRmbClicked = 0;
        CmdAddChar('\n');
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
    if (MouseX < 0)
        MouseX = 0;
    if (MouseX > OUT_RES_X)
        MouseX = OUT_RES_X;
    if (MouseY < 0)
        MouseY = 0;
    if (MouseY > OUT_RES_Y)
        MouseY = OUT_RES_Y;
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
    DrawRect(0, 0, 640, 12, 0xFF000000);

    uint8_t second, minute, hour, day, weekday, month, year;
    GetRTC(&second, &minute, &hour, &day, &weekday, &month, &year);

    char ClockBuffer[128] = {0};
    FormatWriteString(ClockBuffer, sizeof ClockBuffer, "%s, %s %d%s %d %d:%02d:%02d", WeekDayName(weekday), MonthName(month), day, Numst(day), 2000 + year, hour, minute, second);

    int RightOffset = FormatCStringLength(ClockBuffer) * 8;
    DrawString(640 - RightOffset - 2, 2, ClockBuffer, 1, 0xFFFFFFFF);
}
typedef struct
{
    int X, Y, W, H;
} rect;
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

void OS_Start()
{
    ClearScreen();
    DrawString(240, 220, "LOADING :)", 2, 0xFF000000);
    UpdateScreen();
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

    int ConsoleColor = 0xFF000000;
    int OffsetX = 0;

    uint32_t *Destination = (uint32_t *)0x4000000;
    uint8_t *Buf = (uint8_t *)0x1000000;
    for (int I = 0; I < 2400; I++)
    {
        ReadATASector(Buf + I * 512, I);
    }


    bmp_bitmap_info BMPInfo;
    BMP_Read(Buf, &BMPInfo, Destination);

    InitCMD();

    while (1)
    {
        ClearScreen();
        DrawImage(0, 0, 640, 480, Destination);
        KeyboardHandler();
        
        DrawConsole(&Console, 12, 20, ConsoleColor);
        DrawToolBar();
        
        
        CmdClear();
        CmdDraw(0xFFFFFFFF);
        DrawImage(340, 280, CONSOLE_RES_X, CONSOLE_RES_Y, CmdDrawBuffer);
        
        KeepMouseInScreen();
        DrawPointerAt(MouseX, MouseY, 1);
        ClickHandler();
        ClickAnimationStep();

        UpdateScreen();
        if (OffsetX > 400)
            OffsetX = 0;
    }
}
