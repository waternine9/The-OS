#ifndef H_BOS_CMD
#define H_BOS_CMD
#include <stdint.h>
#define CONSOLE_RES_X VESA_RES_X
#define CONSOLE_RES_Y VESA_RES_Y / 4

extern uint32_t CmdDrawBuffer[1920 * 1080];

void InitCMD();
void CmdAddChar(char thechar);
void CmdBackspace();
void CmdClear();
void CmdDraw(uint32_t color);

#endif