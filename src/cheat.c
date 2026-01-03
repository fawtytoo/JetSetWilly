#include "common.h"
#include "game.h"

static char         *cheatCode = "writetyper";
static int          cheatPos = 0;

int                 cheatEnabled = 0;
EVENT               Cheat_Responder = Cheat_Disabled;

void Cheat_Enabled()
{
    int     level = 0;
    int     i;

    for (i = 0; i < 30; i++)
    {
        if (System_IsKey(KEY_0 + i))
        {
            level = i + 1;
            break;
        }
    }

    if (!System_IsKey(KEY_ENTER))
    {
        Game_Pause(0);
        return;
    }

    if (level == 0)
    {
        return;
    }

    if (System_IsKey(KEY_BACKSPACE))
    {
        level += 30;
    }

    level--;
    if (level == gameLevel)
    {
        return;
    }

    gameLevel = level;
    Ticker = Game_InitRoom; // NOT Action
}

void Cheat_Disabled()
{
    if (gameLevel != FIRSTLANDING || minerWilly.y != 104 || cheatCode[cheatPos] != gameInput - KEY_A + 'a')
    {
        cheatPos = 0;
        Game_Pause(0);
        return;
    }

    cheatPos++;
    if (cheatCode[cheatPos] != '\0')
    {
        return;
    }

    Game_CheatEnabled();

    Cheat_Responder = Cheat_Enabled;
}
