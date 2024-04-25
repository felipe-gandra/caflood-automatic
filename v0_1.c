#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

int mkdirOut(float add, float begin, float end, char paths[30][40])
{	
	int count = 0;
	if (end<begin)
	{	
		printf("ERROR. The last parameter must be bigger than the first one\n");
		return 0;
	}

	while(begin<=end)
	{	
		char out_code[10];
		char path[40] = {"./output/case_"};

		sprintf(out_code, "%f", begin);
		strcat(path, out_code);

		mkdir(path, 0700);

		strcpy(paths[count], path);
		begin+=add;
		count++;

	}
	return count;


}











int main(/*int argc, char *argv[]*/)
{
	char paths[30][40];

	int npaths = mkdirOut(0.01, 0.04, 0.10, paths);

	for (int i = 0; i<npaths; i++)
	{
		printf("%s\n", paths[i]);
	}



	return 0;
}