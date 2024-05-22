#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

int changeRG(char file[], float parameter_list[], int i)            // funcao para alterar o roughness global na input.csv
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


int changeEA(char file[], char *parameter_list[], int i)    //funcao para alterar o elevation ascii no input.csv
{
    FILE *arquivo = fopen(file, "r+");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return 1;
    }
    char old_line[100];
    char line[256];
    char parameter[30];
    long pos;

    strcpy(parameter, parameter_list[i]);

    while (fgets(line, sizeof(line), arquivo) != NULL)
    {
        pos = ftell(arquivo);
        strcpy(old_line, line);
        
        line[strcspn(line, "\n")] = 0;

        char *type = strtok(line, ",");
        char *value = strtok(NULL, ",");

        if (type != NULL && strcmp(type, "Elevation ASCII \t\t") == 0 || strcmp(type, "Elevation ASCII			") == 0)
        {	
        	int quantidade = strlen(old_line);
        	int n;

            fseek(arquivo, pos-strlen(old_line), SEEK_SET);

            
            n = fprintf(arquivo, "Elevation ASCII \t\t,%s", parameter);

            if (n<quantidade-1)
            {
            	for (int k = 0; i<quantidade-n-1; i++)         //completa com espaços caso fique um resto do parametro anterior ao sobrescrever
            	{
            		fprintf(arquivo, " ");
            	}
            }

            fclose(arquivo);

            return 0;
        }
    }

    fclose(arquivo);

    printf("Parâmetro 'Elevation ASCII' não encontrado\n");
    return 1;
}



int createOutput(int *countRG, int *countEA, float RGvector[], char *EAvector[], char *paths[], char outputFile[])
{	
	int npaths = (*countRG)*(*countEA);
	

	for (int i = 0; i< npaths; i++)
	{
		paths[i] = malloc(40);
	}
	for (int i = 0; i<(*countRG); i++)
	{	
		char RG[10];
		for (int j = 0; j<(*countEA); j++)
		{ 
			char RG_fatia[6];
            char EA_fatia[10];
            sprintf(RG_fatia, "%.3f", RGvector[i]);
            sscanf(EAvector[j], "%*[^0-9]%5[^.]", EA_fatia);

            snprintf(paths[i * (*countEA) + j], 40, "%s_%s", RG_fatia, EA_fatia);   //formata no jeito correto
		}
	}

	for (int i = 0; i<npaths;i++)
	{	
		char name[20] = {"./"};
		strcat(name, outputFile);
		strcat(name, paths[i]);
		strcpy(paths[i], name);
		if(mkdir(name, 0700))
		{	
			printf("Erro na criacao das pastas de output\n");
			return 1;
		}
	}

	return 0;

}

int readData(int *countRG, int *countEA, char file[], float RGvector[], char *EAvector[])  //funcao para ler os valores dos parametros no arquivo dados.csv
{	
	
	for (int i = 0; i<10; i++)
	{
		EAvector[i] = malloc(30);
	}

	FILE *arquivo = fopen(file, "r+");
	if (arquivo == NULL)
	{
		printf("Erro ao abrir o arquivo com dados dos parametros\n");
		return 1;
	}

	char line[250];
	while (fgets(line, 250, arquivo)!=NULL)
	{
		char *name;
		char *value;
		name = strtok(line, ",");
		char fatia[8];
		strncpy(fatia, name, 7);

		if (strcmp(fatia,"Roughne") == 0)    //comparo com uma fatia caso o nome do parametro nao seja exatamente igual (pode ter um caracter nulo no final e dar erro caso eu compare com tudo)
		{
			while (1)
			{	
				value = strtok(NULL, ",");
				if (value == NULL)
				{
					break;
				}
				RGvector[*countRG] = atof(value);
				*countRG += 1;
			}
		}

		if (strcmp(fatia,"Elevati") == 0)
		{
			while (1)
			{	
				value = strtok(NULL, ",");
				if (value == NULL)
				{
					break;
				}
				strcpy(EAvector[*countEA], value);
				*countEA += 1;
			}
		}

	}
	fclose(arquivo);
	return 0;

}


//   ./v3 <pasta_input> <arquivo_input> <arquivo_parametros> <pasta_output>

int main(int argc, char *argv[])
{	
	if (argc<4)
	{
		printf("Comando de execucao errado\n");
		return 0;
	}
	int countRG = 0, countEA = 0;
	float values_rg[20];
	char *values_ea[30];

	char folder_in[50];
	char file_in[20];
	char parameter_file[30];
	char folder_out[50];
	char input_dir[70];

	strcpy(folder_in, argv[1]);
	strcpy(file_in, argv[2]);
	strcpy(parameter_file, argv[3]);
	strcpy(folder_out, argv[4]);
	strcpy(input_dir, folder_in);
	strcat(input_dir, file_in);

	if (readData(&countRG, &countEA, parameter_file, values_rg, values_ea))   //lendo os parametros e seus intervalos de variacao
	{
		return 0;
	}


	int npaths = (countRG*countEA);
	char *paths[npaths];


	if (createOutput(&countRG, &countEA, values_rg, values_ea, paths, folder_out))   //criando uma pasta para cada combinacao de parametros
	{
		return 0;
	}

	//loop principal

	for (int i = 0; i<countRG; i++)
	{	
		
		if (changeRG(input_dir, values_rg, i))
		{
			return 0;  //erro
		}
		
		for (int j = 0; j<countEA; j++)
		{
			char *args[] = {"./caflood", "-WCA2D", folder_in, file_in, strcat(paths[countEA*i+j],"/"), NULL};

			if (changeEA(input_dir, values_ea, j))
			{
				return 0;  //erro
			}




			

			pid_t pid = fork();

			if (pid == -1)
			{
				printf("Erro ao criar processo filho\n");
			}
			else if (pid == 0)
			{
				//processo filho

				execv("./caflood", args);

				printf("Erro ao executar o caflood\n");
				return 0;
			}
			else
			{
				//processo pai
				int status;

				waitpid(pid, &status, 0);

			}
		}
	}

	return 0;
}