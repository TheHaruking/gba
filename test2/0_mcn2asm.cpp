#include <stdint.h>
#include <stdio.h>

const uint32_t op_size[20][8]={
	{3,2,5,3,3,0,0,0,},	{3,2,1,1,3,3,3,0,}, 
	{3,2,3,8,0,0,0,0,},	{3,3,4,3,3,0,0,0,},//01-04
	{3,3,2,1,1,3,3,0,},	{3,2,3,8,0,0,0,0,},
	{3,1,1,1,1,3,3,3,},	{3,1,1,1,1,3,3,3,},//05-08
	{3,1,1,5,3,3,0,0,},	{3,1,1,5,3,3,0,0,},
	{3,1,1,3,8,0,0,0,},	{3,1,1,3,8,0,0,0,},//09-12
	{3,5,1,7,0,0,0,0,},	{3,1,1,2,1,8,0,0,},
	{3,1,1,3,8,0,0,0,},	{3,1,4,8,0,0,0,0,},//13-16
	{3,5,8,0,0,0,0,0,},	{3,2,11,0,0,0,0,0,},
	{3,1,1,11,0,0,0,0,},	{3,13,0,0,0,0,0,0,},//17-19
};

const uint32_t op_head[20] = {
	5,7,4,5,
	7,4,8,8,
	6,6,5,5,
	4,6,5,4,
	3,3,4,2
};

const int32_t op_5bit[32] = {
	 1,  1,  1,  2,	 3,  3,  3,  3, 
	-1,  6, -2, -2,	 9,  9,  9,  9,
	10, 10, 11, 11,	12, 12, -3, -3, 
	15, 15, 16, -4,	18,  0, 19, 19,	
};

const char op_name[20][64]={
	{	"ERROR!"},
	{	"Move shifted register"},
	{	"Add/substract"},
	{	"Move/compare/add/substract immediate"},
	{	"ALU operations"},
	{	"Hi register operations/branch exchange"},
	{	"PC-relative load"},
	{	"Load/store with register offset"},
	{	"Load/store sign-extended byte/halfword"},
	{	"Load/store with immediate offset"},
	{	"Load/store halfword"},
	{	"SP-relative load/store"},
	{	"Load address"},
	{	"Add offset to stack pointer"},
	{	"Push/pop registers"},
	{	"Multiple load/store"},
	{	"Conditional branch"},
	{	"Software Interrupt"},
	{	"Unconditional branch"},
	{	"Long branch with link"},
};

// test code
int main(){
	int x0, op;
	
	while(1) {
		// 1
		printf("input : ");scanf("%04x", &x0);

		// 2
		op = op_5bit[x0>>11];

		// 3
		int op2 = (op<0) ? 0 : op;
		printf("___output___\n");
//		printf("mcn     : %04X\n", x0);
		printf("op      : %d\n", op);
		printf("op_name : %s\n", op_name[op2]);
		printf("____________\n");		
	}
}