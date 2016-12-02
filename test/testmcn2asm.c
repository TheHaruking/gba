#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/mcn2asm.h"

static void PrintMcnAndAsm(unsigned short mcncode, struct asmarray asmcode){
	printf("mcn = %04X\n", mcncode);
	printf("asm = ");
	for(int i = 0; i < asmcode.head; i++){
		printf("%d ", asmcode.code[i]);
	} printf("\n");
}


int main(){
	unsigned short mcncode;
	struct asmarray asmcode;

	while(1){
		printf("mcncode? : ");
		scanf("%4hx", &mcncode) ;
		asmcode.head = 0;
		memset(asmcode.code, 0, 7);

		PrintMcnAndAsm(mcncode, asmcode);
		McnToAsm(&asmcode, mcncode);
		PrintMcnAndAsm(mcncode, asmcode);
	}
	return 0;
}
