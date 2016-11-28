struct asmarray {
	unsigned char head;
	unsigned char _unused;
	unsigned short code[7];
};

extern void McnToAsm(struct asmarray*, unsigned short);
