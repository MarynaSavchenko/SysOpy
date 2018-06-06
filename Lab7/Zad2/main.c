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
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "barberShop.h"

sigset_t mask, oldmask;
Fifo *fifo = NULL;
sem_t *BARBER;
sem_t *CHECK;
volatile int cltCurr = 0;


void finishShm(){

	if (munmap(fifo, sizeof(Fifo)) == -1) {ERROR("Error while detaching shm fifo\n");}
	else {printf("Shm Fifo is detached\n");}

	if (sem_close(BARBER) == -1) {ERROR("Error while deleting semaphores\n");}
	else {printf("BARBER semaphore deleted\n");}

	if (sem_close(CHECK) == -1) {ERROR("Error while deleting semaphores\n");}
	else {printf("CHECK semaphor deleted\n");}
}


int getPlace(){

	pid_t pid = getpid();
	int barber = -1;
	if (sem_getvalue(BARBER, &barber) == -1) ERROR("Error while taking barber state\n") ;

	if (barber == 0){
		
		if (sem_post(BARBER) == -1) ERROR("Error while changing BARBER semaphore \n");
		printf("Client %d awaked barber, TIME: %ld \n", pid, Time());
		
		if (sem_post(BARBER) == -1) ERROR("Error while changing BARBER semaphore \n");
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
	
	if (sem_wait(CHECK) == -1) ERROR("Error while changing CHECK semaphore \n");

	int pl  = getPlace();
	if (sem_post(CHECK) == -1) ERROR("Error while changing CHECK semaphore \n");
	
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

	int SHMID = shm_open(fifoPath,O_RDWR,0);
	if (SHMID == -1) ERROR("Error while making key \n");

	
	void *shmatCheck = mmap(NULL, sizeof(Fifo),PROT_WRITE|PROT_READ, MAP_SHARED, SHMID,0);
	if (shmatCheck == (void*) (-1)) ERROR("Error while shm getting adress\n");
	fifo = (Fifo*) shmatCheck;
	

}

void openSemaphores(){

	BARBER = sem_open(barberPath, O_RDWR);
	if (BARBER == SEM_FAILED) ERROR("Error while creating BARBER semaphor\n");
	CHECK = sem_open(checkPath, O_RDWR);
	if (CHECK == SEM_FAILED) ERROR("Error while creating CHECK semaphor\n")

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
