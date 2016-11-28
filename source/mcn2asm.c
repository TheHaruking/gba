#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct asmarray {
	unsigned char head;
	unsigned char _unused;
	unsigned short code[7];
};

const char head_tbl[32] = {
//  0  1  2  3  4  5  6  7
	5, 5, 5, 6, 4, 4, 4, 4,
	4, 3, 7, 7, 6, 6, 6, 6,
	5, 5, 4, 4, 4, 4, 5, 5,
	4, 4, 3, 3, 2, 1, 3, 3,
};

void sethead(struct asmarray* asmcode, unsigned short mcncode){
	asmcode->head = head_tbl[mcncode >> 11];
}

void setcode(struct asmarray* asmcode, unsigned short mcncode){
	asmcode->code
}

void McnToAsm(struct asmarray* asmcode, unsigned short mcncode){
	sethead(asmcode, mcncode);	
}

void PrintMcnAndAsm(unsigned short mcncode, struct asmarray asmcode){
	printf("mcn = %04X\n", mcncode);
	printf("asm = ");
	for(int i = 0; i < asmcode.head; i++){
		printf("%d ", asmcode.code[i]);
	} printf("\n");
}


int main(){
	unsigned short mcncode;
	struct asmarray asmcode;

	mcncode = 0x5000;
	asmcode.head = 0;
	memset(asmcode.code, 0, 7);
	PrintMcnAndAsm(mcncode, asmcode);
	McnToAsm(&asmcode, mcncode);
	PrintMcnAndAsm(mcncode, asmcode);
	return 0;
}
