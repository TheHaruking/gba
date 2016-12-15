#include "gba.h"
#include "Input.h"

void InpUpdate(struct Inp){
	k[0] = REG_KEYINPUT ^ 0x03FF;
	k[1] = (k[0] ^ k[3]) & k[0];
	k[2] = (k[0] ^ k[3]) & k[3];
	k[3] = k[0];
}

