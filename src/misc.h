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
