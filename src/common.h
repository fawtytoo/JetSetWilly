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
    KEY_NONE,
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_ENTER,
    KEY_ESCAPE,
    KEY_PAUSE,
    KEY_MUTE,
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
    KEY_QUIT,
    KEY_ELSE
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
