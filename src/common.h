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

#define WIDTH   256
#define HEIGHT  192

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

typedef void (*EVENT)(void);

enum
{
    KEY_LEFT,
    KEY_RIGHT,
    KEY_JUMP,
    KEY_ENTER,
    KEY_LSHIFT,
    KEY_RSHIFT,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_ESCAPE,
    KEY_PAUSE,
    KEY_MUTE,
    KEY_QUIT,
    KEY_ELSE,
    KEY_NONE
};

extern EVENT    Action, Responder, Ticker, Drawer;

extern int      gameInput;

extern int      videoFlash;

void DoNothing(void);
void DoQuit(void);

void System_Border(int);
int System_Rnd(void);
int System_IsKey(int);
void System_SetPixel(int, int);

void Codes_Action(void);
void Loader_Action(void);
void Title_Action(void);
void Game_Action(void);
void Die_Action(void);
void Gameover_Action(void);
