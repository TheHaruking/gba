#include <stdlib.h>
#include <gba.h>
#include "gbaprint.h"

struct window {
	u16 *p;
	u16 i;
	int x, y;
    u16* vram;
};

struct window *win;

//#define XYTOVRAM(x, y)	(x + y * 32)
static inline int xyToVram(const int x, const int y){
    return x + y * 32;
}

void move(int x, int y){
	win->x = x; win->y = y;
	win->i = xyToVram(win->x, win->y);
}

void videoInit(u16 *vram){
    win = (struct window *)malloc(sizeof(struct window));
	win->p = (u16*)malloc(0x1000);
	win->i = 0;
	win->x = 0; win->y = 0;
    win->vram = (u16*)vram;
}

void videoFinish(){
	free(win->p);
}

void testprintval(u16 val){
	*(win->p + win->i++) = (val >> 12 & 0xf) + 0x80;
	*(win->p + win->i++) = (val >>  8 & 0xf) + 0x80;
	*(win->p + win->i++) = (val >>  4 & 0xf) + 0x80;
	*(win->p + win->i++) = (val >>  0 & 0xf) + 0x80;
}

// 桁対応
void testprintval2(unsigned int val){
	int tmp = 0;	unsigned int val2 = val;
	do { tmp++; } while(val2 >>= 4);
	for(tmp--; tmp >= 0; tmp--)
		*(win->p + win->i++) = (val >> tmp*4 & 0xf) + 0x80;
}

void refresh(){
	CpuFastSet(win->p, (u16*)win->vram, (0x100) | COPY16);
}