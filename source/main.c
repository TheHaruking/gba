#include <stdio.h>
#include <stdlib.h>
#include <gba.h>
#include <mcn2asm.h>
#include <gbaprint.h>
#include <gbainput.h>
#include <chr003.h>

#include <Console.h>
#include <MemoryViewer.h>
////////////////////////////////
// 宣言_関数
////////////////////////////////
static void vramInit();
static void go(int (*)(void));

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
void VisualStudioCode_syntaxHighlightBug_of_AfterMacro1(){}

////////////////////////////////
// 宣言_構造体
////////////////////////////////
struct DATA {
	unsigned int k[3];
	int n;
	int mode;
	char c[256];
	struct asmarray asmdata;
	int (*next)(void);
};

////////////////////////////////
// 宣言_変数
////////////////////////////////
struct DATA* d;

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

static void go(int (*func)(void)){
	videoFinish();
	(*func)();
	vramInit();
	videoInit(BG0_MAP_ADR);
}

char* Reverse(char* src){
	static char dst[256];	// 逆にした文字を入れる配列
	int len	= 0;
	int i;				// 文字の長さ
	
	for(i = 0; *(src + i) != '\0'; i++){
		len = i;
	}

	int j = len;
	for(i = 0; i < len; i++){
		dst[i] = *(src + j);
		j--;
	}

	dst[len] = '\0';
	return dst;
}

int main()
{
	// 初期化
	irqInit();
	irqEnable(IRQ_VBLANK);
	REG_IME = 1;

	vramInit();
	videoInit(BG0_MAP_ADR);

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
        switch(d->k[1]){
			case 0x01: go(main_Console);		break;
			case 0x02: go(main_MemoryViewer);	break;
			default:   d->next = NULL;			break;
        }

		move(0,0);
		gbaprint("[main.c]\n");
		move(0,1);
		gbaprintval((unsigned int)d);
		move(0,2);
		gbaprintval(keysHeld());
		move(0,3);
		gbaprint(Reverse("abcde"));

		
		VBlankIntrWait();
		refresh();
	}
	
	free(d);
	videoFinish();
	SoftReset(RAM_RESTART);
	return 0;
}

