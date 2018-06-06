#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

int k;
int K;
int N;
int n;
pid_t* child_array;
pid_t* child_wait;

static void delete_child(pid_t pid){

	for ( int i = 0 ; i < N; i++){
		if (child_array[i] == pid) child_array[i] = 0;
		return;
	}

}

static int check_child(pid_t pid){
	
	for (int i = 0; i < N; i++)
		if(child_array[i] == pid) return 1;
	return -1;

}

static void child_handler(int signo, siginfo_t* info, void* context){

	printf("Parent has just get SIGUSR1 from child: %d :)\n", info -> si_pid);
	if (check_child(info -> si_pid) == -1) return;
	if ( k>=K ){
		printf("Sending SIGUSR1 from parent to child %d :)\n", info -> si_pid);
		kill(info -> si_pid, SIGUSR1);
		waitpid(info -> si_pid, NULL, 0);
	}
	else{
		
		child_wait[k++]=info -> si_pid;
		if ( k >= K ){
			for (int i = 0; i < k; i++){
				
				printf("Sending SIGUSR1 from parent to child %d :)\n", child_wait[i]);
				kill(child_wait[i], SIGUSR1);
				waitpid(child_wait[i], NULL, 0);
			}
		}
	}
}

static void fast_signal(int signo, siginfo_t* info, void* context){

	printf("Parent has just get %d from child: %d :)\n",signo, info -> si_pid); 


}

static void child_exit(int signo, siginfo_t* info, void* context){

	printf("Child %d has just finished this job with status: %d :)\n" , info -> si_pid, info -> si_status);
	n--;
	if (n == 0) {
		printf("All children have finished their jobs :)\n");
		free(child_array);
		free(child_wait);
		exit(EXIT_SUCCESS);
	}

	delete_child(info -> si_pid);
}

static void int_signal(int signo, siginfo_t* info, void* context){

	printf("Signal SIGINT\n");
	for (int i = 0; i < N; i++)
		if(child_array != 0 ){
			kill(child_array[i], SIGKILL);
			waitpid(child_array[i], NULL,0);
		}
	free(child_array);
	free(child_wait);
	exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[]){

	
	N = atoi(argv[1]);
	K = atoi(argv[2]);
	k = 0;	
	n = 0;
	if( argc < 3) {
		
		printf("Wrong number of args!:(\n");
		exit(EXIT_FAILURE);
	}
	if (N < 1){

		printf("N cant be less than 1!\n");
		exit(EXIT_FAILURE);
	}
	if (K < 1){

		printf("K cant be less than 1!\n");
		exit(EXIT_FAILURE);
	}
	if (N < K){

		printf("N cant be less than K!\n");
		exit(EXIT_FAILURE);
	}
	
	child_array = calloc(N, sizeof(pid_t));
	child_wait = calloc(K, sizeof(pid_t));
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = child_handler;
	if (sigaction(SIGUSR1, &act, NULL) == -1) {
		
		printf("Error while catching SIGUSR1:(\n");
		exit(EXIT_FAILURE);
	}

	act.sa_sigaction = child_exit;
	if (sigaction(SIGCHLD, &act, NULL) == -1) {
		
		printf("Error while catching SIGCHLD:(\n");
		exit(EXIT_FAILURE);
	}

	act.sa_sigaction = int_signal;
	if (sigaction(SIGINT, &act, NULL) == -1) {
		
		printf("Error while catching SIGINT:(\n");
		exit(EXIT_FAILURE);
	}
	act.sa_sigaction = fast_signal;
	for ( int i = SIGRTMIN; i <= SIGRTMAX; i++){
	if (sigaction(i, &act, NULL) == -1) {
		
		printf("Error while catching signal from child:(\n");
		exit(EXIT_FAILURE);
	}}
	
	
	printf("dad pid: %d\n",getpid());

	for (int i = 0; i < N; i++){
    
		pid_t pid = fork();
		if (pid < 0 ){
			printf("Error while makig a child:(\n");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0 ){
			printf("son: %d, parent:%d \n", getpid(),getppid());
			execl("./child","./child",NULL);
			//exit(0);
			exit(EXIT_FAILURE);
		}
		else child_array[n++] = pid;	
	}
	
		while(wait(NULL));
	

	return 0;
}
