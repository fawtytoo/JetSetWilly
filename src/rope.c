#include "common.h"
#include "video.h"
#include "game.h"

#define ROPE_SEGS   33
#define ROPE_OFFSET 4

typedef struct
{
    u8      x, y;
}
POINT;

typedef struct
{
    POINT   point;
    int     pos;
    int     tile;
    int     align;
}
ROPE;

static POINT    ropeStart[60] =
{
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {16, 3}, {0, 0}, {16, 15}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {16, 3}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {16, 3}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {14, 15}, {0, 0}, {0, 0}
};

static POINT    ropeInc[86] =
{
    {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {0, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {2, 2}, {2, 2}, {2, 3}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {1, 2}, {2, 2}, {2, 2}, {1, 2}, {1, 2}, {2, 2}, {1, 2}, {1, 2}, {2, 2}, {2, 2}, {3, 2}, {2, 2}, {3, 2}, {2, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}
};

static int      ropeDir, ropeFrame;
static u8       ropeInk;

static ROPE     ropeSeg[ROPE_SEGS];

EVENT           Rope_Ticker, Rope_Drawer;

static void DoRopeDrawer()
{
    int seg;

    for (seg = 0; seg < ROPE_SEGS; seg++)
    {
        Video_DrawRopeSeg(ropeSeg[seg].pos, ropeInk);
    }
}

static int Rope_Animate()
{
    int     x, y, seg, frame, hit = 0;
    ROPE    *rope = &ropeSeg[0];
    int     tile = ropeSeg[0].tile;

    x = rope->point.x;
    y = 0;
    frame = ropeFrame < 0 ? -ropeFrame : ropeFrame;
    for (seg = 1; seg < ROPE_SEGS; seg++, frame++)
    {
        y += ropeInc[frame].y;
        tile += ((y & 8) != (rope->point.y & 8)) * 32;

        if (ropeFrame > 0)
        {
            x += ropeInc[frame].x;
            tile += ((x & 8) != (rope->point.x & 8));
        }
        else if (ropeFrame < 0)
        {
            x -= ropeInc[frame].x;
            tile -= ((x & 8) != (rope->point.x & 8));
        }

        rope++;
        rope->point.x = x;
        rope->point.y = y;
        rope->tile = tile;
        rope->align = YALIGN(y);

        // only accept the first segment that touches Willy
        ropeSeg[seg].pos = (y << 8) + x + ROPE_OFFSET;
        if (hit == 0 && (Video_GetPixel(ropeSeg[seg].pos) & B_WILLY))
        {
            hit = seg;
        }
    }

    ropeFrame += ropeDir ? 2 : -2;
    if (ropeFrame > -18 && ropeFrame < 18)
    {
        ropeFrame += ropeDir ? 2 : -2;
    }
    else if (ropeFrame == 54 || ropeFrame == -54)
    {
        ropeDir = 1 - ropeDir;
    }

    return hit;
}

static void DoRopeTicker()
{
    int seg, x;

    seg = Rope_Animate();
    if (minerWilly.rope < 0) // this allows Willy to jump off a rope
    {
        if (minerWilly.rope-- == -16) // for 16 frames
        {
            minerWilly.rope = 0;
        }

        return;
    }

    if (minerWilly.rope == 0)
    {
        if (seg == 0)
        {
            return;
        }

        minerWilly.rope = seg;
        minerWilly.air = 0;
    }

    x = ropeSeg[minerWilly.rope].point.x;

    minerWilly.x = x & 248;
    minerWilly.y = ropeSeg[minerWilly.rope].point.y - 8;
    minerWilly.frame = ((x & 7) >> 1) | (minerWilly.dir << 2);
    minerWilly.tile = ropeSeg[minerWilly.rope].tile;
    minerWilly.align = ropeSeg[minerWilly.rope].align;

    if (minerWilly.move == 0)
    {
        return;
    }

    seg = minerWilly.rope + (ropeDir - minerWilly.dir ? -1 : 1);

    if (seg == 2)
    {
        Game_ChangeLevel(R_ABOVE);
        return;
    }
    else if (seg < ropeStart[gameLevel].y)
    {
        return;
    }

    if (seg < ROPE_SEGS)
    {
        minerWilly.rope = seg;
        return;
    }

    minerWilly.rope = -1;
    minerWilly.air = 2;
    minerWilly.y &= 124;
    minerWilly.align = YALIGN(minerWilly.y);
}

void Rope_Init()
{
    ropeDir = 0;
    ropeFrame = 34;

    if (ropeStart[gameLevel].x == 0)
    {
        Rope_Ticker = DoNothing;
        Rope_Drawer = DoNothing;
        return;
    }
    Rope_Ticker = DoRopeTicker;
    Rope_Drawer = DoRopeDrawer;

    ropeSeg[0].pos = ropeStart[gameLevel].x * 8;
    ropeSeg[0].point.x = ropeSeg[0].pos - ROPE_OFFSET;
    ropeSeg[0].point.y = 0;
    ropeSeg[0].tile = ropeStart[gameLevel].x - 33;
    ropeSeg[0].align = 4;

    if (gameLevel == SWIMMINGPOOL)
    {
        ropeInk = 7;
    }
    else if (gameLevel == COLDSTORE)
    {
        ropeInk = 6;
    }
    else if (gameLevel == THEBEACH)
    {
        ropeInk = 5;
    }
    else if (gameLevel == ONTHEROOF)
    {
        ropeInk = 4;
    }
    else if (gameLevel == QUIRKAFLEEG)
    {
        ropeInk = 6;
    }

    Rope_Animate();
}
