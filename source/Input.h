#ifndef _INC_INPUT
#define _INC_INPUT

class Input{
private:
	unsigned short k[4];
public:
	void update();
	unsigned short *GetKey(void);
	unsigned short GetKey0(void);
	unsigned short GetKey1(void);
	unsigned short GetKey2(void);
	bool isKey0(unsigned short);
	bool isKey1(unsigned short);
	bool isKey2(unsigned short);
};

#endif
