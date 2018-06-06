#include <sys/types.h> 
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


#define maxline 100
#define ERROR(text){printf(text);exit(EXIT_FAILURE);}

int main(int argc, char** argv){
	printf("slave\n");

	if (argc != 3) ERROR("Wrong amount of args :( \n");
	int pipe = open(argv[1],O_WRONLY);
	if (pipe < 0) ERROR("Slave cant open pipe :( \n");
	printf("Slave %d\n", getpid());
	//write(1,getpid(), sizeof(pid_t));
	char buff1[maxline];
	char buff2[maxline];
	int N = atoi(argv[2]);
	for ( int i = 0; i < N; i++){
		FILE *date = popen("date","r");
		fgets(buff1, maxline , date);
		sprintf(buff2, "Pid : %d, date: %s",getpid(), buff1);
		write(pipe, buff2, strlen(buff2));
		pclose(date);
		sleep(2);
	}	
	close(pipe);
	return 0;
}
