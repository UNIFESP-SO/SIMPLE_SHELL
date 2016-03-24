#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#define TRUE 1
#define MAX_COM 1024
#define MAX_PAR 100

int read_commandline(char *str){
        fgets(str, MAX_COM, stdin);
	return 0;
}

int read_command(char *str, char *command, char **parameters){
	int ip = 0, i=0 , jp= 0;

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

int main(void){
	int status, flag = -1;
	int STDOUT_copy = dup(STDOUT_FILENO);
	int STDIN_copy = dup(STDIN_FILENO);
	char *command;
	char **parameters;
	char *str;
	int fd[2];
	if (pipe(fd) < 0) {
		perror("pipe()");
		return -1;
	}
	
	while(1)
	{
		command = (char *)calloc(MAX_PAR, sizeof (char));
		str	= (char *)calloc(MAX_COM, sizeof (char));
		parameters = aloca(MAX_PAR, MAX_PAR);

		printf("FERNANDO@SHELL$ ");

		read_commandline(str);		// lendo linha de comando inteira
		if(!strcasecmp("exit\n", str)) break; 	// se teclar exit, sai do shell
		if(!strcasecmp("\n", str)) continue;	// se teclar enter, repete o laço

		while(str){	//while tem comando a executar
                	read_command(str, command, parameters); 
			flag = 0;
		
			if( (str = index(str, '|')) ){ // se houver pipe, redireciona stdout
				flag = 1;
				str++;
				close(1);
				dup2(fd[1], 1);
				close(0);
				dup2(fd[0], 0);
			
			}
			if(flag == 0 ){
				close(fd[1]);
				dup2(STDOUT_copy, 1);
			} 
			if(fork() != 0){
				waitpid(-1, &status, 0);
		
			}else {
				execvp(command, parameters);
				return 0;
			}
			
			if(flag == 0){
				close(fd[0]);
				dup2(STDIN_copy, 0);
			}
		}
		free(command);
		free(str);
		free(parameters);
	}
	return 0;
}
