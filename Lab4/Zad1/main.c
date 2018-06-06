#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int waiting = 0;


void get_current_time(){

	time_t current_time;
	struct tm *local_time;
	time(&current_time);
	local_time =  localtime(&current_time);
	printf("Current time : %s\n", asctime(local_time));

}


static void sig_usr(int signo){

	if (signo == SIGTSTP){ 
		if (waiting == 0) printf("\nWaiting for CTRL+Z - continue or CTR+C - finishing the program\n");
		waiting = waiting == 1 ? 0 :1 ;	
	}

	if (signo == SIGINT){
		printf("\nRecieved SIGINT and exit\n");
		exit(0);
	}


}

int main(void)
{

	
	signal(SIGTSTP, sig_usr);
	signal(SIGINT, sig_usr);
	
	while(1){ 
		
		if (!waiting){
		get_current_time();
		sleep(1);
		}
		}
		



	return 0;
}
