#include "misc.h"
#include "game.h"

#include "audio.h"

#define VOLUME      32768 / 4
#define MUSICVOLUME VOLUME / 8
#define SFXVOLUME   VOLUME / 4

#define NCHANNELS   8
#define NMUSIC      5
#define NSFX        3 // sfx channels

#define EV_NOTEOFF  0x00
#define EV_NOTEON   0x10
#define EV_END      0x40

#define L           0
#define R           1

typedef struct
{
    int     left[3], right[3];    // pseudo stereo/panning effects
    u32     phase, frequency;
    EVENT   DoPhase;
}
CHANNEL;

typedef struct
{
    int     *pitch;
    int     length;
    CHANNEL *channel;
    int     clock;
    EVENT   DoSfx, DoPlay;
}
SFX;

static int      panTable[241] =
{
    256, 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 240,
    239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 225, 224,
    223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 210, 209, 208,
    207, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 195, 194, 193, 192,
    191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 180, 179, 178, 177, 176,
    175, 174, 173, 172, 171, 170, 169, 168, 167, 165, 164, 163, 162, 161, 160,
    159, 158, 157, 156, 155, 154, 153, 152, 150, 149, 148, 147, 146, 145, 144,
    143, 142, 141, 140, 139, 138, 137, 135, 134, 133, 132, 131, 130, 129, 128,
    127, 126, 125, 124, 123, 122, 121, 119, 118, 117, 116, 115, 114, 113, 112,
    111, 110, 109, 108, 107, 106, 104, 103, 102, 101, 100,  99,  98,  97,  96,
     95,  94,  93,  92,  91,  89,  88,  87,  86,  85,  84,  83,  82,  81,  80,
     79,  78,  77,  76,  74,  73,  72,  71,  70,  69,  68,  67,  66,  65,  64,
     63,  62,  61,  59,  58,  57,  56,  55,  54,  53,  52,  51,  50,  49,  48,
     47,  46,  44,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  32,
     31,  29,  28,  27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16,
     14,  13,  12,  11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0
};

static u32      frequencyTable[128] =
{
    0x00184cbb, 0x0019bea3, 0x001b4688, 0x001ce5bd, 0x001e9da1, 0x00206fae, 0x00225d71, 0x00246891,
    0x002692cb, 0x0028ddfb, 0x002b4c15, 0x002ddf2d, 0x00309976, 0x00337d46, 0x00368d11, 0x0039cb7a,
    0x003d3b43, 0x0040df5c, 0x0044bae3, 0x0048d122, 0x004d2597, 0x0051bbf7, 0x0056982b, 0x005bbe5b,
    0x006132ed, 0x0066fa8b, 0x006d1a25, 0x007396f4, 0x007a7686, 0x0081beba, 0x008975c6, 0x0091a244,
    0x009a4b30, 0x00a377ee, 0x00ad3056, 0x00b77cb7, 0x00c265db, 0x00cdf516, 0x00da344a, 0x00e72de9,
    0x00f4ed0c, 0x01037d74, 0x0112eb8c, 0x01234488, 0x01349660, 0x0146efdc, 0x015a60ad, 0x016ef96d,
    0x0184cbb6, 0x019bea2e, 0x01b46891, 0x01ce5bd2, 0x01e9da1b, 0x0206fae5, 0x0225d719, 0x02468913,
    0x02692cbe, 0x028ddfb9, 0x02b4c15a, 0x02ddf2dc, 0x0309976d, 0x0337d45a, 0x0368d125, 0x039cb7a5,
    0x03d3b434, 0x040df5cc, 0x044bae33, 0x048d1224, 0x04d2597f, 0x051bbf72, 0x056982b5, 0x05bbe5b7,
    0x06132edb, 0x066fa8b7, 0x06d1a249, 0x07396f4b, 0x07a76867, 0x081beb9b, 0x08975c67, 0x091a2448,
    0x09a4b300, 0x0a377ee5, 0x0ad3056f, 0x0b77cb68, 0x0c265db7, 0x0cdf5173, 0x0da3448d, 0x0e72de96,
    0x0f4ed0d9, 0x1037d72a, 0x112eb8ce, 0x1234489d, 0x134965f4, 0x146efdcb, 0x15a60ac1, 0x16ef96f1,
    0x184cbb6f, 0x19bea2c3, 0x1b468941, 0x1ce5bd2c, 0x1e9da187, 0x206fae82, 0x225d719d, 0x24689107,
    0x2692cc1e, 0x28ddfb96, 0x2b4c1582, 0x2ddf2de3, 0x309976df, 0x337d4586, 0x368d1283, 0x39cb7a58,
    0x3d3b430f, 0x40df5d05, 0x44bae33a, 0x48d1220f, 0x4d25983c, 0x51bbf72d, 0x56982bf5, 0x5bbe5ac8,
    0x6132edbe, 0x66fa8c2a, 0x6d1a23d8, 0x7396f4b1, 0x7a768772, 0x81beb8a3, 0x8975c674, 0x91a245b2
};

static short    musicScore[3][1130] =
{
    {
        16, 58, 0, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 0, 0, 3, 1,
        16, 56, 0, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 0, 0, 3, 1,
        16, 54, 0, 19, 66, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 1, 19, 66, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 0, 0, 3, 1, 16, 51, 0, 19, 66, 13, 3, 1, 19, 71, 13, 3, 1, 19, 75, 13, 3, 1, 19, 66, 13, 3, 1, 19, 71, 13, 3, 1, 19, 75, 13, 0, 0, 3, 1,
        16, 53, 0, 19, 65, 13, 3, 1, 19, 69, 13, 3, 1, 19, 75, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 0, 0, 3, 1, 16, 53, 0, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 72, 13, 3, 1, 19, 63, 13, 3, 1, 19, 69, 13, 3, 1, 19, 72, 13, 0, 0, 3, 1,
        16, 46, 0, 17, 53, 0, 19, 61, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 0, 20, 77, 13, 3, 1, 19, 70, 13, 4, 0, 3, 1, 19, 73, 13, 4, 1, 20, 77, 6, 0, 0, 1, 0, 3, 0, 4, 1,
        16, 45, 0, 17, 53, 0, 19, 65, 0, 20, 77, 13, 3, 1, 19, 72, 13, 3, 1, 19, 75, 13, 3, 1, 19, 65, 13, 3, 1, 19, 72, 13, 3, 1, 19, 75, 13, 3, 1, 19, 65, 13, 3, 1, 19, 72, 13, 3, 1, 19, 75, 13, 3, 0, 4, 1, 19, 65, 0, 20, 77, 13, 3, 1, 19, 72, 13, 4, 0, 3, 1, 19, 75, 13, 4, 1, 20, 77, 6, 0, 0, 1, 0, 3, 0, 4, 1,
        16, 46, 0, 19, 65, 0, 20, 77, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 0, 0, 3, 0, 4, 1, 16, 51, 0, 19, 66, 0, 20, 78, 13, 3, 1, 19, 70, 13, 3, 1, 19, 75, 13, 3, 1, 19, 66, 13, 3, 1, 19, 70, 13, 3, 1, 19, 75, 13, 0, 0, 3, 0, 4, 1,
        16, 56, 0, 19, 65, 0, 20, 77, 13, 3, 1, 19, 68, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 68, 13, 3, 1, 19, 73, 13, 0, 0, 3, 0, 4, 1, 16, 56, 0, 19, 66, 0, 20, 75, 13, 3, 1, 19, 68, 13, 3, 0, 4, 1,
        19, 72, 13, 3, 1, 19, 66, 0, 20, 80, 13, 3, 1, 19, 68, 13, 3, 0, 4, 1, 19, 72, 13, 0, 0, 3, 1, 16, 61, 0, 19, 65, 0, 20, 73, 13, 3, 1, 19, 68, 13, 3, 0, 4, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 68, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 68, 13, 3, 1, 19, 73, 13, 3, 1, 19, 65, 13, 3, 1, 19, 68, 13, 3, 1, 19, 73, 13, 0, 0, 3, 1,
        16, 49, 0, 19, 64, 13, 3, 1, 19, 68, 13, 3, 1, 19, 73, 13, 3, 1, 19, 64, 13, 3, 1, 19, 68, 13, 3, 1, 19, 73, 13, 3, 1, 19, 64, 13, 3, 1, 19, 68, 13, 3, 1, 19, 73, 13, 3, 1, 19, 64, 0, 20, 76, 13, 3, 1, 19, 68, 13, 4, 0, 3, 1, 19, 73, 13, 4, 1, 20, 76, 6, 0, 0, 3, 0, 4, 1,
        16, 47, 0, 19, 64, 0, 20, 76, 13, 3, 1, 19, 68, 13, 3, 1, 19, 74, 13, 3, 1, 19, 64, 13, 3, 1, 19, 68, 13, 3, 1, 19, 74, 13, 3, 1, 19, 64, 13, 3, 1, 19, 68, 13, 3, 1, 19, 74, 13, 3, 0, 4, 1, 19, 64, 0, 20, 76, 13, 3, 1, 19, 68, 13, 4, 0, 3, 1, 19, 74, 13, 4, 1, 20, 76, 6, 0, 0, 3, 0, 4, 1,
        16, 45, 0, 19, 64, 0, 20, 76, 13, 3, 1, 19, 69, 13, 0, 0, 3, 1, 19, 73, 13, 3, 1, 19, 64, 0, 16, 44, 13, 3, 1, 19, 68, 13, 3, 1, 19, 73, 13, 0, 0, 3, 1, 19, 64, 0, 16, 43, 13, 3, 1, 19, 70, 13, 3, 1, 19, 73, 13, 3, 0, 4, 1, 19, 63, 0, 20, 75, 13, 3, 1, 19, 70, 13, 3, 0, 4, 1, 19, 73, 13, 3, 0, 0, 1,
        16, 44, 0, 19, 63, 0, 20, 75, 13, 3, 1, 19, 68, 13, 3, 1, 19, 71, 13, 3, 1, 19, 63, 13, 3, 1, 19, 68, 13, 3, 1, 19, 71, 13, 0, 0, 3, 0, 4, 1, 16, 49, 0, 19, 64, 0, 20, 76, 13, 3, 1, 19, 68, 13, 0, 0, 3, 0, 4, 1, 19, 70, 13, 3, 1, 19, 61, 0, 20, 73, 0, 16, 52, 13, 3, 1, 19, 68, 13, 3, 0, 4, 1, 19, 70, 13, 0, 0, 3, 1, 19, 63, 0, 20, 75, 0,
        16, 51, 13, 3, 1, 19, 68, 13, 3, 1, 19, 71, 13, 3, 1, 19, 63, 13, 3, 1, 19, 68, 13, 3, 1, 19, 71, 13, 3, 0, 4, 0, 0, 1, 16, 51, 0, 19, 63, 0, 20, 75, 13, 3, 1, 19, 67, 13, 3, 1, 19, 70, 13, 3, 1, 19, 63, 13, 3, 1, 19, 67, 13, 3, 1, 19, 70, 13, 0, 0, 3, 0, 4, 1,
        16, 56, 0, 19, 68, 13, 3, 1, 19, 71, 13, 3, 1, 19, 75, 13, 3, 1, 19, 68, 13, 3, 1, 19, 71, 13, 3, 1, 19, 75, 13, 3, 1, 19, 68, 83, 0, 0, 3, 1,
        EV_END, MUS_STOP
    },
    {
        16, 48, 0, 19, 67, 11, 3, 1, 19, 65, 11, 0, 0, 3, 1, 16, 52, 0, 17, 55, 0, 19, 67, 11, 3, 1, 19, 65, 11, 0, 0, 1, 0, 3, 1, 16, 48, 0, 19, 64, 23, 0, 0, 3, 1, 16, 52, 0, 17, 55, 0, 19, 60, 23, 0, 0, 1, 0, 3, 1,
        16, 48, 23, 0, 1, 16, 52, 0, 17, 55, 0, 19, 64, 11, 3, 1, 19, 65, 11, 0, 0, 1, 0, 3, 1, 16, 48, 0, 19, 67, 11, 3, 1, 19, 65, 11, 0, 0, 3, 1, 16, 52, 0, 17, 55, 0, 19, 67, 11, 3, 1, 19, 65, 11, 0, 0, 1, 0, 3, 1,
        16, 48, 0, 19, 64, 11, 3, 1, 19, 65, 11, 0, 0, 3, 1, 16, 52, 0, 17, 55, 0, 19, 67, 11, 3, 1, 19, 69, 11, 0, 0, 1, 0, 3, 1, 16, 48, 0, 19, 70, 11, 3, 1, 19, 69, 11, 0, 0, 3, 1, 16, 52, 0, 17, 55, 0, 19, 70, 11, 3, 1, 19, 69, 11, 0, 0, 1, 0, 3, 1,
        16, 48, 0, 19, 67, 23, 0, 1, 16, 52, 0, 17, 55, 23, 0, 0, 1, 0, 3, 1, 16, 48, 23, 0, 1, 16, 52, 0, 17, 55, 23, 0, 0, 1, 1,
        16, 43, 0, 19, 68, 23, 0, 0, 3, 1, 16, 47, 0, 17, 53, 0, 19, 67, 23, 0, 0, 1, 0, 3, 1, 16, 43, 0, 19, 66, 23, 0, 0, 3, 1, 16, 47, 0, 17, 53, 0, 19, 65, 23, 0, 0, 1, 0, 3, 1,
        16, 48, 0, 17, 51, 0, 19, 63, 11, 3, 1, 19, 62, 11, 0, 0, 1, 0, 3, 1, 19, 60, 11, 3, 1, 19, 62, 11, 3, 0, 0, 1, 16, 48, 0, 19, 60, 0, 20, 63, 23, 0, 1, 16, 46, 23, 0, 0, 3, 0, 4, 1,
        16, 45, 0, 17, 54, 0, 19, 63, 11, 3, 1, 19, 62, 11, 0, 0, 1, 0, 3, 1, 19, 60, 11, 3, 1, 19, 62, 11, 3, 0, 0, 1, 16, 45, 0, 17, 53, 0, 19, 63, 23, 0, 0, 1, 0, 3, 1, 16, 50, 0, 17, 53, 0, 19, 60, 23, 0, 0, 1, 0, 3, 1,
        16, 43, 0, 17, 53, 0, 19, 59, 0, 20, 67, 47, 0, 0, 1, 0, 3, 0, 4, 1, 16, 43, 23, 0, 25, 0, 1,
        16, 48, 0, 19, 67, 11, 3, 1, 19, 65, 11, 0, 0, 3, 1, 16, 52, 0, 17, 55, 0, 19, 67, 11, 3, 1, 19, 65, 11, 0, 0, 1, 0, 3, 1, 16, 48, 0, 19, 64, 23, 0, 0, 3, 1, 16, 52, 0, 17, 55, 0, 19, 60, 23, 0, 0, 1, 0, 3, 1,
        16, 48, 23, 0, 1, 16, 52, 0, 17, 55, 0, 19, 64, 11, 3, 1, 19, 65, 11, 0, 0, 1, 0, 3, 1, 16, 48, 0, 19, 67, 11, 3, 1, 19, 65, 11, 0, 0, 3, 1, 16, 52, 0, 17, 55, 0, 19, 67, 11, 3, 1, 19, 65, 11, 0, 0, 1, 0, 3, 1,
        16, 48, 0, 19, 64, 11, 3, 1, 19, 65, 11, 0, 0, 3, 1, 16, 52, 0, 17, 55, 0, 19, 67, 11, 3, 1, 19, 69, 11, 0, 0, 1, 0, 3, 1, 16, 48, 0, 19, 70, 11, 3, 1, 19, 69, 11, 0, 0, 3, 1, 16, 52, 0, 17, 55, 0, 19, 70, 11, 3, 1, 19, 69, 11, 0, 0, 1, 0, 3, 1,
        16, 48, 0, 19, 67, 23, 0, 1, 16, 52, 0, 17, 55, 23, 0, 0, 1, 0, 3, 1, 16, 48, 23, 0, 1, 16, 52, 0, 17, 55, 23, 0, 0, 1, 1,
        16, 43, 0, 19, 68, 23, 0, 0, 3, 1, 16, 47, 0, 17, 53, 0, 19, 67, 23, 0, 0, 1, 0, 3, 1, 16, 43, 0, 19, 66, 23, 0, 0, 3, 1, 16, 47, 0, 17, 53, 0, 19, 65, 23, 0, 0, 1, 0, 3, 1,
        16, 48, 0, 17, 51, 0, 19, 63, 11, 3, 1, 19, 62, 11, 0, 0, 1, 0, 3, 0, 19, 60, 11, 3, 1, 19, 62, 11, 0, 0, 3, 1, 16, 48, 0, 19, 60, 0, 20, 64, 23, 0, 1, 16, 46, 23, 0, 0, 3, 0, 4, 1,
        16, 45, 0, 17, 54, 0, 19, 63, 11, 3, 1, 19, 62, 11, 0, 0, 1, 0, 3, 0, 19, 60, 11, 3, 1, 19, 64, 11, 0, 0, 3, 1, 16, 43, 0, 17, 53, 0, 19, 62, 11, 3, 1, 19, 60, 11, 0, 0, 1, 0, 3, 0, 19, 59, 11, 3, 1, 19, 62, 11, 0, 0, 3, 1,
        16, 48, 0, 17, 52, 0, 19, 60, 23, 0, 0, 1, 0, 3, 49,
        EV_END, MUS_PLAY
    },
    {
        0, 30, 16, 60, 6, 16, 62, 6, 16, 64, 6, 16, 65, 6, 16, 67, 6, 16, 69, 6, 16, 71, 6, 16, 72, 6, 0, 0, EV_END, MUS_STOP
    }
};

static int      sfxPitch[][50] =
{
    {96, 90, 84, 78, 72, 66, 60, 54, 0},
    {84, 81, 78, 75, 72, 69, 66, 63, 60, 57, 54, 51, 48, 45, 42, 39, 0},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 0},
    {48, 54, 60, 66, 72, 78, 84, 90, 0},
    {0, 0},
    {0}
};

static CHANNEL  audioChannel[NCHANNELS] =
{
    {{0, 0, 0}, {0, 0, 0}, 0, 0, DoNothing},
    {{0, 0, 0}, {0, 0, 0}, 0, 0, DoNothing},
    {{0, 0, 0}, {0, 0, 0}, 0, 0, DoNothing},
    {{MUSICVOLUME, -MUSICVOLUME, 0}, {-MUSICVOLUME, MUSICVOLUME, 0}, 0, 0, DoNothing},
    {{MUSICVOLUME, -MUSICVOLUME, 0}, {-MUSICVOLUME, MUSICVOLUME, 0}, 0, 0, DoNothing},
    {{MUSICVOLUME, -MUSICVOLUME, 0}, {-MUSICVOLUME, MUSICVOLUME, 0}, 0, 0, DoNothing},
    {{MUSICVOLUME, -MUSICVOLUME, 0}, {-MUSICVOLUME, MUSICVOLUME, 0}, 0, 0, DoNothing},
    {{MUSICVOLUME, -MUSICVOLUME, 0}, {-MUSICVOLUME, MUSICVOLUME, 0}, 0, 0, DoNothing}
};

static CHANNEL  *musicChannel[NMUSIC] = // mapping
{
    &audioChannel[3],
    &audioChannel[4],
    &audioChannel[5],
    &audioChannel[6],
    &audioChannel[7]
};

static SFX      sfxInfo[NSFX] =
{
    {sfxPitch[SFX_NONE], 0, &audioChannel[0], 0, DoNothing, DoNothing},
    {sfxPitch[SFX_NONE], 0, &audioChannel[1], 0, DoNothing, DoNothing},
    {sfxPitch[SFX_NONE], 0, &audioChannel[2], 0, DoNothing, DoNothing}
};

static int      musicIndex;
static int      musicTempo, musicPitch;
static int      musicClock = 0, musicDelta;
static short    *curMusic;
static int      samplesMusic = 0;

static int      musicChannels = 0;

static int      sfxClock = 0;
static int      samplesSfx = 0;

static TIMER    timerSfx, timerMusic;
static CHANNEL  *curChannel;
static SFX      *curSfx;

int             audioMusicPlaying = MUS_STOP;
int             audioPanX;

void Audio_ReduceMusicSpeed()
{
    musicPitch--; // down one semitone
    musicTempo -= 6;
    Timer_Set(&timerMusic, SAMPLERATE, musicTempo);
}

static void DoPhase()
{
    u32     phase = curChannel->phase >> 31;

    curChannel->left[2] = curChannel->left[phase];
    curChannel->right[2] = curChannel->right[phase];

    curChannel->phase += curChannel->frequency;
}

static void DoChannelOff()
{
    curChannel->DoPhase = DoNothing;
    curChannel->left[2] = 0;
    curChannel->right[2] = 0;
}

static void ChannelStereo(CHANNEL *channel, int left, int right)
{
    left = SFXVOLUME * left >> 8;
    right = SFXVOLUME * right >> 8;

    channel->left[0] = left;
    channel->right[0] = -right;
    channel->left[1] = -left;
    channel->right[1] = right;
}

static void ChannelPan(CHANNEL *channel, int pan)
{
    ChannelStereo(channel, panTable[pan], 256 - panTable[pan]);
}

static void DoSfxOff()
{
    curSfx->DoSfx = DoNothing;
    curSfx->channel->DoPhase = DoChannelOff;
}

static void DoSfxOn()
{
    curSfx->channel->DoPhase = DoPhase;
    curSfx->DoSfx = curSfx->DoPlay;

    curSfx->DoSfx();
}

static void DoSfxPlay()
{
    curSfx->channel->frequency = frequencyTable[*curSfx->pitch];
    curSfx->clock += curSfx->length;

    curSfx->pitch++;
    if (*curSfx->pitch > 0)
    {
        return;
    }

    curSfx->DoSfx = DoSfxOff;
}

static void DoSfxWilly()
{
    curSfx->channel->DoPhase = DoPhase;
    curSfx->clock += curSfx->length;
    curSfx->DoSfx = DoSfxOff;
}

void Audio_WillySfx(int note, int length)
{
    sfxInfo[0].channel->frequency = frequencyTable[note];
    sfxInfo[0].clock = sfxClock;
    ChannelPan(sfxInfo[0].channel, audioPanX);
    sfxInfo[0].length = length;
    sfxInfo[0].DoSfx = DoSfxWilly;
}

void Audio_Sfx(int sfx)
{
    if (sfx == SFX_GAMEOVER || sfx == SFX_DIE)
    {
        sfxInfo[0].DoSfx = DoSfxOff;
        sfxInfo[1].DoSfx = DoSfxOff;
        curSfx = &sfxInfo[2];
    }
    else if (sfx == SFX_ARROW)
    {
        curSfx = &sfxInfo[2];
    }
    else
    {
        curSfx = &sfxInfo[1];
    }

    switch (sfx)
    {
      case SFX_ITEM:
        sfxInfo[1].length = 1;
        sfxInfo[1].pitch = sfxPitch[sfx];
        ChannelPan(sfxInfo[1].channel, audioPanX);
        sfxInfo[1].DoPlay = DoSfxPlay;
        break;

      case SFX_ARROW:
        sfxInfo[2].length = 1;
        sfxInfo[2].pitch = sfxPitch[sfx];
        ChannelStereo(sfxInfo[2].channel, 256 - audioPanX, audioPanX);
        sfxInfo[2].DoPlay = DoSfxPlay;
        break;

      case SFX_DIE:
        sfxInfo[2].length = 1;
        sfxInfo[2].pitch = sfxPitch[sfx];
        ChannelPan(sfxInfo[2].channel, audioPanX);
        sfxInfo[2].DoPlay = DoSfxPlay;
        break;

      case SFX_GAMEOVER:
        sfxInfo[2].length = 2;
        sfxInfo[2].pitch = sfxPitch[sfx];
        ChannelStereo(sfxInfo[2].channel, 256, -256);
        sfxInfo[2].DoPlay = DoSfxPlay;
        break;
    }

    curSfx->DoSfx = DoSfxOn;
    curSfx->clock = sfxClock;
}

static void MusicReset()
{
    int index;

    for (index = 0; index < NMUSIC; index++)
    {
        musicChannel[index]->DoPhase = DoChannelOff;
    }

    curMusic = musicScore[musicIndex];
    musicDelta = 0;
    musicClock = 0;
}

void Audio_Output(short output[2])
{
    int     channel, data, time;

    if (samplesMusic == 0)
    {
        samplesMusic = Timer_Update(&timerMusic);

        if (audioMusicPlaying)
        {
            if (musicDelta == musicClock)
            {
                do
                {
                    data = *curMusic++;
                    channel = data & 0x0f; 
                    switch (data & 0xf0)
                    {
                      case EV_NOTEOFF:
                        musicChannel[channel]->DoPhase = DoChannelOff;
                        break;

                      case EV_NOTEON:
                        musicChannel[channel]->frequency = frequencyTable[*curMusic++ + musicPitch];
                        musicChannel[channel]->DoPhase = DoPhase;
                        break;

                      case EV_END:
                        audioMusicPlaying = *curMusic;
                        MusicReset();
                        time = audioMusicPlaying ? 0 : 1;
                        continue;
                    }

                    time = *curMusic++;
                    musicDelta += time;
                }
                while (time == 0);
            }

            musicClock++;
        }
    }

    if (samplesSfx == 0)
    {
        samplesSfx = Timer_Update(&timerSfx);

        curSfx = &sfxInfo[0];
        for (channel = 0; channel < NSFX; channel++, curSfx++)
        {
            if (curSfx->clock == sfxClock)
            {
                curSfx->DoSfx();
            }
        }

        sfxClock++;
    }

    output[L] = output[R] = 0;

    curChannel = &audioChannel[0];
    for (channel = 0; channel < musicChannels; channel++, curChannel++)
    {
        curChannel->DoPhase();
        output[L] += curChannel->left[2];
        output[R] += curChannel->right[2];
    }

    samplesMusic--;
    samplesSfx--;
}

void Audio_Play(int playing)
{
    audioMusicPlaying = playing;
    musicChannels = playing == MUS_PLAY ? NCHANNELS : NSFX;
}

void Audio_Music(int music, int playing)
{
    musicIndex = music;
    MusicReset();
    musicPitch = 0;
    musicTempo = TICKRATE;

    sfxInfo[0].DoSfx = DoSfxOff;
    sfxInfo[1].DoSfx = DoSfxOff;
    sfxInfo[2].DoSfx = DoSfxOff;
    // no point resetting the sfx clocks

    samplesMusic = 0;
    Timer_Set(&timerMusic, SAMPLERATE, TICKRATE);
    Audio_Play(playing);
}

void Audio_Init()
{
    Timer_Set(&timerSfx, SAMPLERATE, TICKRATE);
}
