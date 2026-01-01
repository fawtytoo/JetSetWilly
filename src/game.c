#include "misc.h"
#include "audio.h"
#include "video.h"

#include "game.h"

static int      gameMusic = MUS_PLAY;

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
    int     level = Level_Dir(dir);

    if (dir == R_ABOVE)
    {
        // this fixes jumping up from the ramp in "Under the Drive"
        //  and appearing inside the floor in "The Drive"
        // also applies to "First Landing"
        if ((level == THEDRIVE && minerWilly.x > 22 && minerWilly.x < 32) || (level == FIRSTLANDING && minerWilly.x > 182))
        {
            minerWilly.air = 2;
            return; // we're not changing rooms
        }
    }

    gameLevel = level;

    switch (dir)
    {
      case R_ABOVE:
        minerWilly.dy = minerWilly.y = 13 * 8;
        minerWilly.x = (minerWilly.tile & 31) * 8;
        minerWilly.tile = 13 * 32 + (minerWilly.tile & 31);
        minerWilly.align = 4;
        minerWilly.air = 0;
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

    minerWillyRope = 0;

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
