#include <stdlib.h>
#include <gba.h>
#include "gbainput.h"
#include <mappy.h>
////////////////////////////////
// memo
////////////////////////////////
/*
170107
	十字ホールド → Aホールド → 十字向き変えホールド → A離す から
	十字ホールド → 十字向き変えホールド → Aホールド → A離す に 変更
*/

////////////////////////////////
// define
////////////////////////////////
#define MAX_LINEBUF			256

////////////////////////////////
// 宣言_関数
////////////////////////////////
extern int   gbainputInit();
extern void  gbainputFinish();
extern char* gbainputMain(char*);
static void btnUpdate(void);
static void keytomem(void);
static void UpdateModeKey(void);
static void chartobuf(char);
static void mode0(void);
static void mode1(void);
static void mode2(void);
static void mode3(void);
static void command(void);
static void mode10(void);
static void mode11(void);
static void command1X(void);
static void mode20(void);
//static void mode21(void);
//static void command2X(void);
static void debugprint(void);

////////////////////////////////
// 宣言_構造体
////////////////////////////////
struct keys {
	unsigned int hold_j;
	unsigned int hold_a, hold_b;
	unsigned int push_a, push_b;
	unsigned int rrse_a, rrse_b;
	unsigned int hold_a_and_b, hold_a_orr_b;
	unsigned int hold_I, hold_X, rrse_I, push_X, hold_Z;
};

struct data {
    unsigned int k0, k1, k2, k3;
    char linebuf[MAX_LINEBUF];
    int linebuf_p;
	int mode, J, AB, IX, J2;	// 日本語・記号 切り替えもその内ココへ
	int X_done;					// クロス打ち後の2重打ち予防
	int direction;
	int chr_pointer;
	int y;
	struct keys k;
};

////////////////////////////////
// 宣言_変数
////////////////////////////////
static struct data *d;

////////////////////////////////
// 定義_データ
////////////////////////////////
static const int dpadToDigit_tbl[16] = { // 右回り, ↑開始
//	     R   L
	 0,  3,  7,  9,
	 1,  2,  8,  9, // U
	 5,  4,  6,  9, // D
	 9,  9,  9,  9,
};

static const int direction8_tbl[10][10] = { // 右回り, ↑開始
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 0},
	{ 0, 8, 1, 2, 3, 4, 5, 6, 7, 0},
	{ 0, 7, 8, 1, 2, 3, 4, 5, 6, 0},
	{ 0, 6, 7, 8, 1, 2, 3, 4, 5, 0},
	{ 0, 5, 6, 7, 8, 1, 2, 3, 4, 0},
	{ 0, 4, 5, 6, 7, 8, 1, 2, 3, 0},
	{ 0, 3, 4, 5, 6, 7, 8, 1, 2, 0},
	{ 0, 2, 3, 4, 5, 6, 7, 8, 1, 0},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static const int dpadToDigit_tbl2[16] = { // 対照 ↑優先 ←優先
//	     R   L
	 0,  4,  3,  9,
	 1,  6,  5,  9, // U
	 2,  8,  7,  9, // D
	 9,  9,  9,  9,
};

static const int direction8_tbl2[10][10] = { // 対照、↑中央
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 4, 5, 6, 7, 8, 1, 2, 3, 0}, // 1
	{ 0, 8, 7, 6, 5, 4, 3, 2, 1, 0}, // 5
	{ 0, 2, 1, 8, 7, 6, 5, 4, 3, 0}, // 7
	{ 0, 2, 3, 4, 5, 6, 7, 8, 1, 0}, // 3
	{ 0, 3, 2, 1, 8, 7, 6, 5, 4, 0}, // 8
	{ 0, 3, 4, 5, 6, 7, 8, 1, 2, 0}, // 2
	{ 0, 1, 8, 7, 6, 5, 4, 3, 2, 0}, // 6
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 0}, // 4
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static const char ascii_tbl[2][2][9][9] = { // IX AB J J2
	{	// ストレート(IX = 0)
		{	// Aボタンストレート打ち
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 ,'!','a','b','c','?', 0 , 0 ,},
			{ 0 , 0 ,'(','d','e','f',')', 0 , 0 ,},
			{ 0 ,'#', 39,'g','h','i',',', 0 , 0 ,}, // ''
			{ 0 , 0 , 34,'j','k','l','.', 0 , 0 ,},	// "" 
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
		{	// Bボタンストレート打ち	
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 ,'m','n','o','p','q', 0 , 0 ,},
			{ 0 , 0 ,'<','r','s','t','>', 0 , 0 ,},
			{ 0 ,'%','+','u','v','w','-', 0 , 0 ,},
			{ 0 , 0 , 92,'x','y','z','/', 0 , 0 ,}, // '\''
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
	},
	{	// クロス(IX = 1)
		{	// Aボタンクロス打ち
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 ,'@','A','B','C','`', 0 , 0 ,},
			{ 0 , 0 ,'{','D','E','F','}', 0 , 0 ,},
			{ 0 ,'$','^','G','H','I',';', 0 , 0 ,},
			{ 0 , 0 ,'~','J','K','L',':', 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
		{	// Bボタンクロス打ち
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 ,'M','N','O','P','Q', 0 , 0 ,},
			{ 0 , 0 ,'[','R','S','T',']', 0 , 0 ,},
			{ 0 ,'&','*','U','V','W','=',' ', 0 ,},
			{ 0 , 0 ,'|','X','Y','Z','_', 0 , 0 ,},	// '\'
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
	},
};

////////////////////////////////
// 定義_関数
////////////////////////////////
//------------------------------
// 初期化関連
//------------------------------
int gbainputInit(){
    d = (struct data *)malloc(sizeof(struct data));
	if(d == NULL)
		return -1;
	return 0;
}

void gbainputFinish(){
	free(d);
}

//------------------------------
// 入力関連
//------------------------------
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
	d->k.hold_a_and_b = (d->k0 & 0x03) == 0x03;
	d->k.hold_a_orr_b = (d->k0 & 0x03);
}

static void UpdateModeKey(){
	d->k.hold_I = (d->AB) ? d->k.hold_b :
							d->k.hold_a ;
	d->k.hold_X = (d->AB) ? d->k.hold_a :
							d->k.hold_b ;
	d->k.rrse_I = (d->AB) ? d->k.rrse_b :
							d->k.rrse_a ;
	d->k.push_X = (d->AB) ? d->k.push_a :
							d->k.push_b ;
	d->k.hold_Z = (d->IX) ? d->k.hold_X :
							d->k.hold_I ;
}

//------------------------------
// 共通関数
//------------------------------
static void chartobuf(char c){
	if(d->linebuf_p >= MAX_LINEBUF - 1){
		return;
	}
	d->linebuf[d->linebuf_p] = c;
	d->linebuf_p++;
	d->linebuf[d->linebuf_p] = 0;
}

//------------------------------
// 十字キー ホールド
//------------------------------
static void mode0(){
	d->J = dpadToDigit_tbl2[d->k.hold_j];

	if(d->k.hold_j) {	
		d->mode = 1;
		return;
	}

	if(d->k.hold_a){
		d->mode = 0x10;
		return;
	}

	if(d->k.hold_b){
		d->mode = 0x20;
		return;
	}
}

static void mode1(){
	UpdateModeKey();
	// チェック
	if(!d->k.hold_j) {
		d->mode = 0;
		return;
	}

	// チェック2
	if(d->k.hold_a_and_b){
		d->mode = 0;
		return;
	}

	// AB離していたらクロス押したフラグリセット
	if(!d->k.hold_a_orr_b){
		d->X_done = 0;
	}

	// メイン
	if(d->k.hold_a_orr_b) {
		d->AB = (d->k.hold_b) ? 1 :
								0 ;
		d->mode = 2;
	}
}

static void mode2(){
	UpdateModeKey();
	// チェック
	if(d->J2 < 0){
		d->mode = 0;
		return;
	}
	// チェック2 - クロス打ち後2重打ち対策
	if(d->X_done && d->k.rrse_I){
		d->mode = 1;
		return;
	}
	// メイン
	if(d->k.rrse_I || d->k.push_X){
		d->J2 = direction8_tbl2[d->J][dpadToDigit_tbl[d->k.hold_j]];
		d->IX = (d->k.push_X) ? 1 :
								0 ;
		command();
		d->X_done = d->IX;
		d->mode = 3;
	}
}

static void command(){ // 文字挿入！
	char c = ascii_tbl[d->IX][d->AB][d->J][d->J2];
	if(c){
		chartobuf(c);
	}
}

static void mode3(){ // ストッパー
	UpdateModeKey();
	// メイン
	if(!d->k.hold_Z){
		d->mode = 1;
	}
}

//------------------------------
// A ボタン ホールド
//------------------------------
static void mode10(){
	// チェック
	if(!d->k.hold_a){
		d->mode = 0;
		return;
	}

	if(d->k1 & 0xB0){
		command1X();
		d->mode = 0x11;
	}
}

static void command1X(){
	if(d->k1 & 0x10){ // →
		chartobuf(' ');
	}
	if(d->k1 & 0x20){ // ←
		// backspace
		if(d->linebuf_p <= 0){
			return;
		}
		d->linebuf_p--;
		d->linebuf[d->linebuf_p] = 0;
	}
	if(d->k1 & 0x80){ // ↓
		chartobuf('\n');
	}

}

// ストッパー
static void mode11(){
	if(!(d->k0 & 0x30)){
		d->mode = 0x10;
	}	
}


//------------------------------
// B ボタン ホールド // 未使用
//------------------------------
static void mode20(){
	// チェック
	if(!d->k.hold_b){
		d->mode = 0;
		return;
	}
}
/******************
static void mode21(){

}

static void command2X(){

}
*********************/
//------------------------------
// メイン関数
//------------------------------
static void debugprint(){
	mappy_dprintf("mode  = %X\n", d->mode);
	mappy_dprintf("d->J  = %X\n", d->J);
	mappy_dprintf("d->AB = %X\n", d->AB);	
	mappy_dprintf("d->IX = %X\n", d->IX);
	mappy_dprintf("d->J2 = %X\n", d->J2);
	mappy_dprintf("d->linebuf = %s\n", d->linebuf);
}

char* gbainputMain(char* c){
	btnUpdate();
	keytomem();

	if(d->mode == 0) mode0();
	// 十字
	if(d->mode == 1) mode1();
	if(d->mode == 2) mode2();
	if(d->mode == 3) mode3();
	// AB
	if(d->mode == 0x10) mode10();
	if(d->mode == 0x11) mode11();
	if(d->mode == 0x20) mode20();
	
	debugprint();

	DMA3COPY((u16*)d->linebuf, (u16*)c , (MAX_LINEBUF >> 1) | COPY16);
	return d->linebuf;
}