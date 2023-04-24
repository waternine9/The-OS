#include "settings.h"
#include "mem.h"

typedef struct
{
    int X, Y;
    int Padding;
} settings_layout;

user_settings Settings;

void SettingsClearFramebuffer(uint32_t Color, window *Win)
{
    for (int i = 0; i < 640 * 480; i++)
    {
        Win->Framebuffer[i] = Color;
    }
}

void SettingsDestructor(window* Win)
{

}

void SettingsCreateWindow()
{
    rect* Rect = (rect*)malloc(sizeof(rect));
    Rect->X = 100;
    Rect->Y = 100;
    Rect->W = 640;
    Rect->H = 480;
    CreateWindow(Rect, &SettingsWindowProc, &SettingsDestructor, "settings", malloc(4), malloc(640 * 480 * 4), 0, 0);
}

static void DrawSwitch(int X, int Y, bool Value, window *Win)
{
    uint32_t Color1 = 0xFFFF0000;
    if (Value)
    {
        Color1 = 0xFF00FF00;
    }
    DrawRectOnto(X, Y, 40, 16, Color1, Win->Framebuffer, Win->Rect->W, Win->Rect->H);
    DrawRectOnto(X + 2 + (Value ? 20 : 0), Y + 2, 18, 12, 0xFFFFFFFF, Win->Framebuffer, Win->Rect->W, Win->Rect->H);
}

static void DrawSwitchOption(settings_layout *Layout, const char *Name, bool *Value, window *Win, int MouseX, int MouseY, int LmbPressed)
{
    int X = Layout->X, Y = Layout->Y, W = Win->Rect->W - Layout->Padding * 2, H = 16;
    if (MouseX > X && MouseY > Y &&
        MouseX < (X + W) && MouseY < (Y + H) && LmbPressed)
    {
        *Value = !*Value;
    }

    DrawTextOnto(Layout->X, Layout->Y, Name, Settings.LightTheme ? 0 : 0xFFFFFFFF, Win->Framebuffer, Win->Rect->W, Win->Rect->H);

    Layout->X = Win->Rect->W - Layout->Padding;
    Layout->X -= 40;

    DrawSwitch(Layout->X, Layout->Y, *Value, Win);

    Layout->X = Layout->Padding;
    Layout->Y += 16 + Layout->Padding;
}

void SettingsWindowProc(int MouseX, int MouseY, window *Win)
{
    SettingsClearFramebuffer(Settings.LightTheme ? 0xFFFFFFFF : 0, Win);

    settings_layout Layout;
    Layout.X = 10;
    Layout.Y = 10;
    Layout.Padding = 10;

    int RelMouseX = MouseX - Win->Rect->X;
    int RelMouseY = MouseY - Win->Rect->Y;
    int MousePressed = *(Win->Events) & 1;
    *Win->Events &= ~1;

    int I = 0;
    while (I < Win->ChQueueNum)
    {
        uint16_t packet = Win->InCharacterQueue[I];
        uint8_t C = packet & 0xFF;
        if (C)
        {
            if (C == 'm' && packet & (1 << 8))
            {
                
                HideWindow(Win);
            }
            if (C == 'd' && packet & (1 << 9))
            {

                DestroyWindow(Win);
                return;
            }
        }
        I++;
    }
    Win->ChQueueNum = 0;

    DrawSwitchOption(&Layout, "Light Theme", &Settings.LightTheme, Win, RelMouseX, RelMouseY, MousePressed);
}