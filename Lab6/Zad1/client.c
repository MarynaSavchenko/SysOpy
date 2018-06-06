#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "com.h"

#define ERROR(text){printf(text);exit(EXIT_FAILURE);}
int serverID = - 1, clientID = -1, numID = -1,state = 1;

void deleteQueue(){

	if (clientID>-1){
		int check = msgctl(clientID, IPC_RMID, 0);
		if ( check == -1) ERROR("Error while deleting serverQueue\n");
		printf("Client queue deleted\n");
	}
}

int getQueueID(char* path, int ID){

	key_t key = ftok(path, ID);
	if (key == -1) ERROR("Error while making key \n");

	int newID = msgget(key,0);
	if (newID == -1) {ERROR("Error while creating serwer queque\n");}
	return newID;

}


void intHandler(){

	msg buf;
	buf.mtype = STOP_MSG;
	buf.senderPID = getpid();
	if (msgsnd(serverID, &buf, MSG_SIZE, 0) == -1) ERROR("Error while sending message to server\n");
	exit(1);

}

void regist(key_t key){

	msg buf;
	buf.mtype = REG_MSG;
	buf.senderPID = getpid();
	sprintf(buf.count, "%d", key);
	
	if (msgsnd(serverID, &buf,MSG_SIZE, 0) == -1) ERROR("Error while sending clientKey to server\n");
	if (msgrcv(clientID, &buf, MSG_SIZE, 0,0) == -1) ERROR("Error while client recieving numID\n");
	if(sscanf(buf.count, "%d",&numID) == -1) ERROR("Error\n");
	if (numID < 0 ) ERROR("NO more clients allowed\n");
	printf("Client got his numberID : %d\n", numID);	 
	

}


void time(msg* buf){

	buf->mtype = TIME_MSG;
	if (msgsnd(serverID, buf, MSG_SIZE, 0) == -1) ERROR("Error while sending message to server\n");
	if (msgrcv(clientID, buf, MSG_SIZE, 0, 0) == -1) ERROR("Error while recieving message from server\n");
	printf("%s\n", buf->count);
}

void end(msg* buf){

	buf->mtype = END_MSG;
	if (msgsnd(serverID, buf, MSG_SIZE, 0) == -1) ERROR("Error while sending message to server\n");


}

void mirror(msg* buf){

	buf->mtype = MIRROR_MSG;
	printf("Enter string:\n");
	if (fgets(buf->count, MAX_COUNT, stdin) == NULL){
		printf("Error while reading mirror\n");
		return;
	}
	if (msgsnd(serverID, buf, MSG_SIZE, 0) == -1) ERROR("Error while sending mirror msg to server\n");
	if (msgrcv(clientID, buf, MSG_SIZE,0,0) == -1) ERROR("Error while client recieving MIRROR_MSG from server\n");
	printf("%s\n", buf->count);

}

void stoped(msg* buf){

	buf->mtype = STOP_MSG;
	if (msgsnd(serverID, buf, MSG_SIZE, 0) == -1) ERROR("Error while sending message to server\n");
	state = 0;
}

void calc(msg* buf){

	buf->mtype = CALC_MSG;
	printf("Enter string for counting:\n");
	if (fgets(buf->count, MAX_COUNT, stdin) == NULL){
		printf("Error while reading mirror\n");
		return;
	}
	if (msgsnd(serverID, buf, MSG_SIZE, 0) == -1) ERROR("Error while sending CALC_MSG to server\n");
	if (msgrcv(clientID, buf, MSG_SIZE,0,0) == -1) ERROR("Error while client recieving CALC_MSG from server\n");
	printf("%s\n", buf->count);
	
	
}

int main(){


	if (atexit(deleteQueue) == -1) ERROR("Error while atexiting\n");
	if (signal(SIGINT, intHandler) == SIG_ERR) ERROR("Error while recieving SIGINT \n");

	char* path = getenv("HOME");
	if (path == NULL) ERROR("Error while getting HOME \n");

	key_t clientKey = ftok(path, getpid());
	if (clientKey == -1) ERROR("Error while making key \n");

	serverID = getQueueID(path, SERVER_ID);


	clientID = msgget(clientKey,IPC_CREAT|IPC_EXCL|0666);
	if (clientID == -1) {ERROR("Error while creating client queque\n");}
	else printf("Client queue was created\n");
	regist(clientKey);

	msg buf;
	
	char tmp[30];
	while(1){

		if (state == 0) break;
		buf.senderPID = getpid();
		printf("Enter message : \n");
		if (fgets(tmp, 30, stdin) == NULL){
			printf("Not correct message\n");
			continue;
		}
		int n = strlen(tmp);
		if (tmp[n-1]== '\n') tmp[n-1]=0;
		if(strcmp(tmp, "CALC") == 0){calc(&buf);}
		else if(strcmp(tmp, "TIME") == 0){time(&buf);}
		else if(strcmp(tmp, "END") == 0){end(&buf);}
		else if(strcmp(tmp, "MIRROR") == 0){mirror(&buf);}
		else if(strcmp(tmp, "STOP") == 0){stoped(&buf);}
		else printf("Wrong message\n");
		
	}
	return 0;

}
