#include <stdio.h>
#include <stdlib.h>
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
	char c[256];
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

int main_Console()
{
    vramInit();
    videoInit(BG0_MAP_ADR);
    gbainputInit();
    
	d = (struct DATA*)malloc(sizeof(struct DATA));
	d->n = 0;
	d->mode = 1;
	sprintf(d->c, "d->c_MOJI.");

	while(d->mode)
	{
		scanKeys();
        d->k[0] = keysHeld();
        d->k[1] = keysDown();
        d->k[2] = keysUp();
        if(d->k[1] & 0x4){
            d->mode = 0;
        }
		
		d->n += 0x0020;
		if(d->n > 0xFFFF) d->n &= 0xFFFF;
		gbainputMain(d->c);
		McnToAsm(&(d->asmdata), d->n);
		
		move(0,0);
		gbaprintraw(d->n);
		move(0,1);
		gbaprint("[console.c]\n");
		move(0,2);
		gbaprintval(d->n);
		move(0,3);
		gbaprint(d->asmdata.name);
		move(0,4);
		gbaprint(d->c);
        move(0,5);
        gbaprintval((unsigned int)d);
		
		VBlankIntrWait();
		refresh();
	}

    videoFinish();
    gbainputFinish();
    free(d);
    return 0;
}
