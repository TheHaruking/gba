#include <cstring>
#include <cstdio>
#include <gba.h>
#include "Scanner.h"

// 定数
#define MODE_HOME		0x000
#define MODE_HOLDA		0x010
#define MODE_HOLDB		0x020
#define MODE_RERES		0x040	// REREASE	... そのまま離す
#define MODE_CROSS		0x080	// CROSS	... もう片方のボタンを押す
#define MODE_ENG		0x000
#define MODE_HIRA		0x100

// テーブル
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

// コンストラクタ
Scanner::Scanner(){
	memset(linebuf, 0, sizeof(linebuf));
	memset(k, 0, sizeof(k));
	linebuf_p = 0;
	linebuf_p_old = 0;
}

// 毎フレーム初期化
void Scanner::update(){
	k[0] = REG_KEYINPUT ^ 0x03FF;
	k[1] = (k[0] ^ k[3]) & k[0];
	k[2] = (k[0] ^ k[3]) & k[3];
	k[3] = k[0];
	Logic();
}

// チェック
bool Scanner::isPut(){
	return linebuf_p > linebuf_p_old;
}

bool Scanner::isBack(){
	return linebuf_p < linebuf_p_old;
}

bool Scanner::isEnter(){
	return linebuf[linebuf_p_old] == '\n';
}

// プライベート関数
void Scanner::Logic(){
	int m = mode & 0x0f0;
	if(m == MODE_HOME)
		LogicModeHOME();
	if(m & (MODE_HOLDA | MODE_HOLDB))
		LogicModeHOLD();
	if(m & (MODE_RERES | MODE_CROSS))
		LogicModeFINI();
}

void Scanner::LogicModeHOME(){
	int d = (k[0] & 0xf0) >> 4;
	int b = k[1] & 0xf;

	switch (b) {
		case KEY_A:
			mode = d | MODE_HOLDA;
			break;
		case KEY_B:
			mode = d | MODE_HOLDB;
			break;
		case KEY_START:
			// Enter
			linebuf[linebuf_p++] = '\n';
			break;
		default:
			break;
	}
}

void Scanner::LogicModeHOLD(){
	int d = (k[0] & 0xf0) >> 4;
	int d_home = mode & 0xf;
	int b = k[0] & 0x3;
	int same, dfrn;

	switch (mode & 0x030) {
		case MODE_HOLDA:
			same = KEY_A; dfrn = KEY_B;
			break;
		case MODE_HOLDB:
			same = KEY_B; dfrn = KEY_A;
			break;
		default:
			break;
	}

	if(!(k[0]&same)){
		mode = MODE_RERES;
	}
	if(  k[1]&dfrn ){
		mode = MODE_CROSS;
	}
}

void Scanner::LogicModeFINI(){
	int d = (k[0] & 0xf0) >> 4;
	int b = k[1] & 0x3;
	linebuf[linebuf_p++] = 'A';
	mode = MODE_HOME;
}

// パブリック関数
int Scanner::scanf(char* str){
	printf("a");
	do {
		update();
		if(isPut()){
			*str = getc();
			str++;
		}
		// View
		if(isPut()){
			printf("%c", getc());
		}
	} while(!isEnter());
	return linebuf_p;
}

char* Scanner::getLine(){
	return linebuf;
}

char  Scanner::getc(){
	return linebuf[linebuf_p_old];
}
