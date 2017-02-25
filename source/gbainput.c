#include <stdlib.h>
#include <gba.h>
#include <mappy.h>
#include "gbainput.h"
////////////////////////////////
// memo
////////////////////////////////
/*
170107
	十字ホールド → Aホールド → 十字向き変えホールド → A離す から
	十字ホールド → 十字向き変えホールド → Aホールド → A離す に 変更
170108
	ファミコン、GBにも応用できるよう 十字キー＋AB のみで できるようにしていたが、
	LR も 追加。
	それにともない、クロス打ちからRシフトに変更
*/

////////////////////////////////
// define
////////////////////////////////
#define MAX_LINEBUF			256
#define DEBUG

////////////////////////////////
// 宣言_関数
////////////////////////////////
extern int   gbainputInit(void);
extern void  gbainputFinish(void);
extern char* gbainputMain(char*);
extern int	 gbainputIsEnter(void);
extern int	 gbainputIsChanged(void);

static void btnUpdate(void);
static void keytomem(void);
static void UpdateModeKey(void);
static void initStatus(void);
static void chartobuf(char);
inline static int isTokeiMawari(void);

static void mode0(void);
static void mode1(void);
static void mode2(void);
static void command(void);
static void mode_stopper(void);

static void mode10(void);
static void command1X(void);
static void mode1X_stopper(void);

static void mode20(void);
//static void mode21(void);
//static void command2X(void);
static void mode2X_stopper(void);

static void mode30(void);
//static void mode31(void);
//static void command3X(void);
static void mode3X_stopper(void);

static void mode40(void);
//static void mode41(void);
//static void command4X(void);
static void mode4X_stopper(void);

static void mode50(void);
//static void mode51(void);
//static void command5X(void);
static void mode5X_stopper(void);

static void mode60(void);
//static void mode61(void);
//static void command6X(void);
static void mode6X_stopper(void);

static void debugprint(void);

////////////////////////////////
// 宣言_構造体
////////////////////////////////
struct keys {
	unsigned int hold_j, hold_a, hold_b, hold_l, hold_r, hold_start;
	unsigned int push_j, push_a, push_b, push_l, push_r;
	unsigned int rrse_j, rrse_a, rrse_b, rrse_l, rrse_r;
	unsigned int hold_a_and_b, hold_a_orr_b;
	unsigned int hold_r_and_l, hold_r_orr_l;
	unsigned int hold_I, hold_X, rrse_I, push_X, hold_Z;
};

struct data {
    unsigned int k0, k1, k2, k3;
    char	linebuf[MAX_LINEBUF];
    int		linebuf_p;
	int		mode, J, AB, IX, J2, R_SHIFT;	// 日本語・記号 切り替えもその内ココへ
	int		X_done;					// クロス打ち後の2重打ち予防
	int		direction;
	int		chr_pointer;
	int		y;
	struct	keys k;
	int		is_change;
	int		is_enter;
	int		select;
};

////////////////////////////////
// 宣言_変数
////////////////////////////////
static struct data *d;

////////////////////////////////
// 定義_データ
////////////////////////////////
enum {
	SELECT_ALPH,	SELECT_SYMBOL,
	SELECT_HIRA,	SELECT_KANA,
	SELECT_KANJI,	SELECT_SPECIAL,
	SELECT_EMOJI,	SELECT_EMOJI2
};

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

static const char ascii_tbl[2][2][2][9][9] = { // R_SHIFT AB J J2
{	// SELECT_ALPH
	{	// シフトなし
		{	// Aボタン
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'!','!','b','a','c','?','?', 0 ,},
			{ 0 ,'(','(','e','d','f',')',')', 0 ,},
			{ 0 , 39, 39,'h','g','i',',',',', 0 ,}, // ''
			{ 0 , 34, 34,'k','j','l','.','.', 0 ,},	// "" 
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
		{	// Bボタン	
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'p','p','n','m','o','q','q', 0 ,},
			{ 0 ,'<','<','s','r','t','>','>', 0 ,},
			{ 0 ,'+','+','v','u','w','-','-', 0 ,},
			{ 0 , 92, 92,'y','x','z','/','/', 0 ,}, // '\''
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
	},
	{	// シフトあり
		{	// Aボタン
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'@','@','B','A','C','`','`', 0 ,},
			{ 0 ,'{','{','E','D','F','}','}', 0 ,},
			{ 0 ,'^','^','H','G','I',';',';', 0 ,},
			{ 0 ,'~','~','K','J','L',':',':', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
		{	// Bボタン
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'P','P','N','M','O','Q','Q', 0 ,},
			{ 0 ,'[','[','S','R','T',']',']', 0 ,},
			{ 0 ,'*','*','V','U','W','=','=', 0 ,},
			{ 0 ,'|','|','Y','X','Z','_','_', 0 ,},	// '\'
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
	},
},
{	// SELECT_SYMBOL
	{	// シフトなし
		{	// Aボタン
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'!','!','1','0','1','?','?', 0 ,},
			{ 0 ,'(','(','3','2','3',')',')', 0 ,},
			{ 0 , 39, 39,'5','4','5',',',',', 0 ,}, // ''
			{ 0 , 34, 34,'7','6','7','.','.', 0 ,},	// "" 
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
		{	// Bボタン	
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,' ',' ','9','8','9',' ',' ', 0 ,},
			{ 0 ,'<','<','B','A','B','>','>', 0 ,},
			{ 0 ,'+','+','D','C','D','-','-', 0 ,},
			{ 0 , 92, 92,'F','E','F','/','/', 0 ,}, // '\''
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
	},
	{	// シフトあり
		{	// Aボタン
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'@','@','b','a','c','`','`', 0 ,},
			{ 0 ,'{','{','e','d','f','}','}', 0 ,},
			{ 0 ,'^','^','h','g','i',';',';', 0 ,},
			{ 0 ,'~','~','k','j','l',':',':', 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
		{	// Bボタン
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 ,'p','p','n','m','o','q','q', 0 ,},
			{ 0 ,'[','[','s','r','t',']',']', 0 ,},
			{ 0 ,'*','*','v','u','w','=','=', 0 ,},
			{ 0 ,'|','|','y','x','z','_','_', 0 ,},	// '\'
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,},
		},
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
// is関連
//------------------------------
inline int gbainputIsChanged(){
	return d->is_change;
}

inline int gbainputIsEnter(){
	return d->is_enter;
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
	d->k.push_j = (d->k1 & 0xf0) >> 4;
	d->k.rrse_j = (d->k2 & 0xf0) >> 4;
	d->k.hold_a = d->k0 & 0x01;
	d->k.hold_b = d->k0 & 0x02;
	d->k.push_a = d->k1 & 0x01;
	d->k.push_b = d->k1 & 0x02;
	d->k.rrse_a = d->k2 & 0x01;
	d->k.rrse_b = d->k2 & 0x02;
	d->k.hold_a_and_b = (d->k0 & 0x03) == 0x03;
	d->k.hold_a_orr_b = (d->k0 & 0x03);
	// LR追加
	d->k.hold_r = d->k0 & 0x0100;
	d->k.hold_l = d->k0 & 0x0200;
	d->k.push_r = d->k1 & 0x0100;
	d->k.push_l = d->k1 & 0x0200;
	d->k.rrse_r = d->k2 & 0x0100;
	d->k.rrse_l = d->k2 & 0x0200;
	d->k.hold_r_and_l = (d->k0 & 0x0300) == 0x0300;
	d->k.hold_r_orr_l = (d->k0 & 0x0300);
	// START追加
	d->k.hold_start   = d->k0 & 0x08;
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

static void initStatus(){
	d->is_change	= 0;
	d->is_enter		= 0;
}

inline static int isTokeiMawari(void){
	return d->select == SELECT_SYMBOL;
}

//------------------------------
// 十字キー ホールド
//------------------------------
static void mode0(){
	d->J = dpadToDigit_tbl2[d->k.hold_j];

	if(d->k.hold_j) {	
		d->mode = 1; return;
	}

	switch(d->k.hold_l){
		//case 0x100: d->mode = 0x30; return;
		case 0x200: d->mode = 0x40; return;
		//case 0x300: d->mode = 0x50; return;
		default: break;
	}

	switch(d->k.hold_a_orr_b){
		case 0x1: d->mode = 0x10; return;
		case 0x2: d->mode = 0x20; return;
		default: break;
	}

	if(d->k.hold_start){
		d->mode = 0x60; return;
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
	if(d->k.push_a || d->k.push_b){ // if(d->k.rrse_I || d->k.push_X){ // <- クロス打ちの場合
		d->J2 = direction8_tbl2[d->J][dpadToDigit_tbl[d->k.hold_j]];
		d->IX		= (d->k.push_X) ?	1 :
										0 ;
		d->R_SHIFT	= (d->k.hold_r) ?	1 :
										0 ;
		command();
		d->X_done = d->IX;
		d->mode = 15;
	}
}

static void command(){ // 文字挿入！
	char c = ascii_tbl[d->select][d->R_SHIFT][d->AB][d->J][d->J2];
	if(c){
		chartobuf(c);
		// 変更フラグON
		d->is_change = 1;
	}
}

static void mode_stopper(){ // ストッパー
	UpdateModeKey();
	// メイン
	if(!d->k.hold_Z){
		d->mode = 1;
	}
}

//------------------------------
// A ボタン ホールド	// 改行 スペース 削除
//------------------------------
static void mode10(){
	// チェック
	if(!d->k.hold_a){
		d->mode = 0;
		return;
	}

	if(d->k1 & 0xB0){
		command1X();
		d->mode = 0x1f;
	}
}

static void command1X(){
	if(d->k1 & 0x10){ // →
		chartobuf(' ');
		// 変更フラグON
		d->is_change = 1;
	}
	if(d->k1 & 0x20){ // ←
		// backspace
		if(d->linebuf_p <= 0){
			return;
		}
		d->linebuf_p--;
		d->linebuf[d->linebuf_p] = 0;
		// 変更フラグON
		d->is_change = 1;
	}
	if(d->k1 & 0x80){ // ↓
		chartobuf('\n');
		// 確定フラグON
		d->is_enter = 1;
	}
}

static void mode1X_stopper(){
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

static void mode2X_stopper(){
	if(1){
		d->mode = 0x20;
	}
}

//------------------------------
// R ボタン ホールド // 未使用
//------------------------------
static void mode30(){
	// チェック
	if(!d->k.hold_r){
		d->mode = 0;
		return;
	}
}
/******************
static void mode21(){

}
static void command3X(){

}
*********************/

static void mode3X_stopper(){
	if(1){
		d->mode = 0x30;
	}
}

//------------------------------
// L ボタン ホールド // 未使用
//------------------------------
static void mode40(){
	// チェック
	if(!d->k.hold_l){
		d->mode = 0;
		return;
	}
}
/******************
static void mode41(){

}

static void command4X(){

}
*********************/

static void mode4X_stopper(){
	if(1){
		d->mode = 0x40;
	}
}

//------------------------------
// L&R ボタン ホールド // 未使用
//------------------------------
static void mode50(){
	// チェック
	if(!d->k.hold_r_and_l){
		d->mode = 0;
		return;
	}
}
/******************
static void mode51(){

}

static void command5X(){

}
*********************/

static void mode5X_stopper(){
	if(1){
		d->mode = 0x50;
	}
}

//------------------------------
// START ボタン ホールド
//------------------------------
static void mode60(){
	// チェック
	if(!d->k.hold_start){
		d->mode = 0;
		return;
	}

	// select
	switch(d->k.hold_j){
		case 0x1: d->select = SELECT_ALPH;		break;
		case 0x2: d->select = SELECT_SYMBOL;	break;
		default: break;
	}
}
/******************
static void mode51(){

}

static void command5X(){

}
*********************/

static void mode6X_stopper(){
	if(1){
		d->mode = 0x60;
	}
}
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
	initStatus();
	keytomem();

	// ホーム
	if(d->mode == 0) mode0();

	// 十字
	if(d->mode == 1) mode1();
		if(d->mode == 2) mode2();
			if(d->mode == 15) mode_stopper();
	// AB
	if(d->mode == 0x10) mode10();
		if(d->mode == 0x1f) mode1X_stopper();
	if(d->mode == 0x20) mode20();
		if(d->mode == 0x2f) mode2X_stopper();
	
	// LR
	if(d->mode == 0x30) mode30();
		if(d->mode == 0x3f) mode3X_stopper();
	if(d->mode == 0x40) mode40();
		if(d->mode == 0x4f) mode4X_stopper();
	if(d->mode == 0x50) mode50();
		if(d->mode == 0x5f) mode5X_stopper();

	// START
	if(d->mode == 0x60) mode60();
		if(d->mode == 0x6f) mode6X_stopper();
	
	#ifdef    DEBUG
	debugprint();
	#endif // DEBUG

	DMA3COPY((u16*)d->linebuf, (u16*)c , (MAX_LINEBUF >> 1) | COPY16);
	return d->linebuf;
}