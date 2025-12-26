#include "common.h"
#include "video.h"
#include "game.h"

#define ROPE_SEGS   33

typedef struct
{
    int     x, y;
}
DATA;

static DATA     ropeData[86] =
{
    {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {2, 2}, {2, 2}, {2, 3}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {1, 2}, {2, 2}, {2, 2}, {1, 2}, {1, 2}, {2, 2}, {1, 2}, {1, 2}, {2, 2}, {2, 2}, {3, 2}, {2, 2}, {3, 2}, {2, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}
};

static int      ropeMove[2] = {-1, 1};

static int      ropeDir, ropePos;
static int      ropeX, ropeTop;
static u8       ropeInk;

EVENT           Rope_Ticker, Rope_Drawer;

static void DoRopeDrawer()
{
    int     x, y, pos;
    int     seg;
    DATA    *data = &ropeData[ropePos < 0 ? -ropePos : ropePos];

    x = ropeX * 8;
    y = 0;

    Video_DrawRopeSeg(x, ropeInk);

    for (seg = 1; seg < ROPE_SEGS; seg++, data++)
    {
        y += data->y;

        if (ropePos > 0)
        {
            x += data->x;
        }
        else if (ropePos < 0)
        {
            x -= data->x;
        }

        pos = y * WIDTH + x;
        if (minerWillyRope == 0 && (Video_GetPixel(pos) & B_WILLY))
        {
            minerWillyRope = seg;
        }

        Video_DrawRopeSeg(pos, ropeInk);

        if (minerWillyRope == seg)
        {
            minerWilly.x = x & 248;
            minerWilly.y = y - 8;

            if (x & 1)
            {
                minerWilly.frame = 1;
            }
            else if (x & 2)
            {
                minerWilly.frame = 0;
            }
            else
            {
                minerWilly.x -= 8;
                if (x & 4)
                {
                    minerWilly.frame = 3;
                }
                else
                {
                    minerWilly.frame = 2;
                }
            }
            minerWilly.frame |= (minerWilly.dir << 2);

            minerWilly.tile = minerWilly.y / 8 * 32 + x / 8;
            minerWilly.align = YALIGN(y); // y before deduction
        }
    }

    if (minerWillyRope < 0) // this allows Willy to jump off a rope
    {
        if (minerWillyRope-- == -16) // for 16 frames
        {
            minerWillyRope = 0;
        }

        return;
    }

    if (minerWilly.move == 0)
    {
        return;
    }

    seg = minerWillyRope + ropeMove[ropeDir ^ minerWilly.dir];

    if (seg < ropeTop)
    {
        return;
    }

    if (seg < ROPE_SEGS)
    {
        minerWillyRope = seg;
        return;
    }

    minerWillyRope = -1;
    minerWilly.y &= 124;
    minerWilly.air = 0;
}

static void DoRopeTicker()
{
    ropePos += ropeMove[ropeDir] * 2;
    if (ropePos > -18 && ropePos < 18)
    {
        ropePos += ropeMove[ropeDir] * 2;
    }
    else if (ropePos == 54 || ropePos == -54)
    {
        ropeDir ^= 1;
    }
}

void Rope_Init()
{
    if (gameLevel == QUIRKAFLEEG)
    {
        ropeX = 16;
        ropeTop = 2;
        ropeInk = 6;
    }
    else if (gameLevel == ONTHEROOF)
    {
        ropeX = 16;
        ropeTop = 15;
        ropeInk = 4;
    }
    else if (gameLevel == COLDSTORE)
    {
        ropeX = 16;
        ropeTop = 2;
        ropeInk = 6;
    }
    else if (gameLevel == SWIMMINGPOOL)
    {
        ropeX = 16;
        ropeTop = 2;
        ropeInk = 7;
    }
    else if (gameLevel == THEBEACH)
    {
        ropeX = 14;
        ropeTop = 15;
        ropeInk = 5;
    }
    else
    {
        Rope_Ticker = DoNothing;
        Rope_Drawer = DoNothing;
        return;
    }

    // all ropes start the same way
    ropeDir = 0;
    ropePos = 34;

    Rope_Ticker = DoRopeTicker;
    Rope_Drawer = DoRopeDrawer;
}
