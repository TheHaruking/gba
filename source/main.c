#include <stdio.h>
#include <stdlib.h>
#include <gba.h>
#include <mcn2asm.h>
#include <gbaprint.h>
#include <gbainput.h>
#include <chr003.h>

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
void zzzzzzzz(){}

// Data
const u16 syobonkun[7] = {	0x68, 0xAE, 0x60, 0xAD, 0x60, 0xAF, 0x69 };

void testgbaprint(){ CpuFastSet(syobonkun, (u16*)BG0_MAP_ADR, sizeof(syobonkun) / 2 | COPY16); }

void vramInit(){
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

int main()
{
	irqInit();
	irqEnable(IRQ_VBLANK);
	REG_IME = 1;

	vramInit();
	videoInit(BG0_MAP_ADR);
	gbainputInit();
	testgbaprint();

	int n = 0;

	while(1)
	{
		//scanKeys();
		n++;
		move(2,2);
		testprintval2(n);
		move(2,3);
		gbaprint(gbainputMain());
		gbaprint("moji");
		refresh();

		*((u16*)BG0_MAP_ADR + 33) = n & 0xff;
		VBlankIntrWait();
	}
	
	videoFinish();
}
