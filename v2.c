#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

int changeParameter(char file[], float parameter_list[], int i)
{	
	FILE *arquivo = fopen(file, "r+");
		if (arquivo == NULL)
		{
			printf("Erro ao abrir o arquivo");    
			return 1;
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

            parameter = parameter_list[i];						//altera o parametro no input_file
            fseek(arquivo, pos-strlen(old_line)-1, SEEK_SET);
 
            fprintf(arquivo, "\nRoughness Global\t\t, %.3f", parameter);
            fseek(arquivo, 0, SEEK_SET);
            fclose(arquivo);
            break;
        }
	}
	return 0;
	}	


int mkdirOut(int n, float list[20], char output_folder[], char paths[20][40])
{
	int check;
	for (int i = 0; i<n; i++)
	{
		char name[40] = {"./"};
		strcat(name, output_folder);
		strcat(name, "case_");

		char value[15];
		
		sprintf(value, "%.3f", list[i]);

		strcat(name, value);
		check = mkdir(name, 0700);
		strcpy(paths[i], name);
		
	}
	if (check)
	{
		printf("Erro ao criar pasta de output\n");
		return 1;
	}
	return 0;
}



/* 
linha de execução:       ./programa <input_folder> <input_file> <parameter_file> <output_folder> 
*/


int main(int argc, char *argv[])
{	
        if (argc != 5)
        {
        printf("Execucao errada\n");
        printf("Correto: ' ./auto_v2 <diretorio_input> <arquivo_input.csv> <intervalo_parametro.csv> <diretorio_output> '\n");
        return 0;
        }

	FILE *arquivo;

	char output_folder[30];
	char input_folder[30];
	char input_file[30];
	char parameter_file_name[30];
	char paths[30][40];
	char *parameter_value;
	int count = 0;
	float parameter_list[20];



	strcpy(output_folder, argv[4]);
	strcpy(parameter_file_name, argv[3]);
	strcpy(input_file, argv[2]);
	strcpy(input_folder, argv[1]);

	
	FILE *parameter_file = fopen(parameter_file_name, "r+");   //arquivo csv com o intervalo do parametro

	char linha[100];
	fgets(linha, 100, parameter_file);
	parameter_value = strtok(linha, ",");    //aqui poderia haver uma verificação de qual parametro está sendo analisado. Esse codigo apenas ignora a primeira palavra do arquivo: "Roughness Global"


	while (1)       //leitura dos parametros no arquivo csv
	{
		parameter_value = strtok(NULL, ",");
		if (parameter_value == NULL)
		{
			break;
		}
		parameter_list[count] = atof(parameter_value);

		count++;   //variavel que conta o tamanho do intervalo de parametros
	}

	

	if (mkdirOut(count, parameter_list, output_folder, paths))		//criando as pastas de output
	{
		return 0;
	}																	


	char arq_file[60];
	strcpy(arq_file, input_folder);
	strcat(arq_file, input_file);

	

    // loop principal
	
	for (int i = 0; i<count; i++)
	{
		char *args[] = {"./caflood", "-WCA2D", input_folder, input_file, paths[i]};

		arquivo = fopen(arq_file, "r+");

		if (arquivo == NULL)
		{
			printf("Erro ao abrir o arquivoo");
			return 0;
		}

		if (changeParameter(arq_file, parameter_list, i))   //altera o parametro no input.csv
		{
			return 0;     //termina o programa em caso de erro
		}

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

	printf("Comandos executados:\n");
	for (int i = 0; i<count; i++)
	{
		printf("./caflood -WCA2D %s %s %s\n", input_folder, input_file, paths[i]);
	}



	return 0;
}
