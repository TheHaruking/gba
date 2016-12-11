#ifndef _INC_SCANNER
#define _INC_SCANNER

class Scanner {
 private:
	 // メンバ変数
	 char linebuf[256];
	 int linebuf_p;
	 int linebuf_p_old;
	 int mode;
	 int direction;
	 int chr_pointer;
	 int y;
	 unsigned short k[4];

	 // プライベート関数
	 void Logic();
	 void LogicModeHOME();
	 void LogicModeHOLD();
	 void LogicModeFINI();

 public:
	 Scanner();
	 void update();
	 bool isEnter();
	 bool isPut();
	 bool isBack();
	 int  scanf(char*);
	 char *getLine();
	 char getc();
 };

#endif
