#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#define maxargs 100
#define maxlen 100
#define maxline 100
#define print_error(mess){printf(mess);exit(1);}

char** parse_arg(char* text){

	
	char** line = malloc(sizeof(char*)*maxargs);
	for (int i = 0; i < maxargs; i++)
		line = malloc(sizeof(char)*maxlen);


	char deter[3] = {'\t' , '\n', ' '};
	int num = 0;
	while ((line[num]= strtok(num == 0 ? text : NULL, deter)) != NULL) num++;

	return line;
}


void reading_line(char* line){

	int num = 0;
	int fd[2][2];
	pid_t pid;
	char* token[maxline];
	while ((token[num] = strtok(num == 0 ? line : NULL , "|")) != NULL) num++;
	for (int i = 0 ; i < num; i++){

		if (i > 1) {
			close(fd[i%2][0]);
			close(fd[i%2][1]);
		}

		if ( i < num) {if (pipe(fd[i%2]) < 0) print_error("Error while piping :( \n");}
		pid = fork();
		if (pid == 0){
			char **arg = parse_arg(token[i]);
			
			if (i != num-1){
				close(fd[i%2][0]);
				if (dup2(fd[i%2][1],STDOUT_FILENO) < 0) print_error("Error while dup :( \n");
			}
			if (i != 0){
				close(fd[(i+1)%2][1]);
				if (dup2(fd[(i+1)%2][0],STDIN_FILENO) < 0) print_error("Error while dup :( \n");
			}
			execvp(arg[0], arg);
			exit(EXIT_SUCCESS);
		}
		//else wait(NULL);	
	


	}

	wait(NULL);
	exit(EXIT_SUCCESS);
}


int main(int argc, char** argv){
	
	char line[256];

	if (argc != 2) {
		printf("Wrong amount of args:(\n");
		return 1;
	}
	pid_t pid;
	FILE * file = fopen(argv[1], "r");
	if ( file == NULL) print_error("Error while opening the file:(\n");
	while (fgets(line, sizeof(line),file)){

		printf("\n\nLINE :%s\n", line);
		pid = fork();
		if (pid<0) {print_error("Error creating a new proces:(\n");}
		else if (pid == 0) {
			reading_line(line);
			exit(EXIT_SUCCESS);

			}
		wait(NULL); 
    		
	}		

fclose(file);
return 0;
}
