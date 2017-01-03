#include <stdlib.h>
#include <gba.h>
#include <gbaprint.h>

static const unsigned int asc2val_tbl[0x10] = {
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 
	0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 
};

static struct window {
	u16 *p;
	u16 i; // VRAM OFFSET
	int x, y;
    u16* vram;
};

static struct window *win;

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
	*(win->p + win->i++) = asc2val_tbl[(val >> 12 & 0xf)];
	*(win->p + win->i++) = asc2val_tbl[(val >>  8 & 0xf)];
	*(win->p + win->i++) = asc2val_tbl[(val >>  4 & 0xf)];
	*(win->p + win->i++) = asc2val_tbl[(val >>  0 & 0xf)];
}

// 桁対応
void testprintval2(unsigned int val){
	int tmp = 0;	unsigned int val2 = val;
	do { tmp++; } while( val2 >>= 4 );
	for( tmp--; tmp >= 0; tmp--)
		*(win->p + win->i++) = asc2val_tbl[(val >> tmp*4 & 0xf)];
}

void refresh(){
	CpuFastSet(win->p, (u16*)win->vram, (0x100) | COPY16);
}

void gbaprint(char* str){
	while(*(win->p + win->i++) = *str++);
}