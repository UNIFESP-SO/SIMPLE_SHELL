#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

#define TRUE 1
#define MAX_COM 1024
#define MAX_PAR 100
#define STD_INPUT 0
#define STD_OUTPUT 1

#define STD_INPUT1 0
#define STD_OUTPUT1 1

int cont = 0;
int *status;
int i = 0;
char *str;
char *command;
char **parameters;

int read_commandline(char *str){
    fgets(str, MAX_COM, stdin);
	return 0;
}

int conta_pipe(char *str){
    int cont = 0;
    int a = 0;
    while(str[a] != '\n'){
        if(str[a] == '|')cont++;
        a++;
    }
    return cont;
}

int read_command(char *str, char *command, char **parameters){
	int ip = 0, jp = 0;
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

void cria_pipe(){
    int fd[2];
    pipe(&fd[0]);

    int flag = 0;
    int fd1[2];
    pipe(&fd1[0]);

    printf("Contador: %i\n", cont);

    if (fork() != 0){
        waitpid(-1,status,0);
        if(cont>0){
            close(fd1[1]);
            dup(fd1[0]);
            read(fd1[0], &i, sizeof(i));
            close(fd1[0]);
            printf("Valor de i pai: %i\n",i);
            printf("%c\n",str[i]);
            if(str[i] != '\n' && (str[i] == '|' || str[i] == ' '))
                while(str[i] != '\n' && (str[i] == '|' || str[i] == ' '))
                    i++;
            read_command(str, command, parameters);
            printf("%s\t%s*", command, parameters);
        }
        //processo pai executa esses comandos
        waitpid(-1,status,0);
        execvp(command, parameters);
        close(fd[0]); //o processo 1 não precisa ler o pipe
        dup(fd[1]); //configura a saída padrão para fd[1]
        close(fd[1]); //este descritor de arquivo não é mais necessário
        }
    else{
        //o processo filho executa estes comandos
        read_command(str, command, parameters);

        if(cont>0){
            close(fd1[0]);
            dup(fd1[1]);
            write(fd1[1], &i, sizeof(i));
            close(fd1[1]);
        }

        close(fd[1]); //o processo 2 não precisa escrever o pipe
        dup(fd[0]); //configura a saída padrão para fd[0]
        close(fd[0]); //este descritor de arquivo não é mais necessário

        printf("Valor de i filho: %i\n",i);
        execvp(command, parameters);
}
}

int main(int argc, char *argv[]) {

    while(TRUE){
        printf("FERNANDO&BRUNO@SHELL$ ");
		str	= (char *)calloc(MAX_COM, sizeof (char));
        command = (char *)calloc(MAX_PAR, sizeof (char));
		parameters = aloca(MAX_PAR, MAX_PAR);

        read_commandline(str);

        cont = conta_pipe(str);
        cria_pipe();
	}
}