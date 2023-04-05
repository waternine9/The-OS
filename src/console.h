#ifndef H_BOS_CONSOLE
#define H_BOS_CONSOLE
#define CONSOLE_MAX_LINE_LENGTH 77
#define CONSOLE_MAX_LINES 37

typedef struct {
    char Lines[CONSOLE_MAX_LINES][CONSOLE_MAX_LINE_LENGTH + 1];
    int Col, Line;
} console;

void ConsoleWrite(console *Console, const char *Text);
void ConsoleClear(console *Console);

#endif // H_BOS_CONSOLE
