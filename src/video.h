#define B_LEVEL   1
#define B_ROBOT   2
#define B_WILLY   4

#define TILE2PIXEL(t)   (((t & 992) << 6) | ((t & 31) << 3))
#define YALIGN(y)       (4 | ((y & 4) >> 1) | (y & 2) | ((y & 1) << 1))

void Video_Write(int, char *);
void Video_WriteLarge(int, int, char *);
void Video_DrawSprite(int, u16 *, u8, u8);
void Video_DrawRobot(int, u16 *, u8);
void Video_DrawMiner(int, u16 *, int);
void Video_DrawTile(int, u8 *, u8, u8);
void Video_DrawArrow(int, int);
void Video_DrawRopeSeg(int, u8);
void Video_PixelFill(int, int);
void Video_PixelInkFill(int, int, u8);
void Video_PixelPaperFill(int, int, u8);
void Video_CycleColours(int);
int Video_TextWidth(char *);
int Video_GetPixel(int);
