#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

int mkdirOut(float add, float begin, float end)
{	
	if (end<begin)
	{	
		printf("The last parameter must be bigger than the first one\n");
		return 0;
	}

	while(begin<=end)
	{	
		char out_code[10];
		char path[40] = {"./output/case_"};
		sprintf(out_code, "%f", begin);
		strcat(path, out_code);
		mkdir(path, 0700);
		begin+=add;

	}



}











int main(/*int argc, char *argv[]*/)
{
	mkdirOut(0.01, 0.04, 0.10);



	return 0;
}