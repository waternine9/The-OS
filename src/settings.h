#ifndef H_TOS_SETTINGSW
#define H_TOS_SETTINGSW

#include <stdint.h>
#include <stdbool.h>

#include "OS.h"

extern uint32_t SettingsDrawBuffer[640 * 480];

typedef struct {
    bool LightTheme;
} user_settings;

void SettingsCreateWindow(int x, int y);
void SettingsWindowProc(window* Win);

#endif