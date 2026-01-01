#include "misc.h"
#include "video.h"
#include "game.h"
#include "audio.h"

#define D_RIGHT 0
#define D_LEFT  1
#define D_JUMP  2

typedef struct
{
    int jump;
    int tile;
    int align;
    // sfx
    int length;
    int pitch;
}
JUMP;

static u16      minerSprite[16][16] =
{
    {15360, 15360, 32256, 13312, 15872, 15360, 6144, 15360, 32256, 32256, 63232, 64256, 15360, 30208, 28160, 30464},
    {3840, 3840, 8064, 3328, 3968, 3840, 1536, 3840, 7040, 7040, 7040, 7552, 3840, 1536, 1536, 1792},
    {960, 960, 2016, 832, 992, 960, 384, 960, 2016, 2016, 3952, 4016, 960, 1888, 1760, 1904},
    {240, 240, 504, 208, 248, 240, 96, 240, 504, 1020, 2046, 1782, 248, 474, 782, 908},
    {3840, 3840, 8064, 2816, 7936, 3840, 1536, 3840, 8064, 16320, 32736, 28512, 7936, 23424, 28864, 12736},
    {960, 960, 2016, 704, 1984, 960, 384, 960, 2016, 2016, 3824, 3568, 960, 1760, 1888, 3808},
    {240, 240, 504, 176, 496, 240, 96, 240, 472, 472, 472, 440, 240, 96, 96, 224},
    {60, 60, 126, 44, 124, 60, 24, 60, 126, 126, 239, 223, 60, 110, 118, 238},
    {32768, 20480, 43008, 20480, 43008, 54528, 27136, 55040, 43648, 55232, 65472, 32256, 17408, 17408, 0, 0},
    {0, 0, 0, 0, 0, 11328, 7808, 16320, 10912, 22000, 11248, 24448, 4352, 8320, 0, 0},
    {0, 0, 0, 0, 0, 2832, 1952, 4080, 3432, 2748, 5500, 2784, 5440, 10816, 5120, 10240},
    {0, 0, 0, 0, 0, 712, 488, 1020, 682, 1375, 703, 1528, 272, 160, 0, 0},
    {0, 0, 0, 0, 0, 4928, 6016, 16320, 21824, 64160, 64832, 8096, 2176, 1280, 0, 0},
    {0, 0, 0, 0, 0, 2256, 1504, 4080, 5808, 15696, 16040, 1872, 680, 596, 40, 20},
    {0, 0, 0, 0, 0, 564, 376, 1020, 1364, 4010, 4052, 506, 136, 260, 0, 0},
    {3, 10, 21, 10, 21, 171, 86, 235, 341, 1003, 1023, 126, 34, 34, 0, 0}
};

static JUMP     jumpInfo[] =
{
    {-4, -32, 6, 5, 72},
    {-4, 0, 4, 5, 74},
    {-3, -32, 6, 4, 76},
    {-3, 0, 6, 4, 78},
    {-2, 0, 4, 3, 80},
    {-2, -32, 6, 3, 82},
    {-1, 0, 6, 2, 84},
    {-1, 0, 6, 2, 86},
    {0, 0, 6, 1, 88},
    {0, 0, 6, 1, 88},
    {1, 0, 6, 2, 86},
    {1, 0, 6, 2, 84},
    {2, 32, 4, 3, 82},
    {2, 0, 6, 3, 80},
    {3, 0, 6, 4, 78},
    {3, 32, 4, 4, 76},
    {4, 0, 6, 5, 74},
    {4, 32, 4, 5, 72}
};

static MINER    minerStore;
static u16      (*minerFrame)[16];

static u8       minerSeqIndex;
static TIMER    minerTimer;

MINER           minerWilly = {.frame = 0, .dir = D_RIGHT};
int             minerWillyRope;
int             minerAttrSplit;

static int      minerSequence[8] = {0, 1, 2, 3, 7, 6, 5, 4};

void Miner_SetSeq(int index, int speed)
{
    Timer_Set(&minerTimer, 1, speed);
    minerSeqIndex = index;
}

void Miner_IncSeq()
{
    minerSeqIndex += Timer_Update(&minerTimer);
    minerSeqIndex &= 7;
}

void Miner_DrawSeqSprite(int pos, u8 paper, u8 ink)
{
    Video_DrawSprite(pos, minerSprite[minerSequence[minerSeqIndex]], paper, ink);
}

void Miner_Restore()
{
    minerWilly.x = minerStore.x;
    minerWilly.y = minerStore.y;
    minerWilly.tile = minerStore.tile;
    minerWilly.align = minerStore.align;
    minerWilly.frame = minerStore.frame;
    minerWilly.dir = minerStore.dir;
    minerWilly.move = minerStore.move;
    minerWilly.air = minerStore.air;
    minerWilly.jump = minerStore.jump;
}

void Miner_Save()
{
    minerStore.x = minerWilly.x;
    minerStore.y = minerWilly.y;
    minerStore.tile = minerWilly.tile;
    minerStore.align = minerWilly.align;
    minerStore.frame = minerWilly.frame;
    minerStore.dir = minerWilly.dir;
    minerStore.move = minerWilly.move;
    minerStore.air = minerWilly.air;
    minerStore.jump = minerWilly.jump;

    minerFrame = &minerSprite[0];
    if (gameLevel == NIGHTMAREROOM)
    {
        minerFrame = &minerSprite[8];
    }
}

static int IsSolid(int tile)
{
    if (tile < 0 || tile == 512)
    {
        return 0;
    }

    if (Level_GetTileType(tile) == T_SOLID)
    {
        return 1;
    }

    if (Level_GetTileType(tile + 32) == T_SOLID)
    {
        return 1;
    }

    if (tile + 64 > 511)
    {
        return 0;
    }

    if (Level_GetTileType(tile + 64) != T_SOLID)
    {
        return 0;
    }

    if (minerWilly.align == 6)
    {
        return 1;
    }

    if (minerWilly.air == 1 && minerWilly.jump > 9)
    {
        minerWilly.air = 0;
    }

    return 0;
}

static void MoveLeftRight()
{
    int y = 0, offset = 0;

    if (minerWilly.move == 0)
    {
        return;
    }

    if (minerWillyRope > 0)
    {
        return;
    }

    if (minerWilly.dir == D_RIGHT)
    {
        if (minerWilly.frame < 3)
        {
            minerWilly.frame++;
            return;
        }

        if (minerWilly.air == 0)
        {
            if (Level_GetTileRamp(minerWilly.tile + 64) == T_RAMPL)
            {
                y = 8;
                offset = 32;
            }
            else if (Level_GetTileRamp(minerWilly.tile + 34) == T_RAMPR)
            {
                y = -8;
                offset = -32;
            }
        }

        if (minerWilly.x == 30 * 8)
        {
            Game_ChangeLevel(R_RIGHT);
            return;
        }

        if (IsSolid(minerWilly.tile + offset + 2))
        {
            return;
        }

        minerWilly.x += 8;
        minerWilly.tile++;
        minerWilly.frame = 0;
    }
    else if (gameMode != GM_RUNNING)
    {
        if (minerWilly.frame > 0)
        {
            minerWilly.frame--;
            return;
        }

        if (minerWilly.air == 0)
        {
            if (Level_GetTileRamp(minerWilly.tile + 31) == T_RAMPL)
            {
                y = -8;
                offset = -32;
            }
            else if (Level_GetTileRamp(minerWilly.tile + 65) == T_RAMPR)
            {
                y = 8;
                offset = 32;
            }
        }

        if (minerWilly.x == 0)
        {
            Game_ChangeLevel(R_LEFT);
            return;
        }

        if (IsSolid(minerWilly.tile + offset - 1))
        {
            return;
        }

        minerWilly.x -= 8;
        minerWilly.tile--;
        minerWilly.frame = 3;
    }

    minerWilly.y += y;
    minerWilly.tile += offset;
}

static void UpdateDir(int conveyDir)
{
    int dir = 0;

    System_UpdateKeys();

    if ((System_IsKeyLeft() || conveyDir == C_LEFT) && gameMode < GM_RUNNING)
    {
        dir += 1;
    }

    if (System_IsKeyRight() || conveyDir == C_RIGHT || gameMode == GM_RUNNING)
    {
        dir += 2;
    }

    if (dir == 0)
    {
        minerWilly.move = 0;
    }
    else if (dir == 1)
    {
        if (minerWilly.dir == D_RIGHT)
        {
            minerWilly.dir = D_LEFT;
            minerWilly.move = 0;
        }
        else
        {
            minerWilly.move = 1;
        }
    }
    else if (dir == 2)
    {
        if (minerWilly.dir == D_LEFT)
        {
            minerWilly.dir = D_RIGHT;
            minerWilly.move = 0;
        }
        else
        {
            minerWilly.move = 1;
        }
    }

    if (System_IsKeyJump() && gameMode < GM_RUNNING)
    {
        minerWilly.air = 1;
        minerWilly.jump = 0;
        if (minerWillyRope > 0)
        {
            minerWillyRope = -16;
            minerWilly.y &= 120;
            minerWilly.align = 4;
            minerWilly.move = 1;
            return;
        }
    }
}

static void DoMinerTicker()
{
    int tile, type[2], y;
    int conveyDir = C_NONE;

    if (minerWillyRope > 0)
    {
        UpdateDir(conveyDir);
        return;
    }

    if (minerWilly.air == 1)
    {
        y = minerWilly.y + jumpInfo[minerWilly.jump].jump;

        if (y < 0)
        {
            Game_ChangeLevel(R_ABOVE);
            return;
        }

        tile = minerWilly.tile + jumpInfo[minerWilly.jump].tile;
        if (Level_GetTileType(tile) == T_SOLID || Level_GetTileType(tile + 1) == T_SOLID)
        {
            // we need to re-align Willy
            minerWilly.y = (y + 8) & 120;
            minerWilly.tile = tile + 32;
            minerWilly.align = 4;

            minerWilly.air = 2;
            minerWilly.move = 0;
            return;
        }

        audioPanX = minerWilly.x;
        Audio_WillySfx(jumpInfo[minerWilly.jump].pitch, jumpInfo[minerWilly.jump].length);

        minerWilly.y = y;
        minerWilly.tile = tile;
        minerWilly.align = jumpInfo[minerWilly.jump].align;
        minerWilly.jump++;

        if (minerWilly.jump == 18)
        {
            minerWilly.air = 6;
            return;
        }

        if (minerWilly.jump != 13 && minerWilly.jump != 16)
        {
            MoveLeftRight();
            return;
        }
    }

    if (minerWilly.align == 4)
    {
        tile = minerWilly.tile + 64;
        if (tile & 512)
        {
            Game_ChangeLevel(R_BELOW);
            return;
        }

        type[0] = Level_GetTileType(tile);
        type[1] = Level_GetTileType(tile + 1);
        if (type[0] == T_HARM || type[1] == T_HARM)
        {
            if (minerWilly.air == 1 && (type[0] <= T_SPACE || type[1] <= T_SPACE))
            {
                MoveLeftRight();
            }
            else
            {
                Action = Die_Action;
            }
            return;
        }

        if (type[0] > T_SPACE || type[1] > T_SPACE)
        {
            if (minerWilly.air >= 12)
            {
                Action = Die_Action;
                return;
            }

            minerWilly.air = 0;

            if (type[0] == T_CONVEYL || type[1] == T_CONVEYL)
            {
                conveyDir = C_LEFT;
            }
            else if (type[0] == T_CONVEYR || type[1] == T_CONVEYR)
            {
                conveyDir = C_RIGHT;
            }

            UpdateDir(conveyDir);
            MoveLeftRight();
            return;
        }
    }

    if (minerWilly.air == 1)
    {
        MoveLeftRight();
        return;
    }

    minerWilly.move = 0;
    if (minerWilly.air == 0)
    {
        minerWilly.air = 2;
        return;
    }

    minerWilly.air++;
    if (minerWilly.air == 16) // this affects the falling sound effect
    {
        minerWilly.air = 12;
    }

    audioPanX = minerWilly.x;
    Audio_WillySfx(78 - minerWilly.air, 4);
    minerWilly.y += 4;
    minerWilly.align = 4;
    if (minerWilly.y & 7)
    {
        minerWilly.align += 2;
    }
    else
    {
        minerWilly.tile += 32;
    }
}

void Miner_Ticker()
{
    DoMinerTicker();

    if (minerWilly.y < 0)
    {
        Game_ChangeLevel(R_ABOVE);
    }
}

void Miner_Drawer()
{
    int     tile, adj, offset = 0, align;
    int     i;

    align = minerWilly.align;

    if (minerWilly.air == 0)
    {
        if (Level_GetTileRamp(minerWilly.tile + 64) == T_RAMPL)
        {
            offset = minerWilly.frame << 1;
            align = YALIGN(offset);
        }
        else if (Level_GetTileRamp(minerWilly.tile + 65) == T_RAMPR)
        {
            offset = 6 - (minerWilly.frame << 1);
            align = YALIGN(offset);
        }
    }

    Video_DrawMiner(((minerWilly.y + offset) << 8) | minerWilly.x, minerFrame[(minerWilly.dir << 2) | minerWilly.frame], minerAttrSplit);

    tile = minerWilly.tile;
    for (i = 0, adj = 1; i < align; i++, tile += adj, adj ^= 30)
    {
        if (Level_GetTileType(tile) == T_HARM)
        {
            Action = Die_Action;
            return;
        }
    }

    tile = minerWilly.tile;
    for (i = 0, adj = 1; i < align; i++, tile += adj, adj ^= 30)
    {
        if (Level_GetTileType(tile) == T_ITEM)
        {
            Level_EraseItem(tile);
            Game_GotItem();
        }
    }
}

void Miner_Init()
{
    minerWilly.x = 20 * 8;
    minerWilly.y = 13 * 8;
    minerWilly.tile = 13 * 32 + 20;
    minerWilly.align = 4;
    minerWilly.move = 0;
    minerWilly.air = 0;

    Miner_Save();
}
