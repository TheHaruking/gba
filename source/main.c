#include <stdio.h>
#include "gba.h"
#include "mcn2asm.h"
#include "gbaprint.h"
#include "res_font_table-320.h"
#include "chr001.h"

// font関連
#define FONTTILE		chr001Tiles
#define FONTTILEL		chr001TilesLen
#define FONTPAL			chr001Pal
#define FONTPALL		chr001PalLen

// BG関連
#define ALL_CHR			CHAR_BASE(2)
#define ALL_CHR_ADR		CHAR_BASE_ADR(2)
#define BG0_MAP			MAP_BASE(0)		// 0x0000
#define BG0_MAP_ADR		MAP_BASE_ADR(0)
#define BG1_MAP			MAP_BASE(4)		// 0x1000
#define BG1_MAP_ADR		MAP_BASE_ADR(4)
void zzzzzzzz(){}

const uint16_t testarray[32] = {
	0x0001,0x0002,0x0002,0x0001,0x0003,0x0003,0x0021,0x0031,
	0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,
	0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,
	0x0001,0x0001,0x0001,0x0041,0x0001,0x0001,0x0001,0x0001,
};

void testgbaprint(){
	SetMode(MODE_0 | BG0_ENABLE | BG1_ENABLE);
	REG_BG0CNT = (BG_SIZE_1 | BG0_MAP | ALL_CHR);
	REG_BG1CNT = (BG_SIZE_1 | BG1_MAP | ALL_CHR);
	int i;

	// data -> VRAM
	CpuFastSet(FONTPAL,	 (u16*)BG_PALETTE,  (FONTPALL /4) | COPY32);
	CpuFastSet(FONTTILE, (u16*)ALL_CHR_ADR, (FONTTILEL/4) | COPY32);

	CpuFastSet(testarray, (u16*)BG0_MAP_ADR, 32 | COPY16);

}

void testmcn2asm(){
	unsigned short opcode = 1111;
	struct asmarray asmcode;

	consoleDemoInit();
	printf("start\n");
	McnToAsm(&asmcode, opcode);

	int i;
	for(i = 0; i < asmcode.head; i++){
		printf("%d ", asmcode.code[i]);
	} puts("");
}



int main()
{
	//irqInit();
	//irqEnable(IRQ_VBLANK);
	//REG_IME = 1;

	testgbaprint();

	while(1)
	{
		scanKeys();
		*((volatile int *)0x03000000) += 1;
	//	VBlankIntrWait();
	}
}
