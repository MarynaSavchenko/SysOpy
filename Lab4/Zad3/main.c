#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int L, T;
pid_t pid;
int recieved_from_parent = 0;
int recieved_from_child = 0;
int send_to_child = 0;


void print_end(){

	
	printf("Parent has recieved from child: %d signals \n", recieved_from_child);
	printf("Parent has send to child: %d signals \n", send_to_child);

}


void child_handler(int signo, siginfo_t *info, void *context){
	
	if (info -> si_pid != getppid()) return;
	
	if (T==1 || T==2){
		if (signo == SIGUSR1){
			recieved_from_parent++;
			printf("Child recieved and send SIGUSR1 to parent :), %d \n",recieved_from_parent);
			kill(getppid(), SIGUSR1);
		}
		else if (signo == SIGUSR2){
			recieved_from_parent++;
			printf("Child recieved SIGUSR2 :) \n");
			printf("Child has recieved from parent: %d signals \n", recieved_from_parent);
			exit(0);
		}
	}
	if(T==3){
		if (signo == SIGRTMIN){
			recieved_from_parent++;
			printf("Child recieved and send SIGMIN to parent :) , %d \n",recieved_from_parent);
			kill(getppid(), SIGRTMIN);
		}
		else if (signo == SIGRTMAX){
			recieved_from_parent++;
			printf("Child recieved SIGMAX :) \n");
			printf("Child has recieved from parent: %d signals \n", recieved_from_parent);
			exit(0);
		}
	}



}

void child(){

	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = child_handler;
	sigset_t mask;
    	sigfillset(&mask);
    		
	if (T==1 || T==2){
		sigdelset(&mask, SIGUSR1);
		sigdelset(&mask, SIGUSR2);
		if (sigaction(SIGUSR1, &act, NULL) == -1) {
		printf("Error while catching SIGUSR1:(\n");
		exit(EXIT_FAILURE);
		}
		else if(sigaction(SIGUSR2, &act, NULL) == -1) {
		printf("Error while catching SIGUSR2:(\n");
		exit(EXIT_FAILURE);
		}
	}
	if (T==3){
		sigdelset(&mask, SIGRTMIN);
		sigdelset(&mask, SIGRTMAX);
		if (sigaction(SIGRTMIN, &act, NULL) == -1) {
		printf("Error while catching SIGRTMIN:(\n");
		exit(EXIT_FAILURE);
		}
		else if(sigaction(SIGRTMAX, &act, NULL) == -1) {
		printf("Error while catching SIGRTMAX:(\n");
		exit(EXIT_FAILURE);
		}
	}
	
	while(1) sleep(1);

		

}
void parent_handler(int signo, siginfo_t *info, void *context){

	if (signo == SIGINT) {
		printf("Parent recieved SIGINT :( \n");
		if (T == 1 || T == 2) {
			kill(pid,SIGUSR2);
			send_to_child++;
		}
		if (T == 3 ){
			kill(pid, SIGRTMAX);
			send_to_child++;
		}
	exit(0);
	}
	if (info -> si_pid != pid) return;
	if ((signo == SIGUSR1) && ( T==1 || T==2)) {
		printf("Parent recieved SIGUSR1 from child :) \n");
		recieved_from_child++;
	}
	if (signo == SIGRTMIN && T==3){
		printf("Parent recieved SIGRTMIN from child :) \n");
		recieved_from_child++;
	}
		

}

void parent(){
	
	sleep(1);
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = parent_handler;

	if (sigaction(SIGINT, &act, NULL) == -1) {
		printf("Error while catching SIGINT :(\n");
		exit(EXIT_FAILURE);
	}
		
	if (T==1 || T==2) if (sigaction(SIGUSR1, &act, NULL) == -1) {
		printf("Error while catching SIGUSR1 from child :(\n");
		exit(EXIT_FAILURE);
	}
	if (T==3) if(sigaction(SIGRTMIN, &act, NULL) == -1) {
		printf("Error while catching SIGRTMIN from child :(\n");
		exit(EXIT_FAILURE);
	}

	sigset_t mask;
    		sigfillset(&mask);

	
	if (T==1 || T==2){
		
		
    		sigdelset(&mask, SIGUSR1);
		sigdelset(&mask, SIGINT);
		for ( ; send_to_child < L ; send_to_child++){
			printf("Parent is sending SIGUSR1 to child :) \n");
			kill(pid, SIGUSR1);
			if (T==2) sigsuspend(&mask);
		}
	printf("Parent is sending SIGUSR2 to child :) \n");
	send_to_child++;
	kill(pid, SIGUSR2);
	}
	else{
		sigdelset(&mask, SIGRTMIN);
		sigdelset(&mask, SIGINT);
		for ( ; send_to_child < L ; send_to_child++){
			printf("Parent is sending SIGRTMIN to child :) \n");
			kill(pid, SIGRTMIN);
		}
	printf("Parent is sending SIGRTMAX to child :) \n");
	send_to_child++;
	kill(pid, SIGRTMAX);
	}

	waitpid(pid, NULL, 0);

}



int main(int argc, char* argv[]){

	if(argc != 3) {
		printf("Wrong amount of args :( \n");
		exit(EXIT_FAILURE);
	}
	L = atoi(argv[1]);
	T = atoi(argv[2]);
	if (L < 1) {
		printf("L should be 1 or more \n");
		exit(EXIT_FAILURE);
	}
	if (T < 1 || T > 3) {
		printf("Type can be 1, 2 or 3 \n");
		exit(EXIT_FAILURE);
	}
	
	pid = fork();
	if (pid < 0) {
		printf("Error while creating a child :( \n");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0) child();
	else parent();
	print_end();
	return 0;
}
