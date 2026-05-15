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

#define B_LEVEL   1
#define B_ROBOT   2
#define B_WILLY   4

#define TILE2PIXEL(t)   (((t & 992) << 6) | ((t & 31) << 3))
#define YALIGN(y)       (4 | ((y & 4) >> 1) | (y & 2) | ((y & 1) << 1))

void Video_Write(int, char *);
void Video_WriteLarge(int, int, char *);
void Video_DrawSprite(int, u16 *, u8, u8);
void Video_DrawRobot(int, u16 *, u8);
int Video_DrawMiner(int, u16 *, int);
void Video_DrawTile(int, u8 *, u8, u8);
void Video_DrawArrow(int, int);
void Video_DrawRopeSeg(int, u8);
void Video_PixelFill(int, int);
void Video_PixelInkFill(int, int, u8);
void Video_PixelPaperFill(int, int, u8);
int Video_CycleColours(void);
int Video_TextWidth(char *);
int Video_GetPixel(int);
