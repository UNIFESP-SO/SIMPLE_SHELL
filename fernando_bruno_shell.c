// by:
// 	Fernando Soares	RA: 86281
//	Bruno Ferreira	RA: 104790

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
			if(str[i] == '\n' || str[i] == '|') sair = 1;
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

int conta_pipe(char *str){
    int cont = 0;
    int j = 0;
    while(str[j] != '\n'){
        if(str[j] == '|') 
		cont++;
        j++;
    }
    return cont;
}

int exec_comando(char *str, char *command, char **parameters){
	int cpipes,status, i=0;
	int fd[2];
	int past;
	
	cpipes = conta_pipe(str);
	for(i = 0; i < cpipes; i++){
		if (pipe(fd) < 0) {
			perror("pipe()");
			return -1;
		}
	        read_command(str, command, parameters); 
		if(!fork()) {
			if(i!=0){
				dup2(past, STDIN_FILENO);
			}
			dup2(fd[1], STDOUT_FILENO);
			execvp(command, parameters);	
		}
		else wait(&status);
		close(fd[1]);
		if(i!=0)
			close(past);
		past = fd[0];
		str = index(str, '|');
		str++;
	}
	read_command(str, command, parameters); 
	if(!fork()){
		if(i!=0){
			dup2(past, STDIN_FILENO);
		}
		execvp(command, parameters);
	}
	else wait(&status);
	return 1;
}

int main(void){
	int status;
	char *command;
	char **parameters;
	char *str;
	
	while(1)
	{
		command = (char *)calloc(MAX_PAR, sizeof (char));
		str	= (char *)calloc(MAX_COM, sizeof (char));
		parameters = aloca(MAX_PAR, MAX_PAR);

		printf("FERNANDO&BRUNO@SHELL$ ");

		read_commandline(str);	// lendo linha de comando inteira
		if(!strcasecmp("exit\n", str)) break; 
		if(!strcasecmp("\n", str)) continue;
		
		if(fork() == 0) {
			exec_comando(str, command, parameters);
			return 0;
		}
		else{
			wait(&status);
		}
	
		free(command);
		free(str);
		free(parameters);
	}
	return 0;
}
