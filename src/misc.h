#include "common.h"

#define TICKRATE    60

#define SAMPLERATE  22050

// palette ---------------------------------------------------------------------
typedef struct
{
    u8      r;
    u8      g;
    u8      b;
    u8      padding; // unused
}
COLOUR;

extern const COLOUR     videoColour[16];

// timer -----------------------------------------------------------------------
typedef struct
{
    int rate;
    int acc;
    int remainder;
    int divisor;
}
TIMER;

void Timer_Set(TIMER *, int, int);
int Timer_Update(TIMER *);

// external --------------------------------------------------------------------
int Video_Viewport(int, int, int *, int *, int *, int *);

void Audio_Output(short [2]);
