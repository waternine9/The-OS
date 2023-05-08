#include "term.hpp"
#include "draw.hpp"
#include "mutex.hpp"

#define LEVEL_NORMAL 0
#define LEVEL_WARNING 1
#define LEVEL_ERROR 2

struct TermLine
{
    uint8_t level;
    String str;
};

mutex PushMutex;

LinkedList<TermLine> termStorage = LinkedList<TermLine>();

String* commandStr = NULL;


void TermPush(String str, uint8_t level)
{
    MutexLock(&PushMutex);
    TermLine l;
    l.level = level;
    l.str = str;
    termStorage.PushBack(l);
    MutexRelease(&PushMutex);
}


namespace Terminal
{
    void PushNormal(String str)
    {
        TermPush(str, LEVEL_NORMAL);
    }

    void PushError(String str)
    {
        TermPush(str, LEVEL_ERROR);
    }

    void PushWarning(String str)
    {
        TermPush(str, LEVEL_WARNING);
    }

    void AttachCmdStr(String *str)
    {
        commandStr = str;
    }

    void Render()
    {
        Draw::Clear('_', DARK_GRAY_FG);
        int y = 1 + Draw::DrawString(*commandStr, 0, 0, LIGHT_GRAY_FG);;
        for (int i = termStorage.size - 1;i >= 0 && y < 25;i--)
        {
            uint8_t color;

            switch (termStorage[i].level)
            {
                case LEVEL_NORMAL:
                    color = WHITE_FG;
                    break;
                case LEVEL_WARNING:
                    color = YELLOW_FG;
                    break;
                case LEVEL_ERROR:
                    color = RED_FG;
                    break;
            }

            y += Draw::DrawString(termStorage[i].str, 0, y, color) + 1;
        }
        Draw::SwapBuffers();
    }
}