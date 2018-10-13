#include <stdint.h>
#include <stdio.h>

#define typedef_mcn_struct typedef struct { union { uint16_t mcn; struct 
#define format_N(n) }; } format_##n;

typedef_mcn_struct { uint16_t _head:16; };									format_N(0);
// no use macro...
typedef struct { union { uint16_t mcn; struct {		// typedef_mcn_struct
	uint16_t Rd:3, Rs:3, Offset5:5, Op:2, _head:3;
}; }; } format_1;									// format_N(1)
// use macro !
typedef_mcn_struct { uint16_t Rd:3, Rs:3, Rn:3, Op:1, l:1, _head:5; };		format_N(2);
typedef_mcn_struct { uint16_t Offset8:8, Rd:3, Op:2, _head:3; };			format_N(3);
typedef_mcn_struct { uint16_t Rd:3, Rs:3, Op:4, _head:6; };					format_N(4);
typedef_mcn_struct { uint16_t Rd:3, Rs:3, H2:1, H1:1, Op:2, _head:6; };		format_N(5);
typedef_mcn_struct { uint16_t Word8:8, Rd:3, _head:5; };					format_N(6);
typedef_mcn_struct { uint16_t Rd:3, Rb:3, Ro:3, _0:1, B:1, L:1, _head:4; };	format_N(7);
typedef_mcn_struct { uint16_t Rd:3, Rb:3, Ro:3, _1:1, S:1, H:1, _head:4; };	format_N(8);
typedef_mcn_struct { uint16_t Rd:3, Rb:3, Offset5:5, L:1, B:1, _head:3; };	format_N(9);
typedef_mcn_struct { uint16_t Rd:3, Rb:3, Offset5:5, L:1, _head:4; };		format_N(10);
typedef_mcn_struct { uint16_t Word8:8, Rd:3, L:1, _head:4; };				format_N(11);
typedef_mcn_struct { uint16_t Word8:8, Rd:3, SP:1, _head:4;  };				format_N(12);
typedef_mcn_struct { uint16_t SWord7:7, S:1, _head:8; };					format_N(13);
typedef_mcn_struct { uint16_t Rlist:8, R:1, _10:2, L:1, _head:4; };			format_N(14);
typedef_mcn_struct { uint16_t Rlist:8, Rb:3, L:1, _head:4; };				format_N(15);
typedef_mcn_struct { uint16_t Soffset8:8, Cond:4, _head:4; };				format_N(16);
typedef_mcn_struct { uint16_t Value8:8, _head:8; };							format_N(17);
typedef_mcn_struct { uint16_t Offset11:11, _head:5; };						format_N(18);
typedef_mcn_struct { uint16_t Offset:11, H:1, _head:4; };					format_N(19);


/*
typedef_mcn_struct { uint16_t  }; format_N();
*/

const uint32_t HEAD_DATA[20] = {
	0b0,		0b000,		0b00011,	0b001,
	0b010000,	0b010001,	0b01001,	0b0101,
	0b0101,		0b011,		0b1000,		0b1001,
	0b1010,		0b10110000,	0b1011,		0b1100,
	0b1101,		0b11011111,	0b11100,	0b1111,
};

const uint32_t OP_DATA[20] = {
	0, 4, 5, 3,
	3, 5, 2, 5,
	5, 5, 4, 3,
	3, 2, 3, 3,
	2, 1, 1, 2,
};

void asm2mcn(uint16_t* dst, int32_t* src){
	switch(src[0])
	{
		#define case_begin(n) \
		case n: {\
			format_##n* _dst = (format_##n*)dst;\
			_dst->_head = HEAD_DATA[src[0]];

		#define arg_1(arg1) \
			_dst->arg1 = src[1];

		#define arg_2(arg1,arg2) \
			arg_1(arg1);\
			_dst->arg2 = src[2];

		#define arg_3(arg1,arg2,arg3) \
			arg_2(arg1, arg2);\
			_dst->arg3 = src[3];

		#define arg_4(arg1,arg2,arg3,arg4) \
			arg_3(arg1, arg2, arg3);\
			_dst->arg4 = src[4];

		#define arg_5(arg1,arg2,arg3,arg4,arg5) \
			arg_4(arg1, arg2, arg3, arg4);\
			_dst->arg5 = src[5];

		#define arg_6(arg1,arg2,arg3,arg4,arg5, arg6) \
			arg_5(arg1, arg2, arg3, arg4);\
			_dst->arg6 = src[6];

		#define case_end \
			break;\
		}

		// no use macro...
		case 1: {								// case_begin(1)
			format_1* _dst = (format_1*)dst;	//
			_dst->_head = HEAD_DATA[src[0]];	// _____________
			_dst->Op = src[1];					// arg_4(Op, Offset5, Rs, Rd)
			_dst->Offset5 = src[2];				//
			_dst->Rs = src[3];					//
			_dst->Rd = src[4];					// _____________
			break;								// case_end
		}										// _____________
		// use macro !
		case_begin(2);	arg_5(l, Op, Rn, Rs, Rd);		case_end;
		case_begin(3);	arg_3(Op, Rd, Offset8);			case_end;
		case_begin(4);	arg_3(Op, Rs, Rd);				case_end;
		case_begin(5);	arg_5(Op, H1, H2, Rs, Rd);		case_end;
		case_begin(6);	arg_2(Rd, Word8);				case_end;
		case_begin(7);	_dst->_0=0; arg_5(L, B, Ro, Rb, Rd);	case_end;
		case_begin(8);	_dst->_1=1; arg_5(H, S, Ro, Rb, Rd);	case_end;
		case_begin(9);	arg_5(B, L, Offset5, Rb, Rd);	case_end;
		case_begin(10);	arg_4(L, Offset5, Rb, Rd);		case_end;
		case_begin(11);	arg_3(L, Rd, Word8);			case_end;
		case_begin(12);	arg_3(SP, Rd, Word8);			case_end;
		case_begin(13);	arg_2(S, SWord7);				case_end;
		case_begin(14);	_dst->_10=2; arg_3(L, R, Rlist);		case_end;
		case_begin(15);	arg_3(L, Rb, Rlist);			case_end;
		case_begin(16);	arg_2(Cond, Soffset8);			case_end;
		case_begin(17);	arg_1(Value8);					case_end;
		case_begin(18);	arg_1(Offset11);				case_end;
		case_begin(19);	arg_2(H, Offset);				case_end;
		default:	break;
	}
}

int main(){
	uint16_t	_bin;
	int32_t		src[8];
	
	while(1)
	{
		printf("___input___\n");
		// set format
		printf("op : "); scanf("%d", &src[0]);
		if (src[0] < 0 || 20 <= src[0])
			continue;

		// set args
		for (int i = 1; i < OP_DATA[src[0]]+1; i++) {
			printf("%d? : ", i); scanf("%d", &src[i]);
		}

		_bin = 0x0000;
		asm2mcn(&_bin, src);
		printf("mcn = %04X\n", _bin);
	}
}
