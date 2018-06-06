#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h> 
#include <signal.h>
#include "barberShop.h"


int SID = -1;
int SHMID = -1;
key_t key;
Fifo *fifo = NULL;

void finishWork(){

	if (shmdt(fifo) == -1) {ERROR("Error while detaching shm fifo\n");}
	else {printf("Shm Fifo is detached\n");}

	if (shmctl(SHMID,IPC_RMID,0) == -1) {ERROR("Error while deleting shm fifo\n");}
	else {printf("Shm fifo deleted\n");}
	
	if (semctl(SID,0,IPC_RMID) == -1) {ERROR("Error while deleting semaphores\n");}
	else {printf("Semaphores deleted\n");}


}

void cut(pid_t client){

	printf("Barber starts cut %d, TIME: %ld \n", client, Time());
	kill(client, SIGRTMAX);
	printf("Barber finishes cut %d, TIME: %ld \n", client, Time());
	

}

void initBarber(){

	struct sembuf sops;
	sops.sem_flg = 0;
	while(1){
		sops.sem_num = BARBER;
		sops.sem_op = -1;
	 	if (semop(SID,&sops, 1) == -1) ERROR("Error while chaging BARBER semaphor\n");
		pid_t client_pid = fifo->chair;
		cut(client_pid);
		while(1){
			client_pid = popFifo(fifo);
			if (client_pid != -1){
				cut(client_pid);
			}
			else{
				printf("Barber is going to sleep, TIME: %ld\n",Time());
				if (semop(SID,&sops, 1) == -1) ERROR("Error while chaging BARBER semaphor\n");
				break;
			}

		}
	}
	

}


void makeFifo(int max){

	char* path = getenv("HOME");
	if (path == NULL) ERROR("Error while getting HOME \n");

	key = ftok(path, BARBER_ID);
	if (key == -1) ERROR("Error while making key \n");

	SHMID = shmget(key, sizeof(Fifo), IPC_PRIVATE | 0666);
	if (SHMID == -1) ERROR("Error while creating shm\n");
	
	void *shmatCheck = shmat(SHMID, NULL, 0);
	if (shmatCheck == (void*) (-1)) ERROR("Error while shm getting adress\n");
	fifo = (Fifo*) shmatCheck;
	FifoInit(fifo, max);

}


void makeSemaphores(){

	SID = semget(key,2,IPC_PRIVATE  | 0666);
	if (SID == -1) ERROR("Error while creating semaphors\n");
	if (semctl(SID, 0, SETVAL,0) == -1) ERROR("Error while setting value to BARBER");
	if (semctl(SID, 1, SETVAL, 1) == -1) ERROR("Error while setting value to CHECK");
	


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
