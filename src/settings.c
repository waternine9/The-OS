#include "settings.h"

uint32_t SettingsDrawBuffer[640 * 480];
static uint32_t Events;

typedef struct {
    int X, Y;
    int Padding;
} settings_layout;

static settings_window Settings;

void SettingsWindowInit(rect *Rect)
{
    settings_window Init = { 0 };
    Init.Window.Rect = Rect;
    Init.Window.Framebuffer = SettingsDrawBuffer;
    Init.Window.Name = "settings uwu";
    Init.Window.WinProc = SettingsWindowProc;
    Init.Window.Events = &Events;
    RegisterWindow(Init.Window);

    Settings = Init;
}

static void DrawSwitch(int X, int Y, bool Value)
{
    uint32_t Color1 = 0xFFFF0000;
    if (Value) {
        Color1 = 0xFF00FF00;
    } 
    DrawRectOnto(X, Y, 40, 16, Color1, Settings.Window.Framebuffer, Settings.Window.Rect->W, Settings.Window.Rect->H);
    DrawRectOnto(X+2+(Value?20:0), Y+2, 18, 12, 0xFFFFFFFF, Settings.Window.Framebuffer, Settings.Window.Rect->W, Settings.Window.Rect->H);

}

static void DrawSwitchOption(settings_layout *Layout, const char *Name, bool *Value)
{
    int X = Layout->X, Y = Layout->Y, W = Settings.Window.Rect->W-Layout->Padding*2, H = 16;
    if (Settings.MouseX > X && Settings.MouseY > Y &&
        Settings.MouseX < (X + W) && Settings.MouseX < (Y + H) && Settings.MousePressed) {
        *Value = !*Value;
    }

    DrawTextOnto(Layout->X, Layout->Y, Name, 0xFFFFFFFF, Settings.Window.Framebuffer, Settings.Window.Rect->W, Settings.Window.Rect->H);

    Layout->X = Settings.Window.Rect->W-Layout->Padding;
    Layout->X -= 40;
    
    DrawSwitch(Layout->X, Layout->Y, *Value);

    Layout->X = Layout->Padding;
    Layout->Y += 16 + Layout->Padding;
}

void SettingsWindowProc(int MouseX, int MouseY, window* Win)
{
    settings_layout Layout;
    Layout.X = 10;
    Layout.Y = 10;
    Layout.Padding = 10;
   
    Settings.MouseX = MouseX-Settings.WindowRect.X;
    Settings.MouseY = MouseY-Settings.WindowRect.Y;
    Settings.MousePressed = Events&1;
    Events = Events & (~1);

    DrawSwitchOption(&Layout, "Light Theme", &Settings.Settings.LightTheme);
}