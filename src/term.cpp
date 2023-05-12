#include "term.hpp"
#include "draw.hpp"
#include "mutex.hpp"

#define LEVEL_NORMAL 0
#define LEVEL_WARNING 1
#define LEVEL_ERROR 2
#define LEVEL_SUCCESS 3

struct TermLine
{
    uint8_t level;
    String str;
};

struct Command
{
    String name;
    void(*callback)(LinkedList<Terminal::CommandArg>);
};

mutex PushMutex;

LinkedList<TermLine> termStorage = LinkedList<TermLine>();
LinkedList<Command*> cmdStorage = LinkedList<Command*>();

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

bool ParseCommand(Command *found, LinkedList<Terminal::CommandArg> *foundArgs)
{
    int j = 0;
    for (int i = 0;i < cmdStorage.size;i++)
    {


        if (StrStartsWith(*commandStr, cmdStorage[i]->name))
        {
            j = cmdStorage[i]->name.size + 1;
            *found = *cmdStorage[i];
        }
    }
    if (j == 0) return false;
    while (j < commandStr->size)
    {



        while ((*commandStr)[j] == ' ') j++;
        Terminal::CommandArg currentArg = Terminal::CommandArg();
        if ((*commandStr)[j] == '-' || ((*commandStr)[j] >= '0' && (*commandStr)[j] <= '9'))
        {
            currentArg.isVal = true;

            int sign = (*commandStr)[j] == '-' ? -1 : 1;
            int val = 0;
            int multiply = 1;

            int k = j;
            while (k != ' ' && k < (*commandStr).size) k++;
            int oldK = k;
            k--;
            while (k >= j)
            {
                if ((*commandStr)[k] != '-')
                {
                    val += (int)((*commandStr)[k] - '0') * multiply;
                    multiply *= 10;
                }
                k--;
            }
            val *= sign;
            currentArg.val = val;
            j = oldK;
        }
        else
        {
            currentArg.isVal = false;
            char stopAt = ' ';
            if ((*commandStr)[j] == '"')
            {
                j++;
                stopAt = '"';
            }
            while ((*commandStr)[j] != stopAt && j < (*commandStr).size)
            {
                currentArg.str.PushBack((*commandStr)[j]);
                j++;
            }
            if (stopAt == '"') j++;
        }
        (*foundArgs).PushBack(currentArg);
    }
    return true;
}

extern int32_t MouseX;
extern int32_t MouseY;

namespace Terminal
{
    void Init()
    {
        cmdStorage = LinkedList<Command*>();
        termStorage = LinkedList<TermLine>();
    }

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

    void PushSuccess(String str)
    {
        TermPush(str, LEVEL_SUCCESS);
    }

    void PushCommand(void(*callback)(LinkedList<Terminal::CommandArg>), String name)
    {
        if (name.size == 0) return;
        Command* cmd = (Command*)kmalloc(sizeof(Command));
        cmd->name = name;
        cmd->callback = callback;
        cmdStorage.PushBack(cmd);
    }

    void RunCommand()
    {
        Command found;
        LinkedList<Terminal::CommandArg> foundArgs;
        if (ParseCommand(&found, &foundArgs))
        {
            found.callback(foundArgs);
        }
        else
        {
            PushError(StrFromCStr("ERR: Command Not Found"));
        }
    }

    void AttachCmdStr(String *str)
    {
        commandStr = str;
    }

    void Render()
    {
        if (MouseY < 0) MouseY = 0;
        Draw::Clear('_', DARK_GRAY_FG);
        int y = 1 + Draw::DrawString(*commandStr, 0, -MouseY, LIGHT_GRAY_FG) - MouseY;
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
                case LEVEL_SUCCESS:
                    color = LIGHT_GREEN_FG;
                    break;
            }
            Draw::DrawCharacter('-', 0, y, LIGHT_CYAN_BG);
            y += Draw::DrawString(termStorage[i].str, 2, y, color) + 1;
        }
        Draw::SwapBuffers();
    }
}