#include <stdio.h>
#include "gba.h"
#include "mcn2asm.h"
#include "gbaprint.h"
#include "res_font_table-320.h"
#include "chr001.h"

#define FONTTILE	chr001Tiles
#define FONTTILEL	chr001TilesLen
#define FONTPAL	chr001Pal
#define FONTPALL	chr001PalLen

void testgbaprint(){
	SetMode(MODE_0 | BG0_ENABLE);
	REG_BG0CNT = (BG_SIZE_1 | CHAR_BASE(2));
	
	int i;
	for(i = 0; i < (FONTTILEL >> 2) ; i++){
		*(unsigned int *)(CHAR_BASE_ADR(2) + i * 4) = FONTTILE[i];
	}
	
	for(i = 0; i < (FONTPALL >> 1) ; i++ ){
		*(BG_PALETTE + i) = FONTPAL[i];
	}
	
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

	testgbaprint();

	while(1)
	{
		scanKeys();
	}
}
