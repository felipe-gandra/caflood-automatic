#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

void changeParameter(char file[], float p_add)
{	
	FILE *arquivo = fopen(file, "r+");
		if (arquivo == NULL)
		{
			printf("Erro ao abrir o arquivo");

		}

	char old_line[100];
    char line[100];
    char *type;
    char *value;
    float parameter;
    long long int pos;




	while (fgets(line, sizeof(line), arquivo) != NULL) {
			pos = ftell(arquivo);
        	strcpy(old_line, line);
        
        	type = strtok(line, ",");
        	value = strtok(NULL, ",");



        

        if (strcmp(type, "Roughness Global\t\t") == 0) {
             
        	parameter = atof(value);

            parameter += p_add;
            fseek(arquivo, pos-strlen(old_line)-1, SEEK_SET);
 
            fprintf(arquivo, "\nRoughness Global\t\t, %.2f", parameter);
            fseek(arquivo, 0, SEEK_SET);
            fclose(arquivo);
            break;
        }
	}
	}	











int mkdirOut(float add, float begin, float end, char paths[30][40], char output_file[30])
{	
	int count = 0;
	if (end<begin)
	{	
		printf("ERROR. The last parameter must be bigger than the first one\n");
		return 0;
	}

	while(begin<=(end+0.001))
	{	
		char out_code[10];
		char path[] = {"/case_"};
		char path_[40] = {"./"};

		sprintf(out_code, "%f", begin);

		strcat(path_, output_file);
		strcat(path_, path);
		strcat(path_, out_code);

		mkdir(path_, 0700);

		strcpy(paths[count], path_);
		begin+=add;
		count++;

	}
	return count;
}


/* ./programa input_folder  input file output_folder */


int main(int argc, char *argv[])
{	
	FILE *arquivo;

	char output_folder[30];
	char input_folder[30];
	char input_file[30];

	strcpy(output_folder, argv[3]);
	strcpy(input_file, argv[2]);
	strcpy(input_folder, argv[1]);

	float p_begin;
	printf("Inicio:\n");
	scanf("%f", &p_begin);

	float p_end;
	printf("Fim:\n");
	scanf("%f", &p_end);

	float p_add;
	printf("Passo:\n");
	scanf("%f", &p_add);



	char paths[30][40];

	int npaths = mkdirOut(p_add, p_begin, p_end, paths, output_folder);


	
	char arq_file[60];
	strcpy(arq_file, input_folder);
	strcat(arq_file, input_file);

	//printf("inputfolder %s\n", input_folder);
	//printf("arqfile %s\n", arq_file);
	//printf("inputfile %s\n", input_file);






	arquivo = fopen(arq_file, "r+");
	if (arquivo == NULL)
	{
		printf("Erro ao abrir o arquivo");
		return 0;
	}

	char old_line[100];
    char line[100];
    char *type;
    char *value;
    float parameter;

    long long int pos = ftell(arquivo);
	while (fgets(line, sizeof(line), arquivo) != NULL) {
	long long int pos = ftell(arquivo);
        strcpy(old_line, line);
        
        type = strtok(line, ",");
        value = strtok(NULL, ",");



        

        if (strcmp(type, "Roughness Global\t\t") == 0) {
             
        	parameter = atof(value);

            parameter = p_begin-p_add;
            fseek(arquivo, pos-strlen(old_line)-1, SEEK_SET);
 
            fprintf(arquivo, "\nRoughness Global\t\t, %.2f", parameter);
            fseek(arquivo, 0, SEEK_SET);
            fclose(arquivo);
            break;
        }
        
    }



	//verificação dos paths

	for (int i = 0; i<npaths; i++)
	{
		printf("./caflood -WCA2D %s %s %s\n", input_folder, input_file, paths[i]);
	}


























    //main loop
	
	for (int i = 0; i<npaths; i++)
	{
		char *args[] = {"./caflood", "-WCA2D", input_folder, input_file, paths[i]};

		//printf("%s\n", paths[i]);
		arquivo = fopen(arq_file, "r+");
		if (arquivo == NULL)
		{
			printf("Erro ao abrir o arquivo");
			return 0;
		}

		changeParameter(arq_file, p_add);

		pid_t pid = fork();
		
			if (pid == -1)
			{
				//Deu erro
				printf("Erro ao criar processo filho\n");
				return 0;
			}
			else if (pid == 0)
			{
				//codigo processo filho
				execv("./caflood", args);


				printf("Erro ao executar caflood\n");
				return 0;

			}
			else
			{
				//processo pai
				int status;

				waitpid(pid, &status, 0);


			}
	}







	



	return 0;
}





