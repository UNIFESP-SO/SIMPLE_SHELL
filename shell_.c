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


int conta_pipe(char *str)
{
    int cont = 0;
    int a = 0;
    while(str[a] != '\n'){
        if(str[a] == '|')cont++;
        a++;
    }
    return cont;
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
	int cont_pipe, i, status;
	char *command;
	char **parameters;
	char *str;
	int fd[2];
	int past;
//	char *coma[] = {"grep", "sh", NULL};
//	char *coma1[] = {"ls", "-la", NULL};
	
/*	if (pipe(fd) < 0) {
		perror("pipe()");
		return -1;
	}
*/	
	while(1)
	{
		str	= (char *)calloc(MAX_COM, sizeof (char));
		command = (char *)calloc(MAX_PAR, sizeof (char));
		parameters = aloca(MAX_PAR, MAX_PAR);

		printf("FERNANDO@SHELL$ ");
		read_commandline(str);	// lendo linha de comando inteira
		if(!strcasecmp("exit\n", str)) break; 
		if(!strcasecmp("\n", str)) continue;
		
		cont_pipe = conta_pipe(str);
		read_command(str, command, parameters);

		for(i = 0; i< cont_pipe; i++){
			str = index(str, '|');
			str++;
			pipe(fd);//cria o tunel que é herdado pelo filho
			if(!fork()){//para o novo processo
				if(i!=0){//se não for o primeiro comando
					dup2(past, 0);//recebe como entrada a saida do tunel passado
				}
				dup2(fd[1], 1);//a saida vai para o novo tunel
    				execvpe(command,parameters);//especializa o novo processo
			}
			close(fd[1]);//fecha o tunel para o pai
			if(i!=0) close(past);//fecha o tunel passado para o pai
			past = fd[0];//o tunel atual agora é o passado
		}
		if(!fork()){//executado para o ultimo processo
			if(i!=0){//se ele tb não for o primeiro
				dup2(past, 0);//coloca a entrada para a saida do tunel
			}
			execvpe(command,parameters);//especializa este processo
		}else{
			wait(&status);
		}

		free(command);
		free(str);
		free(parameters);
	}
	return 0;
}
