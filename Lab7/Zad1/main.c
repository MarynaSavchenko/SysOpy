#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h> 
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include "barberShop.h"

sigset_t mask, oldmask;
int SID = -1;
int SHMID = -1;
key_t key;
Fifo *fifo = NULL;
volatile int cltCurr = 0;


void finishShm(){

	if (shmdt(fifo) == -1) {ERROR("Error while closing shmdt\n");}
	else printf("Shm Fifo is detached\n");
}


int getPlace(){

	pid_t pid = getpid();
	int barber = semctl(SID,0, GETVAL,0);
	if (barber == -1) ERROR("Error while taking barber state\n");

	if (barber == 0){
		struct sembuf sops;
		sops.sem_num = BARBER;
		sops.sem_op = 1;
		sops.sem_flg = 0;
		if (semop(SID, &sops, 1) == -1) ERROR("Error while changing BARBER semaphore \n");
		printf("Client %d awaked barber, TIME: %ld \n", pid, Time());
		
		if (semop(SID, &sops, 1) == -1) ERROR("Error while changing BARBER semaphore \n");
		fifo -> chair = pid;
		return 1;
	}
	else{
		int a = pushFifo(fifo,pid);
		if (a == -1){
			printf("Client %d cant find free place, TIME:%ld\n", pid, Time());
		return -1;
		}
		else{
			printf("Client %d found free place, TIME:%ld\n", pid, Time());
		return 0;
		}

	}
}

void getCut(int cuts){

	while(cltCurr < cuts){
	struct sembuf sops;
	sops.sem_num = CHECK;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	if (semop(SID, &sops, 1) == -1) ERROR("Error while changing CHECK semaphore \n");

	int pl  = getPlace();
	sops.sem_op = 1;
	if (semop(SID, &sops, 1) == -1) ERROR("Error while changing CHECK semaphore \n");
	
	if (pl != -1){

		sigsuspend (&oldmask);
		printf("Client: %d got cut, TIME: %ld\n", getpid(),Time());
	}
	}



}

void sigMaxHandler(int signo){
	
	cltCurr++;

}

void openFifo(){

	char* path = getenv("HOME");
	if (path == NULL) ERROR("Error while getting HOME \n");

	key = ftok(path, BARBER_ID);
	if (key == -1) ERROR("Error while making key \n");

	SHMID = shmget(key, 0,0) ;
	if (SHMID == -1) ERROR("Error while opening shm\n");

	void *shmatCheck = shmat(SHMID, NULL, 0);
	if (shmatCheck == (void*) (-1)) ERROR("Error while shm getting adress\n");
	fifo = (Fifo*) shmatCheck;
	

}

void openSemaphores(){

	SID = semget(key,0,0);
	if (SID == -1) ERROR("Error while opening semaphores\n");

}

void intHandler(int signo){
	
	exit(1);

}

int main(int argc, char** argv){

	if (argc != 3) ERROR("Not right amount of args \n");
	int clients = atoi(argv[1]);
	int cuts = atoi(argv[2]);

	if (atexit(finishShm) == -1) ERROR("Error finishing shm\n");

	signal(SIGRTMAX,sigMaxHandler);
	signal(SIGINT, intHandler);
	openFifo();
	printf("Fifo opened\n");

	openSemaphores();
	printf("Semaphores opened\n");
	sigemptyset (&mask);
	sigaddset (&mask, SIGRTMAX);
	sigprocmask (SIG_BLOCK, &mask, &oldmask);

	for (int i = 0; i < clients; i++){
		pid_t pid = fork();
		if (pid < 0) {ERROR("Error while creating a process \n");}
		else if(pid == 0) {
			getCut(cuts);
			return 0;
			
		}
	}
	printf("All clients created\n");
	while(1) wait(NULL);

	return 0;

	

}
