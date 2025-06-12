#include "misc.h"

u32     videoPalette[16] =
{
    0x000000,   // black
    0x0000ff,   // blue
    0xff0000,   // red
    0xff00ff,   // magenta
    0x00ff00,   // green
    0x00aaff,   // light blue
    0xffff00,   // yellow
    0xffffff,   // white
    0xcccccc,   // light grey (only for Willy or BT_SOLID)
    0x0055ff,   // mid blue
    0xaa0000,   // mid red
    0x550000,   // dark red
    0x00aa00,   // mid green
    0x005500,   // dark green
    0xff8000,   // orange
    0x804000    // brown
};

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
    if (multiply > 2)
    {
        multiply -= 2;
    }

    return multiply;
}
