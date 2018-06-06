#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>


int main(int argc, char** argv){
	
	int max_number_args = 64;
	char* token[max_number_args];
	char line[256];

	if (argc != 2) {
		printf("Wrong amount of args:(\n");
		return 1;
	}

	FILE * file = fopen(argv[1], "r");
	if ( file == NULL) {
		printf("Error while opening the file:(\n");
		return 1;
	}

	while (fgets(line, sizeof(line),file)){
		
		int number = 0;
		int child_status;
		
		while( (token[number] = strtok(number == 0 ? line :NULL," " )) != NULL)
			number++;
		token[number]='\0';
		for(int i = number-1; i < max_number_args; i++)
			token[i]=NULL;

		
                
		if ( number > max_number_args) {
			printf("Too much args:(\n");
			return 1;
		}
		printf("%d\n", number);
		for( int i  =0 ; i <= number; i++)
			printf("number: %d,%s\n", i,token[i]);

		pid_t pid = fork();
		if (pid<0) {
			printf("Error creating a new proces:(\n");
			return 1;
		}
		else if (pid == 0) {
			execvp(token[0], token);
			printf("Error while exec\n");
			return 1;
			}
		
			
		else{
			wait(NULL);       	
    		 	
		}

			
} 	



fclose(file);
return 0;
}
