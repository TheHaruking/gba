
struct asmarray {
	unsigned char head;		
	unsigned char p;
	signed   int  type;
	unsigned short code[7];
};

typedef uint16_t (*fgetop)(struct asmarray*, unsigned short*);

extern void McnToAsm(struct asmarray*, unsigned short);

