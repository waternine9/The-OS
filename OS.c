#include <stdint.h>
#include "sysfont.h"
#include "console.h"
#include "kernel.h"
#include "format.h"
#include "logo.h"
#include "pic.h"
#include "idt.h"
#include "io.h"
#include "mouse.h"
#include "pci.h"

extern int32_t MouseX, MouseY;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

const uint32_t OUT_RES_X = 640;
const uint32_t OUT_RES_Y = 480;

uint32_t BackBuffer[640 * 480];

void SetPixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x < 0) return;
    if (y < 0) return;
    if (x >= 640) return;
    if (y >= 480) return;
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

void ProbeAllPCIDevices() {
    for (int Bus = 0; Bus < 256; Bus++) {
        for (int Device = 0; Device < 32; Device++) {
            pci_device_path Path;
            Path.Bus = Bus;
            Path.Device = Device;
            Path.Function = 0;

            int F = 1;
            if (PCI_QueryDeviceHeader(Path).MultiFunction) {
                F = 8;
            }

            for (int I = 0; I < F; I++) {
                Path.Function = I;

                pci_device_header Header = PCI_QueryDeviceHeader(Path);
                if (Header.VendorId == 0xFFFF || Header.VendorId == 0x0000) {
                    continue;
                }

                KPrintf("TYP ");
                switch (PCI_QueryDeviceSpecialty(Header)) {
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
                if (Header.MultiFunction || I != 0) {
                    KPrintf("MUF");
                }
                KPrintf(")\n");
            }
        }
    }
}

void OS_Start()
{
    PIC_Init();
    PIC_SetMask(0xFFFF); // Disable all irqs

    MouseInstall();

    IDT_Init();
    PIC_SetMask(0x0000); // Enable all irqs
  

    KPrintf("Welcome to BananaOS\n-------------------\n");
    ProbeAllPCIDevices();
    int Color = 0x000001;
    int OffsetX = 0;
    while (1)
    {
        ClearScreen();
        DrawConsole(&Console, 12, 12, Color);

    if (OffsetX > 400) OffsetX = 0;
    DrawLogoAt(MouseX, MouseY, 2);
    UpdateScreen();
    
    char b[2] = { 0, 0 };

  }
}
