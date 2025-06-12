#include "common.h"

#define TICKRATE    60

#define SAMPLERATE  22050

typedef struct
{
    int rate;
    int acc;
    int remainder;
    int divisor;
}
TIMER;

extern u32  videoPalette[16];

void Timer_Set(TIMER *, int, int);
int Timer_Update(TIMER *);
int Video_Viewport(int, int, int *, int *, int *, int *);

void Audio_Output(short [2]);
