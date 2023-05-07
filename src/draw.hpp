#ifndef H_DRAW
#define H_DRAW

#include <stdint.h>
#include <stddef.h>
#include "string.hpp"

#define BLACK_BG 0
#define BLUE_BG 1 << 4
#define GREEN_BG 2 << 4
#define CYAN_BG 3 << 4
#define RED_BG 4 << 4
#define MAGENTA_BG 5 << 4
#define BROWN_BG 6 << 4
#define LIGHT_GRAY_BG 7 << 4
#define DARK_GRAY_BG 8 << 4
#define LIGHT_BLUE_BG 9 << 4
#define LIGHT_GREEN_BG 10 << 4
#define LIGHT_CYAN_BG 11 << 4
#define LIGHT_RED_BG 12 << 4
#define LIGHT_MAGENTA_BG 13 << 4
#define YELLOW_BG 14 << 4
#define WHITE_BG 15 << 4

#define BLACK_FG 0
#define BLUE_FG 1
#define GREEN_FG 2
#define CYAN_FG 3
#define RED_FG 4
#define MAGENTA_FG 5
#define BROWN_FG 6
#define LIGHT_GRAY_FG 7
#define DARK_GRAY_FG 8
#define LIGHT_BLUE_FG 9
#define LIGHT_GREEN_FG 10
#define LIGHT_CYAN_FG 11
#define LIGHT_RED_FG 12
#define LIGHT_MAGENTA_FG 13
#define YELLOW_FG 14
#define WHITE_FG 15

void DrawCharacter(uint8_t c, size_t x, size_t y, uint8_t color);
void DrawString(String s, size_t x, size_t y, uint8_t color);

#endif