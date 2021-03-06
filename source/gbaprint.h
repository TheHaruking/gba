#ifndef _GBA_DE_ASM__GBAPRINT_
#define _GBA_DE_ASM__GBAPRINT_

extern void move(int, int);
extern void videoInit(unsigned short*);
extern void videoFinish();
extern void testprintval(unsigned short);
extern void gbaprintval(unsigned int);
extern void gbaprintraw(unsigned short);
extern void gbaprint(const char*);
extern void refresh();

#endif