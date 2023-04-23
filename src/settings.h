#ifndef H_TOS_SETTINGSW
#define H_TOS_SETTINGSW

#include <stdint.h>
#include <stdbool.h>

#include "OS.h"

extern uint32_t SettingsDrawBuffer[640 * 480];

typedef struct {
    bool LightTheme;
} user_settings;

typedef struct {
    char *DrawBuffer;
    user_settings Settings;
    window Window;
    rect WindowRect;
    int MouseX, MouseY;
    bool MousePressed;
} settings_window;

void SettingsWindowInit(rect *Rect);
void SettingsWindowProc(int MouseX, int MouseY, window* Win);

#endif