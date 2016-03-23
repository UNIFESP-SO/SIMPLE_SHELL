#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define TRUE 1
#define MAX_COM 1024
#define MAX_PAR 100

int read_commandline(char *str){
        fgets(str, MAX_COM, stdin);
	return 0;
}

int read_command(char *str, char *command, char **parameters){
	int ip = 0, i=0 , jp= 0, flag = 0;

	while(str[i] != '\n' && str[i] != '|'){
		while( str[i] == ' ' ) i++;
		if(str[i] != '|'){
			while( str[i] != ' ' && str[i] != '\n' && str[i] != '|' ){
				parameters[ip][jp] = str[i];
				i++; jp++;
			}
			parameters[ip][jp] = '\0';
			jp = 0; ip++;
		}
	}
	parameters[ip] = NULL;
	strcpy(command , parameters[0]);

	return ip;
}

char **aloca(int L, int C){
	char **parameters = NULL;
	int i;
        parameters = (char **)calloc(L, sizeof (char *));
        for(i = 0; i < L; i++){
                parameters[i] = (char *)calloc(C, sizeof(char));
        }
	return parameters;
}

int main(){
	int status;
	char *command;
	char **parameters;
	char *str;

	while(1)
	{
		command = (char *)calloc(MAX_PAR, sizeof (char));
		str	= (char *)calloc(MAX_COM, sizeof (char));
		parameters = aloca(MAX_PAR, MAX_PAR);

		printf("FERNANDO@SHELL$ ");

		read_commandline(str);		// lendo linha de comando inteira
		if(!strcasecmp("exit\n", str)) break; 	// se teclar exit, sai do shell
		if(!strcasecmp("\n", str)) continue;	// se teclar enter, repete o laço

                read_command(str, command, parameters); 

		// redirecionando saida padrao
		if( (str = index(str, '|')) ){
			
		}
		if(fork() != 0){
			waitpid(-1, &status, 0);
		}else {
			execvp(command, parameters);
			return 0;
		}
	}
	return 0;
}
