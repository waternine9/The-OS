#include <stdint.h>
#include "console.h"

#include "kernel_constants.h"

// NOTE: Scrolls 1 up
static void ConScroll(console *Console)
{
  for (uint32_t i = 0; i < CONSOLE_MAX_LINES-1; i++) {
    for (uint32_t j = 0; j < CONSOLE_MAX_LINE_LENGTH; j++) {
      Console->Lines[i][j] = Console->Lines[i+1][j];
    }
  }

  if (Console->Line > 0) {
    Console->Line -= 1;
  }
  Console->Lines[CONSOLE_MAX_LINES-1][0] = 0;
}
void ConsoleWrite(console *Console, const char *Text)
{
  for (int I = 0; Text[I]; I++) {
    if (Text[I] != '\n' && Text[I] != ESC_PS2_KEYCODE && Text[I] != LEFT_CONTROL_PS2_KEYCODE && Text[I] != LEFT_SHIFT_PS2_KEYCODE && Text[I] != LEFT_ALT_PS2_KEYCODE && Text[I] != RIGHT_SHIFT_PS2_KEYCODE && Text[I] != TAB_PS2_KEYCODE) {
      Console->Lines[Console->Line][Console->Col] = Text[I];
    }
    switch (Text[I]) {
      case '\n':
        Console->Line += 1;
        Console->Col = 0;
        break;
      case ESC_PS2_KEYCODE:
        break;
      case LEFT_CONTROL_PS2_KEYCODE:
        break;
      case LEFT_SHIFT_PS2_KEYCODE:
        break;
      case LEFT_ALT_PS2_KEYCODE:
        break;
      case RIGHT_SHIFT_PS2_KEYCODE:
        break;
      case TAB_PS2_KEYCODE:
        break;
      default:
        Console->Col += 1;
        break;
    }
    if (Console->Col >= CONSOLE_MAX_LINE_LENGTH) {
      Console->Line++;
      Console->Col = 0;
    } 
    while (Console->Line >= CONSOLE_MAX_LINES) {
      ConScroll(Console);
    }
  }

  Console->Lines[Console->Line][Console->Col] = 0;
}
void ConsoleClear(console *Console)
{
  Console->Line = 0;
  Console->Col = 0;
  for (uint32_t i = 0; i < CONSOLE_MAX_LINES; i++) {
    for (uint32_t j = 0; j < CONSOLE_MAX_LINE_LENGTH; j++) {
      Console->Lines[i][j] = 0;
    }
  }
}

