#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcn2asm.h"
///////////////////////////
typedef unsigned short (*fgetop)(unsigned short *);
inline static unsigned short nop  (unsigned short *);
inline static unsigned short setV1(unsigned short *);
inline static unsigned short setV2(unsigned short *);
inline static unsigned short setV3(unsigned short *);
inline static unsigned short setV4(unsigned short *);
inline static unsigned short setV5(unsigned short *);
inline static unsigned short setV8(unsigned short *);
inline static unsigned short setVb(unsigned short *);
inline static unsigned short setVf(unsigned short *);

///////////////////////////
// data table 
///////////////////////////
static const char head_tbl_old[32] = {	// unused
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

static const char head_tbl[20] = {
	1,
	5, 7, 4, 5, 7, 4, 8, 8, 6, 6,
	5, 5, 4, 6, 5, 4, 3, 3, 4
};

static const char *typename_tbl[20] = {
	"UNDEFINED",
	"Move shifted register",
	"Add/subtract",
	"Move/compare/add /subtract immediate",
	"ALU operations",
	"Hi register operations /branch exchange",
	"PC-relative load",
	"Load/store with register offset",
	"Load/store sign-extended byte/halfword",
	"Load/store with immediate offset",
	"Load/store halfword",
	"SP-relative load/store",
	"Load address",
	"Add offset to stack pointer",
	"Push/pop registers",
	"Multiple load/store",
	"Conditional branch",
	"Software Interrupt",
	"Unconditional branch",
	"Long branch with link",
};

static const fgetop func_tbl[20][8] = {
	{ setVf, nop  , nop  , nop  , nop  , nop  , nop  , nop   },
	{ setV3, setV3, setV5, setV2, setV3, nop  , nop  , nop   },
	{ setV3, setV3, setV3, setV1, setV1, setV2, setV3, nop   },
	{ setV8, setV3, setV2, setV3, nop  , nop  , nop  , nop   },
	{ setV3, setV3, setV4, setV3, setV3, nop  , nop  , nop   },
	{ setV3, setV3, setV1, setV1, setV2, setV3, setV3, nop   },
	{ setV8, setV3, setV2, setV3, nop  , nop  , nop  , nop   },
	{ setV3, setV3, setV3, setV1, setV1, setV1, setV1, setV3 },
	{ setV3, setV3, setV3, setV1, setV1, setV1, setV1, setV3 },
	{ setV3, setV3, setV5, setV1, setV1, setV3, nop  , nop   },
	{ setV3, setV3, setV5, setV1, setV1, setV3, nop  , nop   },
	{ setV8, setV3, setV1, setV1, setV3, nop  , nop  , nop   },
	{ setV8, setV3, setV1, setV1, setV3, nop  , nop  , nop   },
	{ setV8, setV3, setV2, setV3, nop  , nop  , nop  , nop   },
	{ setV8, setV1, setV2, setV1, setV1, setV3, nop  , nop   },
	{ setV8, setV3, setV1, setV1, setV3, nop  , nop  , nop   },
	{ setV8, setV4, setV1, setV3, nop  , nop  , nop  , nop   },
	{ setV8, setV5, setV3, nop  , nop  , nop  , nop  , nop   },
	{ setVb, setV2, setV3, nop  , nop  , nop  , nop  , nop   },
	{ setVb, setV1, setV1, setV3, nop  , nop  , nop  , nop   },
};

///////////////////////////
// static function
///////////////////////////
inline static unsigned short nop  (unsigned short *mcncode){
	return 0;
}

inline static unsigned short setV1(unsigned short *mcncode){
	unsigned short r = *mcncode & 0x1;
	*mcncode >>= 1;
	return r;
}

inline static unsigned short setV2(unsigned short *mcncode){
	unsigned short r = *mcncode & 0x3;
	*mcncode >>= 2;
	return r;
}

inline static unsigned short setV3(unsigned short *mcncode){
	unsigned short r = *mcncode & 0x7;
	*mcncode >>= 3;
	return r;
}

inline static unsigned short setV4(unsigned short *mcncode){
	unsigned short r = *mcncode & 0xf;
	*mcncode >>= 4;
	return r;
}

inline static unsigned short setV5(unsigned short *mcncode){
	unsigned short r = *mcncode & 0x1f;
	*mcncode >>= 5;
	return r;
}

inline static unsigned short setV8(unsigned short *mcncode){
	unsigned short r = *mcncode & 0xff;
	*mcncode >>= 8;
	return r;
}

inline static unsigned short setVb(unsigned short *mcncode){
	unsigned short r = *mcncode & 0x7ff;
	*mcncode >>= 11;
	return r;
}

inline static unsigned short setVf(unsigned short *mcncode){
	unsigned short r = *mcncode & 0xffff;
	*mcncode >>= 15;
	return r;
}

inline static void sethead(struct asmarray* asmcode, unsigned short mcncode){
	asmcode->head = head_tbl[asmcode->type];
}

static void settype(struct asmarray* asmcode, unsigned short mcncode){
	asmcode->type = type_tbl[mcncode >> 11];
}

// 分類しきれなかった一部の命令を、ここで処理する。
// 未定義命令への振り分けもここ。
// ... 一部の命令 → 4-5,7-8,13-14,16-17
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

static void setcode(struct asmarray* asmcode, unsigned short mcncode){
	unsigned short mcncode_copy = mcncode;
	int i;
	for(i = 0; i < asmcode->head ; i++){
		// 関数ポインタテーブルから順次実行
		asmcode->code[i] = func_tbl[asmcode->type][i](&mcncode_copy);
	}
}


///////////////////////////
// external function 
///////////////////////////
void McnToAsm(struct asmarray* asmcode, unsigned short mcncode){
	settype(asmcode, mcncode);	
	settyp2(asmcode, mcncode);
	sethead(asmcode, mcncode);
	setcode(asmcode, mcncode);
}
