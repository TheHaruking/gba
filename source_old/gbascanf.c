
#include <gba.h>
#include <stdio.h>
#include "gbaScanf.h"

#define UP_HOLD 0x40
#define DN_HOLD 0x80
#define RT_HOLD 0x10
#define LT_HOLD 0x20

static const int dpadToDigit_tbl[16] = {
//	     R   L
	 0,  3,  7, -1,
	 1,  2,  8, -1, // U
	 5,  4,  6, -1, // D
	-1, -1, -1, -1,
};

static void btnInit(unsigned short *btn){
	btn[0] = REG_KEYINPUT ^ 0x03FF;
	btn[1] = (btn[0] ^ btn[3]) & btn[0];
	btn[2] = (btn[0] ^ btn[3]) & btn[3];
	btn[3] = btn[0];
}

int gbaScanf(char buf[]){
	unsigned short btn[4] = {0};
	int dpad;
	int a_btn, b_btn;
	int bp = 0;
	do {
		btnInit(btn);
		dpad	= (btn[0]&DPAD ) >> 4;
		a_btn	= (btn[1]&KEY_A) > 0;
		b_btn	= (btn[1]&KEY_B) > 0;
		if(a_btn || b_btn){
			if(dpad > 0){
				buf[bp] = (dpadToDigit_tbl[dpad]-1) * 2 + 'A' + b_btn;
				printf("%c", buf[bp++]);
			} else {
				if(a_btn){
					buf[bp] = ' ';
					printf("%c",buf[bp++]);
				}
				if(b_btn){
					if(bp){
						printf(CON_LEFT(1));
						printf(" ");
						printf(CON_LEFT(1));
						bp--;
					}
				}
			}
		}
	} while (!(btn[1]&0x08));
	buf[bp] = 0;
	printf("\n");
	return bp;
}
