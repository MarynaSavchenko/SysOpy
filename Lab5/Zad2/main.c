#include <sys/types.h> 
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


#define ERROR(text){printf(text);exit(EXIT_FAILURE);}

int main(int argc, char**argv){

	if (argc != 4) ERROR("Wrong amount of args :( \n");
	int child = atoi(argv[2]);
	int N = atoi(argv[3]);
	pid_t childpid;
	printf("Creating a master \n");
	
	sleep(2);	
	printf("Making slaves \n");
	for ( int i = 0; i < child; i++){
		childpid = fork();
		if (childpid < 0) {ERROR("Error while creating a child :( \n");}
		else if (childpid == 0){
			execlp("./slave", "./slave", argv[1], N, NULL );
			ERROR("Error while slave doing his job :( \n");
		}
	}
pid_t masterpid = fork();
	if (masterpid < 0) {ERROR("Error creating master :( \n");}
	else if (masterpid == 0){
		execlp("./master","./master",argv[1], NULL);
		ERROR("Error while master doing his job :( \n");
	}
	
	while(wait(NULL));
	return 0;
}
