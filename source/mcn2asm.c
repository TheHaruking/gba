#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcn2asm.h"
///////////////////////////
static uint16_t setV1(struct asmarray*, unsigned short*);
static uint16_t setV3(struct asmarray*, unsigned short*);

///////////////////////////
// data table 
///////////////////////////
static const char head_tbl[32] = {
	//  0  1  2  3  4  5  6  7
		5, 5, 5, 6, 4, 4, 4, 4,
		4, 3, 7, 7, 6, 6, 6, 6,
		5, 5, 4, 4, 4, 4, 5, 5,
		4, 4, 3, 3, 2, 1, 3, 3,
};

static const char type_tbl[32] = {
	//  0  1  2  3  4  5  6  7
		1, 1, 1, 2, 3, 3, 3, 3,
	   20,20, 6,21, 9, 9, 9, 9,
	   10,10,11,11,12,12,22,14,
	   15,15,16,23,18, 0,19,19,
};

static const fgetop func_tbl[8][20] = {
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV1, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV1, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV1, setV1, setV1, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
	{ setV3, setV3, setV3, setV3, setV3, setV3, setV3, setV3 },
};

///////////////////////////
// static function
///////////////////////////
inline static uint16_t setV0(struct asmarray* asmcode, unsigned short *mcncode){
	uint16_t r = *mcncode & 0x0;
	*mcncode >>= 0;
	return r;
}

inline static uint16_t setV1(struct asmarray* asmcode, unsigned short *mcncode){
	uint16_t r = *mcncode & 0x1;
	*mcncode >>= 1;
	return r;
}

inline static uint16_t setV2(struct asmarray* asmcode, unsigned short *mcncode){
	uint16_t r = *mcncode & 0x3;
	*mcncode >>= 2;
	return r;
}

inline static uint16_t setV3(struct asmarray* asmcode, unsigned short *mcncode){
	uint16_t r = *mcncode & 0x7;
	*mcncode >>= 3;
	return r;
}


inline static uint16_t setV4(struct asmarray* asmcode, unsigned short *mcncode){
	uint16_t r = *mcncode & 0xf;
	*mcncode >>= 4;
	return r;
}

inline static uint16_t setV5(struct asmarray* asmcode, unsigned short *mcncode){
	uint16_t r = *mcncode & 0x1f;
	*mcncode >>= 5;
	return r;
}

inline static uint16_t setV8(struct asmarray* asmcode, unsigned short *mcncode){
	uint16_t r = *mcncode & 0xff;
	*mcncode >>= 8;
	return r;
}

inline static uint16_t setVb(struct asmarray* asmcode, unsigned short *mcncode){
	uint16_t r = *mcncode & 0x7ff;
	*mcncode >>= 11;
	return r;
}

inline static uint16_t setVf(struct asmarray* asmcode, unsigned short *mcncode){
	uint16_t r = *mcncode & 0xffff;
	*mcncode >>= 15;
	return r;
}

inline static void sethead(struct asmarray* asmcode, unsigned short mcncode){
	asmcode->head = head_tbl[mcncode >> 11];
}

//-----
inline static void setp   (struct asmarray* asmcode, unsigned short mcncode){
	asmcode->p    = asmcode->head - 1;
}

//-----
static void settyp2(struct asmarray* asmcode, unsigned short mcncode){
	switch(asmcode->type){
		case 20:
			asmcode->type =
				((mcncode & 0x0400) ? 5 : 4);
			break;
		case 21:
			asmcode->type =
				((mcncode & 0x0200) ? 8 : 7);
			break;
		case 22:
			if     (!(mcncode & 0x0F00))
				asmcode->type = 13;
			else if( (mcncode & 0x0600) == 0x0400)
				asmcode->type = 14;
			else
				asmcode->type = 0;
			break;
		case 23:
			asmcode->type =
				(((mcncode & 0x0F00)==0x0F00) ? 17 : 16);
			break;
	}
}

static void settype(struct asmarray* asmcode, unsigned short mcncode){
	asmcode->type = type_tbl[mcncode >> 11];
}

static void setcode(struct asmarray* asmcode, unsigned short mcncode){
	uint16_t mcncode_copy = mcncode;
	for(int i = 0; i < asmcode->p ; i++){
		asmcode->code[i] = func_tbl[asmcode->type][i](asmcode, &mcncode_copy);
	}
}


///////////////////////////
// external function 
///////////////////////////
void McnToAsm(struct asmarray* asmcode, unsigned short mcncode){
	sethead(asmcode, mcncode);	
	setp   (asmcode, mcncode);
	settype(asmcode, mcncode);	
	settyp2(asmcode, mcncode);
	printf("yahoo\n");
	setcode(asmcode, mcncode);
}
