#include "gba.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Input.h"
#include "Scanner.h"
#include "gbascanf.h"
extern "C" { 
#include "mcn2asm.h" 
}

void test(){
	uint16_t opcode = 1111;
	struct asmarray asmcode;
	McnToAsm(&asmcode, opcode);
	for(int i = 0; i < asmcode.head; i++){
		printf("%d ", asmcode.code[i]);
	} puts("");
}

int main()
{
	Input i;
	Scanner s;
	char buf[256];

	consoleDemoInit();
	printf("start\n");
	test();

	while(1)
	{
	// Input
		i.update();

	// preModel
		//s.update();

	// Model
		s.scanf(buf);

	// vModel


	// View
		printf("%s", buf);
	}
}
