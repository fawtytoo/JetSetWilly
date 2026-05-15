//  Copyright 2021-2026 by Steve Clark

//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.

//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:

//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required. 
//  2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source distribution.

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
