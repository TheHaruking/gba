#ifndef _GBA_DE_ASM__GBAPRINT_
#define _GBA_DE_ASM__GBAPRINT_

extern void move(int, int);
extern void videoInit(unsigned short*);
extern void videoFinish();
extern void testprintval(unsigned short);
// 桁対応
extern void testprintval2(unsigned int);
extern void gbaprint(char*);
extern void refresh();

#endif