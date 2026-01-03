#define LIVES           (18 * 8 + 4) * WIDTH + 4
#define STATUS          (21 * 8 + 4)

#define THEDRIVE        4
#define QUIRKAFLEEG     16
#define ONTHEROOF       18
#define BALLROOMEAST    20
#define COLDSTORE       25
#define THECHAPEL       27
#define FIRSTLANDING    28
#define NIGHTMAREROOM   29
#define SWIMMINGPOOL    31
#define EASTWALL        32
#define THEBATHROOM     33
#define MASTERBEDROOM   35
#define THEBEACH        57

enum
{
    GM_NORMAL,
    GM_MARIA,
    GM_RUNNING,
    GM_TOILET
};

enum
{
    R_ABOVE,
    R_RIGHT,
    R_BELOW,
    R_LEFT
};

typedef struct
{
    int     x, y;
    int     tile, align;
    int     frame;
    int     dir, move;
    int     air, jump;
} MINER;

extern int      itemCount;
extern int      gameMode;
extern int      gamePaused;
extern int      gameLives;

extern MINER    minerWilly;
extern int      minerWillyRope;

// game
extern int  gameLevel;
extern int  gameClockTicks;

void Game_InitRoom(void);
void Game_GotItem(void);
void Game_ChangeLevel(int);
void Game_Inactivity(int);
void Game_GameReset(void);
void Game_DrawStatus(void);
void Game_Pause(int);
void Game_CheatEnabled(void);

// cheat
extern int      cheatEnabled;
extern EVENT    Cheat_Responder;

void Cheat_Disabled(void);

// levels
enum
{
    T_ITEM,
    T_SPACE,
    T_SOLID,
    T_FLOOR,
    T_SOLIDFLOOR,
    T_CONVEYL,
    T_CONVEYR,
    T_RAMPL,
    T_RAMPR,
    T_RAMPLC,
    T_RAMPRC,
    T_HARM
};

enum
{
    C_NONE,
    C_LEFT,
    C_RIGHT
};

void Level_RestoreItems(void);
void Level_Init(void);
void Level_Drawer(void);
void Level_Ticker(void);
int Level_GetTileType(int);
int Level_GetTileRamp(int);
void Level_EraseItem(int);
int Level_ItemCount(void);
int Level_Dir(int);
void Level_SetBorder(void);

// miner
extern int  minerAttrSplit;

void Miner_Init(void);
void Miner_Ticker(void);
void Miner_Drawer(void);
void Miner_Save(void);
void Miner_Restore(void);
void Miner_SetSeq(int, int);
void Miner_IncSeq(void);
void Miner_DrawSeqSprite(int, u8, u8);

// robots
void Robots_Init(void);
void Robots_Drawer(void);
void Robots_Ticker(void);
void Robots_Flush(void);
void Robots_DrawCheat(void);

// rope
void Rope_Init(void);
extern EVENT    Rope_Ticker, Rope_Drawer;
