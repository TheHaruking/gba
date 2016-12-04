#ifndef _GBA_DE_ASM__MCN2ASM_
#define _GBA_DE_ASM__MCN2ASM_

struct asmarray {
	unsigned char head;		
	unsigned char p;
	signed   int  type;
	unsigned short code[7];
};

extern void McnToAsm(struct asmarray*, unsigned short);

#endif 
