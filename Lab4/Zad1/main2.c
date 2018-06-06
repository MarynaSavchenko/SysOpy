#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int waiting = 0;
int no_child = 1;
pid_t pid = 0;


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

	struct sigaction act;
	act.sa_handler = sig_usr;
	sigaction(SIGTSTP, &act, NULL);
	signal(SIGINT, sig_usr);	
	while(1){
	
	if  (waiting){
		if(!no_child){
			kill(pid, SIGKILL);
			no_child = 1;
		}
	}
	else {
		if(no_child){
			no_child = 0; 
			pid = fork();
			if (pid == 0) {
				execl("./date.sh","./date.sh", NULL);
				exit(EXIT_FAILURE);
			}
		}
			

	}
	}


	return 0;
}
