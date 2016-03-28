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
	char *command;
	char **parameters;
	//int copyout = dup(STDOUT_FILENO);
	//int copyin = dup(STDIN_FILENO);
	char *str;
	int fd[2];
	char *coma[] = {"grep", "sh", NULL};

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

		read_commandline(str);	// lendo linha de comando inteira
		if(!strcasecmp("exit\n", str)) break; 
		if(!strcasecmp("\n", str)) continue;
		
		int flag = 0, flag2 = -1;

		while(str){	//while tem comando a executar
                	read_command(str, command, parameters);
			flag = 0; 
			if( (str = index(str, '|')) ){
				str++;
				flag = 1;
				flag2 = 0;
			}
			else{
				if(flag2 == 0) flag2 = 1;
	//			continue;
			}
			if(fork() == 0) {
				if(flag == 1){
					dup2(fd[1], 1);
					dup2(fd[0], 0);
				}
				if(flag2 == 1){
			//		dup2(copyout, 1);
					dup2(fd[0], 0);
					execvp(coma[0], coma);
				}
				printf("executando\n");
				execvp(command, parameters);
				printf("depois de exec\n");
			//	return 0;
			}else {
				printf("esperadno\n");
				waitpid(0,&status,0);
				printf("saindo do pai\n");
			}
		}
		free(command);
		free(str);
		free(parameters);
	}
	return 0;
}
