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

const COLOUR    videoColour[16] =
{
    {.r = 0x00, .g = 0x00, .b = 0x00},      // black
    {.r = 0x00, .g = 0x00, .b = 0xff},      // blue
    {.r = 0xff, .g = 0x00, .b = 0x00},      // red
    {.r = 0xff, .g = 0x00, .b = 0xff},      // magenta
    {.r = 0x00, .g = 0xff, .b = 0x00},      // green
    {.r = 0x00, .g = 0xaa, .b = 0xff},      // light blue
    {.r = 0xff, .g = 0xff, .b = 0x00},      // yellow
    {.r = 0xff, .g = 0xff, .b = 0xff},      // white
    {.r = 0xcc, .g = 0xcc, .b = 0xcc},      // mid grey (only for Willy or BT_SOLID)
    {.r = 0x00, .g = 0x55, .b = 0xff},      // mid blue
    {.r = 0xaa, .g = 0x00, .b = 0x00},      // mid red
    {.r = 0x55, .g = 0x00, .b = 0x00},      // dark red
    {.r = 0x00, .g = 0xaa, .b = 0x00},      // mid green
    {.r = 0x00, .g = 0x55, .b = 0x00},      // dark green
    {.r = 0xff, .g = 0x80, .b = 0x00},      // orange
    {.r = 0x80, .g = 0x40, .b = 0x00}       // brown
};

// timer -----------------------------------------------------------------------
void Timer_Set(TIMER *timer, int numerator, int divisor)
{
    timer->acc = 0;
    timer->rate = numerator / divisor;
    timer->remainder = numerator - timer->rate * divisor;
    timer->divisor = divisor;
}

int Timer_Update(TIMER *timer)
{
    timer->acc += timer->remainder;
    if (timer->acc < timer->divisor)
    {
        return timer->rate;
    }

    timer->acc -= timer->divisor;

    return timer->rate + 1;
}
 // external -------------------------------------------------------------------
int Video_Viewport(int width, int height, int *x, int *y, int *w, int *h)
{
    int multiply;

    if (height * 4 / 3 <= width) // landscape
    {
        *h = HEIGHT * height / (float)(HEIGHT + 16);
        *w = *h * 4 / 3;
    }
    else // portrait
    {
        *w = WIDTH * width / (float)(WIDTH + 16);
        *h = *w * 3 / 4;
    }

    *x = (width - *w) / 2;
    *y = (height - *h) / 2;

    multiply = *h / HEIGHT;
    if (multiply < 1)
    {
        multiply = 1;
    }

    return multiply;
}
