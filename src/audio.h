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

#define MUS_STOP    0
#define MUS_PLAY    1

enum
{
    MUS_TITLE,
    MUS_GAME,
    MUS_LOADER
};

enum
{
    SFX_ITEM,
    SFX_DIE,
    SFX_GAMEOVER,
    SFX_ARROW,
    SFX_WILLY,
    SFX_NONE
};

extern int  audioMusicPlaying;
extern int  audioPanX;

void Audio_Init(void);
void Audio_Music(int, int);
void Audio_Play(int);
void Audio_WillySfx(int, int);
void Audio_Sfx(int);
void Audio_ReduceMusicSpeed(void);
