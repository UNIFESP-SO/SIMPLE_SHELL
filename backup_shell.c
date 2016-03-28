#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#define TRUE 1
#define MAX_COM 1024
#define MAX_PAR 1000

int read_commandline(char *str){
        fgets(str, MAX_COM, stdin);
	return 0;
}

int read_command(char *str, char *command, char **parameters){
	int ip = 0, i=0 , jp= 0, sair = 0;
	while(str[i] != '\n' && str[i] != '|'){
		while( str[i] == ' ' ){
			 i++;
			if(str[i] == '\n') sair = 1;
		}
		if ( sair ) break;
		if(str[i] != '|'){
			while( str[i] != ' ' && str[i] != '\n' && str[i] != '|' ){
				parameters[ip][jp] = str[i];
				i++; jp++;
			}
			parameters[ip][jp] = '\0';
			jp = 0; ip++;
		}
	}
	parameters[ip] = (char *)NULL;
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

int main(void){
	int status;
	char *command1;
	char *command2;
	char **parameters1;
	char **parameters2;
	char *str;
	int fd[2];

	if (pipe(fd) < 0) {
		perror("pipe()");
		return -1;
	}
	
	while(1)
	{
		command1 = (char *)calloc(MAX_PAR, sizeof (char));
		command2 = (char *)calloc(MAX_PAR, sizeof (char));
		str	= (char *)calloc(MAX_COM, sizeof (char));
		parameters1 = aloca(MAX_PAR, MAX_PAR);
		parameters2 = aloca(MAX_PAR, MAX_PAR);

		printf("FERNANDO@SHELL$ ");

		read_commandline(str);	// lendo linha de comando inteira
		if(!strcasecmp("exit\n", str)) break; 
		if(!strcasecmp("\n", str)) continue;

		while(str){	//while tem comando a executar
                	read_command(str, command1, parameters1); 
			if( (str = index(str, '|')) ){
				str++;
			}
			else
				continue;
			if(fork() == 0) {
				if(fork() == 0){
					waitpid(0, &status, 0);
					read_command(str, command2, parameters2);
					dup2(fd[0], 0);
					execvp(command2, parameters2);
					return 0;
				}else {
					dup2(fd[1], 1);
					execvp(command1, parameters1);
					return 0;
				}
			}
			else{
				waitpid(0,&status, 0);
			}
		}
		free(command1);
		free(command2);
		free(str);
		free(parameters1);
		free(parameters2);
	}
	return 0;
}
