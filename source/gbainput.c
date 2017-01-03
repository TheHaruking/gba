#include <stdlib.h>
#include <gba.h>
#include "gbainput.h"
#include <mappy.h>

struct keys {
	unsigned int hold_j;
	unsigned int hold_a, hold_b;
	unsigned int push_a, push_b;
	unsigned int rrse_a, rrse_b;
	unsigned int hold_I, rrse_I, push_X;
};

struct data {
    unsigned int k0, k1, k2, k3;
    char linebuf[256];
    int linebuf_p;
	int linebuf_p_old;
	int mode, J, AB, IX, J2;
	int direction;
	int chr_pointer;
	int y;
	struct keys k;
};

static struct data *d;

static const int dpadToDigit_tbl[16] = {
//	     R   L
	 0,  3,  7, -1,
	 1,  2,  8, -1, // U
	 5,  4,  6, -1, // D
	-1, -1, -1, -1,
};

static const int direction3_tbl[9][9] = {
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 0,  1,  2,  0,  0, -1,  0,  0,  3},
	{ 0,  3,  1,  2,  0,  0, -1,  0,  0},
	{ 0,  0,  3,  1,  2,  0,  0, -1,  0},
	{ 0,  0,  0,  3,  1,  2,  0,  0, -1},
	{ 0, -1,  0,  0,  3,  1,  2,  0,  0},
	{ 0,  0, -1,  0,  0,  3,  1,  2,  0},
	{ 0,  0,  0, -1,  0,  0,  3,  1,  2},
	{ 0,  2,  0,  0, -1,  0,  0,  3,  1},
};

static const int direction5_tbl[9][9] = {
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 0,  1,  2,  3,  0, -1,  0,  5,  4},
	{ 0,  4,  1,  2,  3,  0, -1,  0,  5},
	{ 0,  5,  4,  1,  2,  3,  0, -1,  0},
	{ 0,  0,  5,  4,  1,  2,  3,  0, -1},
	{ 0, -1,  0,  5,  4,  1,  2,  3,  0},
	{ 0,  0, -1,  0,  5,  4,  1,  2,  3},
	{ 0,  3,  0, -1,  0,  5,  4,  1,  2},
	{ 0,  2,  3,  0, -1,  0,  5,  4,  1},
};

static const char ascii_tbl[2][2][9][6] = { // IX AB J J2
	{	// ストレート(IX = 0)
		{	// Aボタンストレート打ち
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'a','b', 0 ,'c', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'d','e', 0 ,'f', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'g','h', 0 ,'i', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'j','k', 0 ,'l', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
		{	// Bボタンストレート打ち	
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'m','n','p','o','q',},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'r','s', 0 ,'t', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'u','v', 0 ,'w', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'x','y', 0 ,'z', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
	},
	{	// クロス(IX = 1)
		{	// Aボタンクロス打ち
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'A','B', 0 ,'C', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'D','E', 0 ,'F', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'G','H', 0 ,'I', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'J','K', 0 ,'L', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
		{	// Bボタンクロス打ち
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'M','N','P','O','Q',},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'R','S', 0 ,'T', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'U','V', 0 ,'W', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'X','Y', 0 ,'Z', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
	},
};

int gbainputInit(){
    d = (struct data *)malloc(sizeof(struct data));
	if(d == NULL)
		return -1;
	return 0;
}

void gbainputFinish(){
	free(d);
}

static void btnUpdate(){
	d->k0 = REG_KEYINPUT ^ 0x03FF;
	d->k1 = (d->k0 ^ d->k3) & d->k0;
	d->k2 = (d->k0 ^ d->k3) & d->k3;
	d->k3 =  d->k0;
}

static void keytomem(){
	d->k.hold_j = (d->k0 & 0xf0) >> 4;
	d->k.hold_a = d->k0 & 0x01;
	d->k.hold_b = d->k0 & 0x02;
	d->k.push_a = d->k1 & 0x01;
	d->k.push_b = d->k1 & 0x02;
	d->k.rrse_a = d->k2 & 0x01;
	d->k.rrse_b = d->k2 & 0x02;
}

static void keytomemAB(){
	d->k.hold_I = (d->AB) ? d->k.hold_b :
							d->k.hold_a ;
	d->k.rrse_I = (d->AB) ? d->k.rrse_b :
							d->k.rrse_a ;
	d->k.push_X = (d->AB) ? d->k.push_a :
							d->k.push_b ;
}

static void command(){
	char c = ascii_tbl[d->IX][d->AB][d->J][d->J2];
	if(c){
		d->linebuf[d->linebuf_p] = c;
		d->linebuf_p++;
		d->linebuf[d->linebuf_p] = 0;
	}
}

static void mode0(){
	// 十字キー
	if(d->k.hold_j) {	
		d->mode = 1;
	}
}

static void mode1(){
	// キーチェック
	if(!(d->k.hold_j)) {
		d->mode = 0;
		return;
	}

	if(d->k.hold_a || d->k.hold_b) {
		d->J = dpadToDigit_tbl[d->k.hold_j];
		d->AB = (d->k.hold_a) ? 0 :
								1 ;
		d->mode = 2;
		return;
	}
}

static void mode2(){
	// キーチェック
	keytomemAB();
	d->J2 = direction5_tbl[d->J][dpadToDigit_tbl[d->k.hold_j]];
	if(d->J2 < 0){
		d->mode = 0;
		return;
	}

	if(d->k.rrse_I || d->k.push_X){
		d->IX = (d->k.rrse_I) ? 0 :
								1 ;
		command();
		d->mode = 3;
	}
}

static void mode3(){
	keytomemAB();
	if(!d->k.hold_I)
		d->mode = 0;
}

void debugprint(){
	mappy_dprintf("mode  = %X\n", d->mode);
	mappy_dprintf("d->J  = %X\n", d->J);
	mappy_dprintf("d->AB = %X\n", d->AB);	
	mappy_dprintf("d->IX = %X\n", d->IX);
	mappy_dprintf("d->J2 = %X\n", d->J2);
	mappy_dprintf("d->linebuf = %s\n", d->linebuf);
}

char* gbainputMain(){
	btnUpdate();
	keytomem();
	if(d->mode == 0) mode0();
	if(d->mode == 1) mode1();
	if(d->mode == 2) mode2();
	if(d->mode == 3) mode3();
	debugprint();
	return d->linebuf;
}