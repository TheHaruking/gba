#include <stdlib.h>
#include <gba.h>
#include "gbainput.h"

struct data {
    u16 k0, k1, k2, k3;
    char linebuf[256];
    int linebuf_p;
	int linebuf_p_old;
	int mode;
	int direction;
	int chr_pointer;
	int y;
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
	{ 0,  2,  0, -1,  0,  5,  4,  1,  2},
	{ 0,  2,  3,  0, -1,  0,  5,  4,  1},
};

static void btnUpdate(){
	d->k0 = REG_KEYINPUT ^ 0x03FF;
	d->k1 = (d->k0 ^ d->k3) & d->k0;
	d->k2 = (d->k0 ^ d->k3) & d->k3;
	d->k3 = d->k0;
}

void gbainputInit(){
    d = (struct data *)malloc(sizeof(struct data));
}
