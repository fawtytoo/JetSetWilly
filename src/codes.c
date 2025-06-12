// jetset willy

#include "common.h"
#include "video.h"

#define CODE(p, s)  ((codesCode[p] - 1) << s)

static int      codesDigit[180] =
{
    43, 76, 15, 123, 206, 101, 35, 212, 99, 39, 8, 55, 204, 37, 1, 32, 2, 81,
    44, 202, 222, 181, 47, 83, 74, 179, 90, 45, 154, 27, 165, 71, 44, 238, 124, 65,
    228, 159, 217, 233, 237, 71, 102, 67, 46, 4, 238, 89, 30, 113, 29, 93, 30, 117,
    88, 217, 120, 36, 250, 185, 243, 93, 66, 98, 99, 100, 101, 102, 190, 232, 130, 9,
    77, 104, 132, 40, 140, 138, 24, 13, 109, 20, 87, 114, 33, 113, 129, 23, 15, 35,
    164, 121, 153, 228, 189, 93, 141, 153, 100, 129, 138, 40, 8, 128, 121, 115, 106, 64,
    148, 132, 59, 190, 142, 92, 85, 155, 133, 96, 69, 163, 99, 163, 94, 187, 103, 165,
    132, 76, 218, 159, 68, 26, 157, 112, 2, 60, 82, 211, 168, 173, 112, 205, 192, 112,
    208, 114, 180, 117, 212, 86, 89, 202, 178, 102, 209, 190, 26, 155, 202, 107, 24, 190,
    160, 109, 112, 29, 195, 210, 141, 118, 62, 180, 141, 213, 181, 134, 138, 115, 208, 118
};

static u16      codesSprite[4][16] =
{
    {32766, 49923, 49149, 49149, 65533, 65533, 65533, 63997, 61951, 63999, 63999, 63999, 63997, 63997, 65531, 32766},
    {32766, 50947, 49149, 49149, 65533, 65533, 65533, 61951, 60671, 64767, 63999, 62463, 59391, 57597, 65523, 32766},
    {32766, 50691, 49149, 49149, 49149, 65533, 65535, 61951, 60671, 64767, 61951, 64767, 60671, 61949, 65511, 32766},
    {32766, 50691, 49149, 49149, 49149, 49151, 65535, 65023, 63999, 61951, 59903, 57599, 63999, 63997, 65435, 32766}
};

static int      codesAttempt = 1;
static int      codesNeeded;
static int      codesPos, codesPosLast = 0;
static int      codesCode[4];
static int      codesKey;
static char     codesCell[7] = "\x1\x0\x2\x7\x14\x14";

static void DrawCursor(int pos)
{
    int pixel = 88 * WIDTH + 16 * 8;

    codesCell[1] = codesCode[pos];

    Video_Write(pixel + pos * 24, codesCell);
    pixel += 8 * WIDTH;
    Video_Write(pixel + pos * 24, codesCell);
}

static void GetCode()
{
    char    location[5] = "\x2\x7  ";

    codesNeeded = System_Rnd() % 180;

    location[2] = (char)(codesNeeded % 18 + 'A');
    location[3] = (char)(codesNeeded / 18 + '0');
    Video_WriteLarge(29 * 8, 8 * 8, location);

    codesNeeded = codesDigit[codesNeeded];

    codesCode[0] = 0;
    codesCode[1] = 0;
    codesCode[2] = 0;
    codesCode[3] = 0;

    codesKey = -1;
    codesPos = codesPosLast = 0;

    codesCell[0] = '\x1';
    codesCell[2] = '\x2';
    DrawCursor(1);
    DrawCursor(2);
    DrawCursor(3);
}

static void DoCodesDrawer()
{
    codesCell[3] = '\x7';
    DrawCursor(codesPos);

    if (codesKey == 0)
    {
        return;
    }

    codesCell[3] = codesCode[codesPosLast];
    DrawCursor(codesPosLast);

    codesKey = 0;
}

static void DoCodesTicker()
{
    if (videoFlash)
    {
        codesCell[0] = '\x2';
        codesCell[2] = '\x1';
    }
    else
    {
        codesCell[0] = '\x1';
        codesCell[2] = '\x2';
    }

    if (codesKey < 1)
    {
        return;
    }

    codesPosLast = codesPos;
    codesCode[codesPos] = codesKey;
    codesPos++;
    codesPos &= 3;
}

static void DoCodesResponder()
{
    switch (gameInput)
    {
      case KEY_1:
        codesKey = 1; // blue
        break;

      case KEY_2:
        codesKey = 2; // red
        break;

      case KEY_3:
        codesKey = 3; // magenta
        break;

      case KEY_4:
        codesKey = 4; // green
        break;

      case KEY_ENTER:
        if (codesCode[3] == 0)
        {
            break;
        }

        if ((CODE(0, 6) | CODE(1, 4) | CODE(2, 2) | CODE(3, 0)) == codesNeeded)
        {
            Action = Title_Action;
            break;
        }

        if (codesAttempt == 2)
        {
            DoQuit();
            break;
        }

        Video_WriteLarge(0, 8 * 8, "\x1\x0\x2\x5" "Sorry, try code at location     ");
        codesAttempt = 2;
        GetCode();
        break;

      case KEY_ESCAPE:
        DoQuit();
        break;
    }
}

void Codes_Action()
{
    System_Border(0);
    Video_PixelFill(0, WIDTH * HEIGHT);
    Video_WriteLarge(0, 8 * 8, "\x1\x0\x2\x5" "Enter Code at grid location     ");
    Video_Write(88 * WIDTH + 3 * 8 - 1, "\x2\x7\x15");
    Video_DrawRobot(88 * WIDTH + 2 * 8, codesSprite[0], 1);
    Video_Write(88 * WIDTH + 6 * 8 - 1, "\x15");
    Video_DrawRobot(88 * WIDTH + 5 * 8, codesSprite[1], 2);
    Video_Write(88 * WIDTH + 9 * 8 - 1, "\x15");
    Video_DrawRobot(88 * WIDTH + 8 * 8, codesSprite[2], 3);
    Video_Write(88 * WIDTH + 12 * 8 - 1, "\x15");
    Video_DrawRobot(88 * WIDTH + 11 * 8, codesSprite[3], 4);

    GetCode();

    Responder = DoCodesResponder;
    Ticker = DoCodesTicker;
    Drawer = DoCodesDrawer;

    Action = DoNothing;
}

// jetset willy
