#include "gba.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Input.h"
#include "Scanner.h"
#include "gbascanf.h"

void test(){

}

int main()
{
	Input i;
	Scanner s;
	char buf[256];

	consoleDemoInit();
	printf("start\n");

	while(1)
	{
	// Input
		i.update();

	// preModel
		//s.update();

	// Model
		s.scanf(buf);

	// vModel


	// View
		printf("%s", buf);
	}
}
