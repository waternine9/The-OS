#ifndef H_TOS_SETTINGSW
#define H_TOS_SETTINGSW

#include <stdint.h>
#include <stdbool.h>

#include "OS.h"

#define SETTINGS_RES_X 640
#define SETTINGS_RES_Y 480

extern uint32_t SettingsDrawBuffer[640 * 480];

typedef struct {
    bool LightTheme;
} user_settings;

void SettingsCreateWindow(int x, int y);
void SettingsWindowProc(window* Win);

#endif