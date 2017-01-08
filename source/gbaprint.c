#include <stdlib.h>
#include <gba.h>
#include <gbaprint.h>
////////////////////////////////
// 宣言_関数
////////////////////////////////
static inline int xyToVram(const int, const int);
extern void move(int, int);
static void edgeline();
extern void videoInit(unsigned short *);
extern void videoFinish();
extern void testprintval(unsigned short);
extern void gbaprintval(unsigned int);
extern void gbaprintraw(unsigned short);
extern void refresh();
extern void cls();
extern void gbaprint(const char*);

#define BUF_SIZE		0x1000

////////////////////////////////
// 宣言_構造体
////////////////////////////////
struct _window {
	u16 *vram;
	u16 *p[2];
	int b;
	int i; // VRAM OFFSET
	int x, y;
};

////////////////////////////////
// 宣言_変数
////////////////////////////////
static struct _window *win;

////////////////////////////////
// 定義_データ
////////////////////////////////
static const unsigned int asc2val_tbl[0x10] = {
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 
	0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 
};

static const unsigned int zero = 0;

////////////////////////////////
// 定義_関数
////////////////////////////////
//#define XYTOVRAM(x, y)	(x + y * 32)
static inline int xyToVram(const int x, const int y){
    return x + y * 32;
}

inline void move(int x, int y){
	win->x = x; win->y = y;
	win->i = xyToVram(win->x, win->y);
}

static inline void edgeline(){
	if((win->i & 31) >= 30){
		move(0, ++win->y);
	}
}

void videoInit(u16 *vram){
    win = (struct _window *)malloc(sizeof(struct _window));
	win->p[0] = (u16*)malloc(BUF_SIZE);
	win->p[1] = (u16*)malloc(BUF_SIZE);
	win->i = 0;
	win->x = 0; win->y = 0;
	win->b = 0;
    win->vram = (u16*)vram;
}

void videoFinish(){
	free(win->p[0]);
	free(win->p[1]);
	free(win);
}

void testprintval(u16 val){
	*(win->p[win->b] + win->i++) = asc2val_tbl[(val >> 12 & 0xf)];
	*(win->p[win->b] + win->i++) = asc2val_tbl[(val >>  8 & 0xf)];
	*(win->p[win->b] + win->i++) = asc2val_tbl[(val >>  4 & 0xf)];
	*(win->p[win->b] + win->i++) = asc2val_tbl[(val >>  0 & 0xf)];
}

void gbaprintraw(unsigned short num){
	*(win->p[win->b] + win->i++) = num;
}

// 桁対応
void gbaprintval(unsigned int val){
	int tmp = 0;	unsigned int val2 = val;
	do { tmp++; } while( val2 >>= 4 );
	for( tmp--; tmp >= 0; tmp--)
		*(win->p[win->b] + win->i++) = asc2val_tbl[(val >> tmp*4 & 0xf)];
}

void refresh(){
	DMA3COPY(win->p[win->b], (u16*)win->vram , (BUF_SIZE >> 1) | COPY16);
	win->b ^= 1;
	DMA3COPY(&zero , (u16*)win->p[win->b]	 , (BUF_SIZE >> 1) | COPY16 | DMA_SRC_FIXED);
}

void gbaprint(const char* str){
	char c;
	for( ; *str != 0 ; str++ ){
		c = *str;
		if(c == '\n'){
			move(0, ++win->y);
			continue;
		}
		*(win->p[win->b] + win->i++) = *str;
		edgeline();
	}
}