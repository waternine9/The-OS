#ifndef H_TOS_CMD
#define H_TOS_CMD
#include <stdint.h>
#include "OS.h"
#define CONSOLE_RES_X VESA_RES_X / 4
#define CONSOLE_RES_Y VESA_RES_Y / 2

window* CmdCreateWindow(int X, int Y);
void CmdBackspace(window*);
void CmdAddChar(uint8_t, window*);
void CmdProc(window* Win);

#endif
