#include "common.h"
#include "video.h"
#include "game.h"
#include "audio.h"

static u16      dieBlank[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static int      dieLevel;

static void Die_Drawer()
{
    Video_PixelInkFill(0, 128 * WIDTH, dieLevel >> 1);
}

static void Die_Ticker()
{
    if (dieLevel-- > 0)
    {
        return;
    }

    if (gameLives < 0)
    {
        Action = Gameover_Action;
        return;
    }

    Video_DrawSprite(LIVES + gameLives * 16, dieBlank, 0x0, 0x0);

    Miner_Restore();

    Audio_ReduceMusicSpeed();

    Action = Game_Action;
}

static void Die_Init()
{
    gameLives--;

    dieLevel = 15;

    System_Border(0x0);
    Video_PixelPaperFill(0, 128 * WIDTH, 0x0);

    audioPanX = minerWilly.x;
    Audio_Sfx(SFX_DIE);

    Ticker = Die_Ticker;
}

void Die_Action()
{
    Responder = DoNothing;
    Ticker = Die_Init;
    Drawer = Die_Drawer;

    Action = DoNothing;
}
