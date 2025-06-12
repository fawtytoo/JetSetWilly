#include "misc.h"
#include "audio.h"
#include "video.h"

#include "game.h"

static int      gameMusic = MUS_PLAY;
static int      gameLevelDir;

static char     *levelName[] =
{
    "The Off Licence",
    "The Bridge",
    "Under the MegaTree",
    "At the Foot of the MegaTree",
    "The Drive",
    "The Security Guard",
    "Entrance to Hades",
    "Cuckoo's Nest",
    "Inside the MegaTrunk",
    "On a Branch Over the Drive",
    "The Front Door",
    "The Hall",
    "Tree Top",
    "Out on a limb",
    "Rescue Esmerelda",
    "I'm sure I've seen this before..",
    "We must perform a Quirkafleeg",
    "Up on the Battlements",
    "On the Roof",
    "The Forgotten Abbey",
    "Ballroom East",
    "Ballroom West",
    "To the Kitchens                                Main Stairway",
    "The Kitchen",
    "West of Kitchen",
    "Cold Store",
    "East Wall Base",
    "The Chapel",
    "First Landing",
    "The Nightmare Room",
    "The Banyan Tree",
    "Swimming Pool",
    "Halfway up the East Wall",
    "The Bathroom",
    "Top Landing",
    "Master Bedroom",
    "A bit of tree",
    "Orangery",
    "Priests' Hole",
    "Emergency Generator",
    "Dr Jones will never believe this",
    "The Attic",
    "Under the Roof",
    "Conservatory Roof",
    "On top of the house",
    "Under the Drive",
    "Tree Root",
    "Nomen Luni",
    "The Wine Cellar",
    "Watch Tower",
    "Tool Shed",
    "Back Stairway",
    "Back Door",
    "West Wing",
    "West Bedroom",
    "West Wing Roof",
    "Above the West Bedroom",
    "The Beach",
    "The Yacht",
    "The Bow"
};

static int      levelBorder[] =
{
    5, 4, 6, 2, 3, 1, 2, 1, 4, 2,
    2, 4, 6, 5, 1, 3, 2, 1, 2, 1,
    2, 1, 4, 4, 1, 1, 5, 2, 3, 2,
    2, 2, 2, 2, 1, 1, 5, 6, 2, 2,
    1, 1, 2, 5, 3, 4, 1, 2, 4, 5,
    5, 2, 1, 2, 5, 1, 2, 2, 5, 5
};

static char     gameScoreItems;
static char     gameScoreClock[3];
static EVENT    DoClockUpdate;

static int      gameRoom[][4] =
{
    {0, 0, 0, 1}, {0, 0, 0, 2}, {0, 1, 0, 3}, {8, 2, 0, 4}, {6, 3, 45, 5}, {10, 4, 6, 19}, {0, 5, 0, 14}, {0, 0, 2, 8}, {12, 7, 3, 9}, {13, 8, 4, 10},
    {0, 9, 5, 11}, {0, 10, 0, 20}, {0, 0, 8, 13}, {0, 12, 9, 10}, {20, 44, 39, 15}, {0, 14, 0, 16}, {49, 15, 0, 17}, {0, 16, 0, 18}, {18, 17, 0, 47}, {0, 5, 0, 48},
    {26, 11, 0, 21}, {27, 20, 0, 22}, {28, 21, 0, 23}, {0, 22, 0, 24}, {30, 23, 0, 25}, {31, 24, 0, 51}, {32, 0, 20, 27}, {33, 26, 21, 28}, {34, 27, 22, 29}, {0, 28, 0, 30},
    {36, 29, 24, 31}, {37, 30, 0, 53}, {38, 0, 26, 33}, {0, 0, 0, 34} /* the bathroom */, {40, 33, 28, 35}, {41, 34, 29, 36}, {42, 35, 30, 37}, {43, 36, 31, 55}, {59, 0, 32, 39}, {14, 38, 0, 40},
    {16, 39, 0, 41}, {0, 40, 34, 42}, {47, 41, 36, 43}, {0, 42, 37, 0}, {0, 0, 38, 14}, {4, 46, 0, 6}, {3, 47, 0, 45}, {0, 18, 42, 0}, {51, 19, 0, 50}, {0, 0, 16, 0},
    {52, 48, 0, 57}, {53, 25, 48, 52}, {54, 51, 50, 0}, {55, 31, 51, 54}, {56, 53, 52, 0}, {0, 37, 53, 56}, {0, 55, 54, 0}, {57, 50, 0, 58}, {0, 57, 0, 59}, {0, 58, 0, 0}
};

static int      gameInactivityTimer;

static u8       lifeInk[] = {0x2, 0x4, 0x6, 0x1, 0x3, 0x5, 0x7};

static int      gameFrame;
static TIMER    gameTimer;

int             gamePaused = 0;
int             gameLevel;
int             gameLives;
int             gameClockTicks;
int             gameMode;

int             itemCount;

static void DoDrawClock()
{
    char    text[24] = "\x1\x0\x2\x7" " " "\x2\x6" " " "\x2\x5" ":" "\x2\x4" " " "\x2\x3" " " "\x2\x2" " " "\x2\x1" "m";

    text[19] = gameScoreClock[2] ? 'p' : 'a';
    text[16] = (gameScoreClock[0] % 10) + '0';
    text[13] = (gameScoreClock[0] / 10) + '0';
    text[7] = (gameScoreClock[1] % 10) + '0';
    if (gameScoreClock[1] > 9)
    {
        text[4] = (gameScoreClock[1] / 10) + '0';
    }

    Video_WriteLarge(WIDTH - 60, STATUS, text);

    DoClockUpdate = DoNothing;
}

static void DrawItems()
{
    char    text[9] = "\x1\x0\x2\x6" " " "\x2\x7" " ";

    text[7] = (gameScoreItems % 10) + '0';
    if (gameScoreItems > 9)
    {
        text[4] = (gameScoreItems / 10) + '0';
    }

    Video_WriteLarge(6 * 8 + 4, STATUS, text);
}

static void GameDrawLives()
{
    int l;

    for (l = 0; l < gameLives; l++)
    {
        Miner_DrawSeqSprite(LIVES + l * 16, 0x0, lifeInk[l]);
    }
}

void Game_DrawStatus()
{
    Video_PixelPaperFill(128 * WIDTH, 64 * WIDTH, 0x0);
    Video_PixelInkFill(129 * WIDTH, 8 * WIDTH, 0x6);

    Video_WriteLarge(4, STATUS, "\x1\x0\x2\x1" "I" "\x2\x2" "t" "\x2\x3" "e" "\x2\x4" "m" "\x2\x5" "s");
    DrawItems();

    DoDrawClock();

    GameDrawLives();
}

void Game_ChangeLevel(int dir)
{
    if (dir == R_ABOVE)
    {
        // this fixes jumping up from the ramp in "Under the Drive"
        //  and appearing inside the floor in "The Drive"
        // also applies to "First Landing"
        if ((gameRoom[gameLevel][dir] == THEDRIVE && minerWilly.x > 22) || (gameRoom[gameLevel][dir] == FIRSTLANDING && minerWilly.x > 182))
        {
            minerWilly.air = 2;
            return; // we're not changing rooms
        }
    }

    gameLevel = gameRoom[gameLevel][dir];

    switch (dir)
    {
      case R_ABOVE:
        minerWilly.dy = minerWilly.y = 13 * 8;
        minerWilly.tile = 13 * 32 + (minerWilly.tile & 31);
        minerWilly.align = 4;
        minerWilly.air = 0;
        minerWilly.rope = 0;
        break;

      case R_RIGHT:
        minerWilly.x = 0;
        minerWilly.tile &= ~31;
        break;

      case R_BELOW:
        if (minerWilly.air < 11)
        {
            minerWilly.air = 2;
        }

        minerWilly.dy = minerWilly.y = 0;
        minerWilly.tile &= 31;
        break;

      case R_LEFT:
        minerWilly.x = 30 * 8;
        minerWilly.tile |= 30;
        break;
    }

    Game_InitRoom();
}

static void ClockTicker()
{
    // 256 frames = 1 game minute
    // 19 game hours = 6.75... actual hours (19 * 60 * 256 / 12 / 60 / 60)
    // there's a guy on YouTube that can do it in less than 20m
    //  (2m15s using cheat mode)
    if (gameClockTicks++ < 256)
    {
        return;
    }

    gameClockTicks = 0;

    gameScoreClock[0]++;
    if (gameScoreClock[0] == 60)
    {
        gameScoreClock[0] = 0;
        gameScoreClock[1]++;
        if (gameScoreClock[1] == 12)
        {
            gameScoreClock[2] = 1 - gameScoreClock[2];
            if (gameScoreClock[2] == 0 && gameMode < GM_MARIA)
            {
                Action = Gameover_Action;
            }
        }
        else if (gameScoreClock[1] == 13)
        {
            gameScoreClock[1] = 1;
        }
    }

    DoClockUpdate = DoDrawClock;
}


void Game_GotItem()
{
    gameScoreItems++;
    DrawItems();

    if (--itemCount == 0)
    {
        gameMode = GM_MARIA;
    }

    audioPanX = minerWilly.x;
    Audio_Sfx(SFX_ITEM);
}

static void DoPauseDrawer()
{
    if (gamePaused == 16 * 5)
    {
        Video_CycleColours(1);
    }
}

static void DoPauseTicker()
{
    if (gamePaused++ == 16 * 5)
    {
        gamePaused = 1;
    }
}

static void DoGameDrawer()
{
    if (gameMusic == MUS_PLAY)
    {
        GameDrawLives();
    }

    if (gameFrame == 0)
    {
        return;
    }

    Level_Drawer();
    Robots_Drawer();

    if (gameMode == GM_TOILET)
    {
        return;
    }

    Miner_Drawer();
    Rope_Drawer();

    DoClockUpdate();
}

static void DoGameTicker()
{
    if (gameMusic == MUS_PLAY)
    {
        Miner_IncSeq();
    }

    gameFrame = Timer_Update(&gameTimer);
    if (gameFrame == 0)
    {
        return;
    }

    gameInactivityTimer++;

    Level_Ticker();
    Robots_Ticker();

    if (gameMode == GM_TOILET)
    {
        if (gameClockTicks++ == 256)
        {
            Action = Title_Action;
        }

        return;
    }

    Miner_Ticker();

    if (gameMode == GM_RUNNING)
    {
        minerWilly.frame |= 1;

        if (minerWilly.x == 224 && gameLevel == THEBATHROOM)
        {
            gameMode = GM_TOILET;
            Robots_Flush();
            gameClockTicks = 0;
        }

        return;
    }

    if (gameMode == GM_MARIA && gameLevel == MASTERBEDROOM)
    {
        if (minerWilly.air == 0 && minerWilly.x == 40)
        {
            gameMode = GM_RUNNING;
        }
    }

    Rope_Ticker();

    ClockTicker();
}

void Game_Pause(int state)
{
    if (gameMode >= GM_RUNNING)
    {
        return;
    }

    gamePaused = state;

    if (gamePaused)
    {
        if (cheatEnabled)
        {
            Ticker = DoNothing;
            Drawer = DoNothing;
        }
        else
        {
            Ticker = DoPauseTicker;
            Drawer = DoPauseDrawer;
        }
        Audio_Play(MUS_STOP);
    }
    else
    {
        Ticker = DoGameTicker;
        Drawer = DoGameDrawer;
        Audio_Play(gameMusic);

        gameInactivityTimer = 0;
        if (cheatEnabled == 0)
        {
            Game_DrawStatus();
            System_Border(levelBorder[gameLevel]);
        }
    }
}

static void DoGameResponder()
{
    gameInactivityTimer = 0;

    if (gameInput == KEY_PAUSE || gamePaused)
    {
        Game_Pause(gamePaused ? 0 : 1);
    }

    if (gameInput == KEY_MUTE)
    {
        gameMusic = gameMusic == MUS_PLAY ? MUS_STOP : MUS_PLAY;
        Audio_Play(gameMusic);
    }
    else if (gameInput == KEY_ESCAPE)
    {
        Action = Title_Action;
    }
    else
    {
        Cheat_Responder();
    }
}

static void DoGameAction()
{
    if (gameInactivityTimer == 256)
    {
        if (gameMusic == MUS_STOP && gameMode < GM_RUNNING)
        {
            Game_Pause(1);

            gameInactivityTimer++;
        }
    }
}

void Game_InitRoom()
{
    Level_Init();
    Robots_Init();
    Rope_Init();
    Video_PixelFill(129 * WIDTH, 8 * WIDTH);
    Video_Write(0, "\x1\x0\x2\x6");
    Video_Write((16 * 8 + 1) * WIDTH + (WIDTH - Video_TextWidth(levelName[gameLevel])) / 2, levelName[gameLevel]);
    System_Border(levelBorder[gameLevel]);
    Miner_Save();

    minerAttrSplit = 6;
    if (gameLevel == SWIMMINGPOOL)
    {
        minerAttrSplit = 5; // willy goes blue when underwater
    }

    Timer_Set(&gameTimer, 12, TICKRATE);
    gameFrame = 1;
    gameInactivityTimer = 0;

    Ticker = DoGameTicker;

    Action = DoGameAction;
}

void Game_GameReset()
{
    gameScoreItems = 0;

    gameScoreClock[0] = 0;
    gameScoreClock[1] = 7;
    gameScoreClock[2] = 0; // am
    DoClockUpdate = DoDrawClock;

    gameClockTicks = 0;

    gamePaused = 0;

    Miner_SetSeq(0, 20);
    gameLives = 7;

    Audio_Music(MUS_GAME, gameMusic);
}

void Game_Action()
{
    Responder = DoGameResponder;
    Ticker = Game_InitRoom;
    Drawer = DoGameDrawer;
}
