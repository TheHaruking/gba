#include <stdint.h>
#include <stdio.h>


/*
	input 1 : 
			op	: 1-19
	input 2 :
		case 1:
			op_2	: 0-2
			rd	: 0-7
			rs	: 0-7
			ofs5	: 0-31
		case 2:
			op_2	: 0-1
			i	: 0-1
			(i == 0)
				rd	: 0-7
				rs	: 0-7
				rn	: 0-7
			(i == 1)
				rd	: 0-7
				rs	: 0-7
				ofs3	: 0-7
		
*/

enum {
	msk_1 = 0x0001,		msk_2 = 0x0003,
	msk_3 = 0x0007,		msk_4 = 0x000f,
	msk_5 = 0x001f,		msk_6 = 0x003f,
	msk_7 = 0x007f,		msk_8 = 0x00ff,
	msk_9 = 0x01ff,		msk_10 = 0x03ff,
	msk_11 = 0x07ff,		msk_12 = 0x0fff,
	msk_13 = 0x1fff,		msk_14 = 0x3fff,
	msk_15 = 0x7fff,		msk_16 = 0xffff,
} mask;

// test code
int main1(){
	uint32_t op;
	uint32_t op_2, rd, rs, ofs5;
	uint32_t l, word8;
	uint16_t mcn;
	
	while(1) {
		// 1
		printf("____________\n");
		printf("input : "); scanf("%d", &op);

		// 2
//		op = op_5bit[x0>>11];
		#define getbit(x, mask, n) ((x & mask) << n)
		switch (op) {
			case 1:
				printf("op_2 : ");scanf("%d", &op_2);
				printf("rd   : ");scanf("%d", &rd);
				printf("rs   : ");scanf("%d", &rs);
				printf("ofs5 : ");scanf("%d", &ofs5);
				mcn = 0x0000;
				mcn |=  getbit(0x0,  msk_3, 13) |
						getbit(op_2, msk_2, 11) |
						getbit(ofs5, msk_5,  6) |
						getbit(rs,   msk_3,  3) |
						getbit(rd,   msk_3,  0);
				break;
			case 11:
				break;
			default:
				break;
		}

		// 3
		int op2 = (op<0) ? 0 : op;
		printf("___output___\n");
		printf("mcn     : %04X\n", mcn);
	}
}

#define typedef_mcn_struct typedef struct { union { uint16_t mcn; struct 
#define mcn_format(n) }; } format_ ## n;

typedef struct { union { uint16_t mcn; struct {
	uint16_t Rd:3, Rs:3, Offset5:5, Op:2, _head:3;
}; }; } format_1;

typedef struct { union { uint16_t mcn; struct { 
	uint16_t Rd:3, Rs:3, Rn:3, Op:1, l:1, _head:5;
}; }; } format_2;

typedef struct { union { uint16_t mcn; struct { 
	uint16_t Offset8:8, Rd:3, Op:2, _head:3;
}; }; } format_3;

typedef struct { union { uint16_t mcn; struct { 
	uint16_t Rd:3, Rs:3, Op:4, _head:6;
}; }; } format_4;

typedef struct { union { uint16_t mcn; struct { 
	uint16_t Rd:3, Rs:3, H2:1, H1:1, Op:2, _head:6;
}; }; } format_5;

typedef_mcn_struct { uint16_t Word8:8, Rd:3, _head:5; }; mcn_format(6);

/*
typedef struct { union { uint16_t mcn; struct { 
	uint16_t _head:;
}; }; } format_;
*/

int main(){
	int op;
	uint16_t _bin;
	
	while(1)
	{
		printf("___input___");
		printf("op : ");
		scanf("%d", &op);
		
		_bin = 0x0000;
		switch(op)
		{
			case 1: {
				format_1* bin = (format_1*)&_bin;
				bin->Rd = 7;
				bin->Rs = 0;
				bin->Offset5 = 1;
				bin->Op = 0;
				bin->_head = 0b000;
				break;
			}
			case 6: {
				format_6* bin = (format_6*)&_bin;
				bin->Word8 = 1;
				bin->Rd = 1;
				bin->_head = 1;
				break;
			}
			default: {
				break;
			}
		}

		printf("mcn = %04X\n", _bin);
	}
}