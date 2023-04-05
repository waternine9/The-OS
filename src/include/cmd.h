#ifndef H_BOS_CMD
#define H_BOS_CMD
#include <stdint.h>

#define CONSOLE_RES_X 300
#define CONSOLE_RES_Y 200

extern uint32_t CmdDrawBuffer[CONSOLE_RES_X * CONSOLE_RES_Y];

void InitCMD();
void CmdAddChar(char thechar);
void CmdClear();
void CmdDraw(uint32_t color);

#endif