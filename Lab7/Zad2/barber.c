#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h> 
#include <sys/sem.h> 
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "barberShop.h"


Fifo *fifo = NULL;
sem_t *BARBER;
sem_t *CHECK;

void finishWork(){

	if (munmap(fifo, sizeof(Fifo)) == -1) {ERROR("Error while detaching shm fifo\n");}
	else {printf("Shm Fifo is detached\n");}

	if (shm_unlink(fifoPath) == -1) {ERROR("Error while deleting shm fifo\n");}
	else {printf("Shm fifo deleted\n");}
	
	if (sem_close(BARBER) == -1) {ERROR("Error while deleting semaphores\n");}
	else {printf("BARBER semaphore deleted\n");}
	if (sem_close(CHECK) == -1) {ERROR("Error while deleting semaphores\n");}
	else {printf("CHECK semaphor deleted\n");}


}

void cut(pid_t client){

	printf("Barber starts cut %d, TIME: %ld \n", client, Time());
	kill(client, SIGRTMAX);
	printf("Barber finishes cut %d, TIME: %ld \n", client, Time());
	

}

void initBarber(){

	
	while(1){
		
	 	if (sem_wait(BARBER) == -1) ERROR("Error while chaging BARBER semaphor\n");
		pid_t client_pid = fifo->chair;
		cut(client_pid);
		while(1){
			client_pid = popFifo(fifo);
			if (client_pid != -1){
				cut(client_pid);
			}
			else{
				printf("Barber is going to sleep, TIME: %ld\n",Time());
				if (sem_wait(BARBER) == -1) ERROR("Error while chaging BARBER semaphor\n");
				break;
			}

		}
	}
	

}


void makeFifo(int max){

	

	int SHMID = shm_open(fifoPath,O_CREAT|O_EXCL|O_RDWR, 0666);
	if (SHMID == -1) ERROR("Error while making key \n");

	if(ftruncate(SHMID, sizeof(Fifo)) == -1)ERROR("Error while creating shm\n");
	
	void *shmatCheck = mmap(NULL, sizeof(Fifo),PROT_WRITE|PROT_READ, MAP_SHARED, SHMID,0);
	if (shmatCheck == (void*) (-1)) ERROR("Error while shm getting adress\n");
	fifo = (Fifo*) shmatCheck;
	FifoInit(fifo, max);

}


void makeSemaphores(){

	BARBER = sem_open(barberPath, O_CREAT|O_EXCL|O_RDWR, 0666,0);
	if (BARBER == SEM_FAILED) ERROR("Error while creating BARBER semaphor\n");
	CHECK = sem_open(checkPath, O_CREAT|O_EXCL|O_RDWR , 0666,1);
	if (CHECK== SEM_FAILED) ERROR("Error while creating CHECK semaphor\n");
	


}
void intHandler(int signo){
	
	exit(1);

}

int main(int argc, char** argv){

	if (argc != 2) ERROR("Wrong amount of args\n");
	int chairs = atoi(argv[1]);

	if (atexit(finishWork) == -1) ERROR("Error finishing work\n");
	signal(SIGINT, intHandler);

	makeFifo(chairs);
	printf("Fifo created\n");

	makeSemaphores();
	printf("Semaphores created\n");
	initBarber();


	return 0;
}
