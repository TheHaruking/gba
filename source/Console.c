#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gba.h>
#include <mcn2asm.h>
#include <gbaprint.h>
#include <gbainput.h>
#include <chr003.h>

#include <Console.h>

////////////////////////////////
// define
////////////////////////////////
// マクロ
#define FONT(s)			chr003 ## s
// BG関連
#define ALL_CHR			CHAR_BASE(2)
#define ALL_CHR_ADR		CHAR_BASE_ADR(2)
#define BG0_MAP			MAP_BASE(0)		// 0x0000
#define BG0_MAP_ADR		MAP_BASE_ADR(0)
#define BG1_MAP			MAP_BASE(4)		// 0x1000
#define BG1_MAP_ADR		MAP_BASE_ADR(4)
#define BG2_MAP			MAP_BASE(8)		// 0x2000
#define BG2_MAP_ADR		MAP_BASE_ADR(8)
#define BG3_MAP			MAP_BASE(12)	// 0x3000
#define BG3_MAP_ADR		MAP_BASE_ADR(12)
void VisualStudioCode_syntaxHighlightBug_of_AfterMacro2(){}

////////////////////////////////
// 宣言_構造体
////////////////////////////////
struct DATA {
    unsigned int k[3];
	int n;
	int mode;
	char line[256];
	struct asmarray asmdata;
    char str[1024];
};

////////////////////////////////
// 宣言_変数
////////////////////////////////
static struct DATA* d;

////////////////////////////////
// 定義_データ
////////////////////////////////
/*
void Command(char c){
	switch (c){
		case ' ':		break;
		case '!':		break;
		case '\"':		break;
		case '#':		break;
		case '$':		break;
		case '%':		break;
		case '&':		break;
		case '\'':		break;
		default:		break;
	}
}
*/
enum {
	COMMAND_NONE = 0,
	COMMAND_CALL = 1,
	COMMAND_PEEK = 2,
	COMMAND_POKE = 3,
};

////////////////////////////////
// メイン
////////////////////////////////
static void vramInit(){
	// Init
	SetMode(MODE_0 | BG0_ENABLE | BG1_ENABLE);
	REG_BG0CNT = (BG_SIZE_1 | BG0_MAP | ALL_CHR);
	REG_BG1CNT = (BG_SIZE_1 | BG1_MAP | ALL_CHR);
	REG_BG2CNT = (BG_SIZE_1 | BG2_MAP | ALL_CHR);
	REG_BG3CNT = (BG_SIZE_1 | BG3_MAP | ALL_CHR);
	// data -> VRAM
	CpuFastSet(FONT(Pal),	(u16*)BG_PALETTE,  (FONT(PalLen)  /4) | COPY32);
	CpuFastSet(FONT(Tiles),	(u16*)ALL_CHR_ADR, (FONT(TilesLen)/4) | COPY32);
}

static unsigned int StrToInt(unsigned int* num,char* hex){ // command内で使う
	// 16進数以外の文字があったらreturn
	for(int i = 0; i < 8; i++){
		if(!isxdigit(*(hex+i))){
			return 0;
		}
	}
	//strncpy(buf, hex, 8);
	num* = strtol(hex, hex + 8, 16);
	return 1;
}

static void command(char* line){
	if(line[0] != '@') return;
	
	unsigned int addr;
	StrToInt(&addr, &line[1]);
	gbaprintval(*(int *)(addr));
}

int main_Console()
{
    vramInit();
    videoInit(BG0_MAP_ADR);
    gbainputInit();
    
	d = (struct DATA*)malloc(sizeof(struct DATA));
	d->n = 0;
	d->mode = 1;
	strcpy(d->line, "");

	move(0,0);
	gbaprintraw(d->n);
	move(0,1);
	gbaprint("[console.c]\n");
	move(0,2);
	gbaprintval(d->n);
	move(0,3);
	gbaprint(d->line);
	move(0,4);
	gbaprintval((unsigned int)d);

	int change_count = 0;
	int enter_count = 0;
	char moji[256];

	while(d->mode)
	{
		scanKeys();
        d->k[0] = keysHeld();
        d->k[1] = keysDown();
        d->k[2] = keysUp();
        if(d->k[1] & 0x4){
            d->mode = 0;
        }

		gbainputMain(d->line);

		if(gbainputIsChanged()){
			change_count++;
		}
		if(gbainputIsEnter()){
			enter_count++;
			strcpy(moji,"");
			command(d->line);
		}
		sprintf(moji,
			"change : %d\n"
			"enter  : %d\n"
			"S2Hex  : %d\n"
			, change_count, enter_count, StrToInt(d->line));
		move(0,0);
		gbaprint(moji);
		gbaprint(d->line);
		McnToAsm(&(d->asmdata), d->n);
		
		
		VBlankIntrWait();
		refresh();
	}

    videoFinish();
    gbainputFinish();
    free(d);
    return 0;
}
