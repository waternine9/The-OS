#include "settings.h"
#include "mem.h"

typedef struct
{
    int X, Y;
    int Padding;
} settings_layout;

user_settings Settings;

typedef struct
{
    uint32_t* BackBuff;
} settings_reserve;


void SettingsClearFramebuffer(uint32_t Color, window *Win)
{
    settings_reserve* Rsrv = (settings_reserve*)Win->Reserved;
    for (int i = 0; i < SETTINGS_RES_X * SETTINGS_RES_Y; i++)
    {
        Rsrv->BackBuff[i] = Color;
    }
}

void SettingsDestructor(window* Win)
{
    settings_reserve* Rsrv = (settings_reserve*)Win->Reserved;
    free(Rsrv->BackBuff, SETTINGS_RES_X * SETTINGS_RES_Y * 4);
}

void SettingsWinHostProc(window* Win)
{

}

void SettingsCreateWindow(int x, int y)
{
    rect* Rect = (rect*)malloc(sizeof(rect));
    Rect->X = x;
    Rect->Y = y;
    Rect->W = SETTINGS_RES_X;
    Rect->H = SETTINGS_RES_Y;
    
    settings_reserve* Rsrv = (settings_reserve*)malloc(sizeof(settings_reserve));
    Rsrv->BackBuff = malloc(SETTINGS_RES_X * SETTINGS_RES_Y * 4);
    CreateWindow(Rect, &SettingsWindowProc, &SettingsWinHostProc, &SettingsDestructor, "settings", malloc(4), malloc(SETTINGS_RES_X * SETTINGS_RES_Y * 4), (uint8_t*)Rsrv, sizeof(settings_reserve));
}

static void DrawSwitch(int X, int Y, bool Value, window *Win)
{
    settings_reserve* Rsrv = (settings_reserve*)Win->Reserved;
    uint32_t Color1 = 0xFFFF0000;
    if (Value)
    {
        Color1 = 0xFF00FF00;
    }
    DrawRectOnto(X, Y, 40, 16, Color1, Rsrv->BackBuff, Win->Rect->W, Win->Rect->H);
    DrawRectOnto(X + 2 + (Value ? 20 : 0), Y + 2, 18, 12, 0xFFFFFFFF, Rsrv->BackBuff, Win->Rect->W, Win->Rect->H);
}

static void DrawSwitchOption(settings_layout *Layout, const char *Name, bool *Value, window *Win, int MouseX, int MouseY, int LmbPressed)
{
    settings_reserve* Rsrv = (settings_reserve*)Win->Reserved;
    int X = Layout->X, Y = Layout->Y, W = Win->Rect->W - Layout->Padding * 2, H = 16;
    if (MouseX > X && MouseY > Y &&
        MouseX < (X + W) && MouseY < (Y + H) && LmbPressed)
    {
        *Value = !*Value;
    }

    DrawTextOnto(Layout->X, Layout->Y, Name, Settings.LightTheme ? 0 : 0xFFFFFFFF, Rsrv->BackBuff, Win->Rect->W, Win->Rect->H);

    Layout->X = Win->Rect->W - Layout->Padding;
    Layout->X -= 40;

    DrawSwitch(Layout->X, Layout->Y, *Value, Win);

    Layout->X = Layout->Padding;
    Layout->Y += 16 + Layout->Padding;
}


extern int MouseX;
extern int MouseY;

void SettingsWindowProc(window *Win)
{
    settings_reserve* Rsrv = (settings_reserve*)Win->Reserved;
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
            // key pressed...
        }
        I++;
    }
    Win->ChQueueNum = 0;

    DrawSwitchOption(&Layout, "Light Theme", &Settings.LightTheme, Win, RelMouseX, RelMouseY, MousePressed);

    memcpy(Win->Framebuffer, Rsrv->BackBuff, SETTINGS_RES_X * SETTINGS_RES_Y * 4);
}