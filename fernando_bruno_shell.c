// by:
// 	Fernando Soares	RA: 86281
//	Bruno Ferreira	RA: 104790
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#define TRUE 1
#define MAX_COM 1024
#define MAX_PAR 1000

int read_commandline(char *str){
        fgets(str, MAX_COM, stdin);
	return 0;
}

char *getfilename(char *str){
	char *filename;
	int k = 0, l = 0;
	filename = (char *)calloc(MAX_PAR, sizeof (char));

	str = index(str, '>');
	str++;
	while(str[k] != '\n' || str[k] != '\0'){
		while( str[k] == ' ' )
			k++;
		filename[l] = str[k];
		l++; k++;
		if(str[k] == ' ') break;
	}
	filename[l]='\n';
	return filename;
}

int read_command(char *str, char *command, char **parameters){
	int ip = 0, i=0 , jp= 0, sair = 0;
	while(str[i] != '\n' && str[i] != '|' && str[i] != '>'){
		while( str[i] == ' ' ){
			 i++;
			if(str[i] == '\n' || str[i] == '>') sair = 1;
		}
		if ( sair ) break;
		if(str[i] != '|'){
			while( str[i] != ' ' && str[i] != '\n' && str[i] != '|' && str[i] != '>'){
				parameters[ip][jp] = str[i];
				i++; jp++;
			}
			parameters[ip][jp] = '\0';
			jp = 0; ip++;
		}
	}
	parameters[ip] = (char *)NULL;
	strcpy(command , parameters[0]);
	printf("command =%s\nparameters[0] = %s\nparameters[1] = %s\n", command, parameters[0], parameters[1]);
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
	int cpipes,status, i=0, flagfile = 0, file;
	int fd[2];
	int past;
	char *filename;
	filename = (char *)calloc(MAX_PAR, sizeof (char));

	if(index(str, '>')){
		filename = getfilename(str);
		printf("filename = %s", filename);
		file =	open(filename, O_CREAT | O_WRONLY | O_APPEND);
		flagfile = 1;
	
	}
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
		if(flagfile){
			dup2(file, STDOUT_FILENO);
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
	
//	int filename
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
