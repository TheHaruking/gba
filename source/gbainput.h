#ifndef _GBA_DE_ASM__GBAINPUT_
#define _GBA_DE_ASM__GBAINPUT_

extern int   gbainputInit(void);
extern void  gbainputFinish(void);
extern char* gbainputMain(char*);
extern int	 gbainputIsEnter(void);
extern int	 gbainputIsChanged(void);


#endif