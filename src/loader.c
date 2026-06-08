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

#include "misc.h"
#include "video.h"
#include "audio.h"

static int      loaderTicks = 0;
static char     loaderText[25] = "\x1\x7\x2\x2" "JetSet Willy Loading";

static void DoLoaderResponder()
{
    Action = Title_Action;
}

static void DoLoaderTicker()
{
    if (videoFlash)
    {
        loaderText[1] = '\x7';
        loaderText[3] = '\x2';
    }
    else
    {
        loaderText[1] = '\x2';
        loaderText[3] = '\x7';
    }

    if (loaderTicks++ == 256)
    {
        Action = Codes_Action;
    }
}

static void DoLoaderDrawer3()
{
    Video_WriteLarge(6 * 8, 11 * 8, loaderText);
}

static void DoLoaderDrawer2()
{
    if (audioMusicPlaying)
    {
        return;
    }

    Video_Write(80 * WIDTH + 5 * 8, "\x1\x6\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14");
    Video_Write(88 * WIDTH + 5 * 8, "\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14");
    Video_Write(96 * WIDTH + 5 * 8, "\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14");
    Video_Write(104 * WIDTH + 5 * 8, "\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14");
    DoLoaderDrawer3();

    Responder = DoLoaderResponder;
    Ticker = DoLoaderTicker;
    Drawer = DoLoaderDrawer3;
}

static void DoLoaderDrawer1()
{
    System_Border(0x1);
    Video_PixelPaperFill(0, WIDTH * HEIGHT, 0x1);
    Video_Write(23 * 8 * WIDTH, "\x1\x1\x2\x7" "fawtytoo");
    Video_Write(23 * 8 * WIDTH + WIDTH - Video_TextWidth(__DATE__), "\x2\x0" __DATE__);

    Audio_Music(MUS_LOADER, MUS_PLAY);

    Drawer = DoLoaderDrawer2;
}

void Loader_Action()
{
    Drawer = DoLoaderDrawer1;

    Action = DoNothing;
}
