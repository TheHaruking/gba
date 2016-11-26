#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct asmarray {
	unsigned char head;
	unsigned char _unused;
	unsigned short code[7];
};

const char head_tbl[32] = {
	5, 5, 5, 6, 4, 4, 4, 4,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
};

void McnToAsm(struct asmarray* asmcode, unsigned short mcncode){
	asmcode->head = head_tbl[mcncode >> 11];
	
}

void TestMcnToAsm(){

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

	mcncode = 0x1234;
	asmcode.head = 0;
	memset(asmcode.code, 0, 7);
	PrintMcnAndAsm(mcncode, asmcode);
	McnToAsm(&asmcode, mcncode);
	PrintMcnAndAsm(mcncode, asmcode);
	return 0;
}
