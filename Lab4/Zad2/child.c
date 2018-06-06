#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

static void handler(int signo){

	printf("Child %d sending signal to parent\n", getpid());
	kill(getppid(), SIGRTMIN + rand()%(SIGRTMAX - SIGRTMIN));

}


int main(){
	

	signal(SIGUSR1, handler);
	sigset_t mask;
    	sigfillset(&mask);
    	sigdelset(&mask, SIGUSR1);
	srand(getpid());
	int sleeptime = rand()%11;
	printf("I am child with pid: %d, sleeping for %d sek:)\n", getpid(), sleeptime);
	sleep(sleeptime);
	printf("Child %d sending SIGUSR1 to parent:)\n", getpid());
	kill(getppid(),SIGUSR1);
	sigsuspend(&mask);

	return sleeptime;
}
