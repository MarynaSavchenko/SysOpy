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


int clientAmount = 0, state = 1;
int clients[MAX_CLIENT][2];
int serverID = -1;


void intHandler(){

	exit(1);

}
int findID(struct msg* buf){

	for(int i = 0; i < clientAmount;i++)
		if(clients[i][0] == buf->senderPID) return clients[i][1];
	return -1;
}
void deleteQueue(){

	if (serverID>-1){
		int check = msgctl(serverID, IPC_RMID, 0);
		if ( check == -1) ERROR("Error while deleting serverQueue\n");
		printf("Server queue deleted\n");
	}
}

void registr(struct msg* buf){


	key_t clientKey;
	if (sscanf(buf->count, "%d", &clientKey) == -1) ERROR("Error while getting clientKey\n");
	int clientID = msgget(clientKey, 0);
	if (clientID == -1) ERROR("Error while opening client \n");

	int clientPID = buf -> senderPID;
	buf->mtype = REG_MSG;
	buf->senderPID= getpid();

	if (clientAmount > MAX_CLIENT - 1) {
		printf("Max amount of clients already exists\n");
		sprintf(buf->count, "%d", -1);
	}
	else {
		clients[clientAmount][0] = clientPID;
		clients[clientAmount][1] = clientID;
		clientAmount++;
		sprintf(buf->count, "%d", clientAmount - 1);
	}

	if (msgsnd(clientID, buf, MSG_SIZE,0) == -1) ERROR("Error while sending clientID to client\n");
}

void time(struct msg* buf){

	int clientID = findID(buf);
	if (clientID == -1 ) {printf("There is no such client \n");return;}
	buf->mtype = TIME_MSG;
	buf->senderPID = getpid();
	
	FILE* date = popen("date", "r");
	if (date < 0) ERROR("Error while getting date\n");
	fgets(buf->count,MAX_COUNT ,date);
	pclose(date);
	if(msgsnd(clientID, buf, MSG_SIZE,0) == -1) ERROR("Error while sending time\n");
}

void mirror(struct msg* buf){

	int clientID = findID(buf);
	if (clientID == -1 ) {printf("There is no such client \n");return;}
	buf->mtype = MIRROR_MSG;
	buf->senderPID = getpid();

	int bufLen = strlen(buf->count);
	if (buf->count[bufLen - 1] == '\n') bufLen--;

	for (int i = 0; i < bufLen/2; i++){
		char c = buf -> count[i];
		buf->count[i]= buf-> count[bufLen - i - 1];
		buf->count[bufLen - i - 1] = c;
	}

	if (msgsnd(clientID, buf, MSG_SIZE, 0) == -1) ERROR("Error while mirroring \n");
}


void calc(struct msg* buf){

	int clientID = findID(buf);
	if (clientID == -1 ) {printf("There is no such client \n");return;}
	buf->mtype = CALC_MSG;
	buf->senderPID = getpid();

	char tmp[100];
	sprintf(tmp, "echo '%s' | bc", buf->count);
	FILE* calc = popen(tmp, "r");
	fgets(buf->count, MAX_COUNT, calc);
	pclose(calc);
	if (msgsnd(clientID, buf, MSG_SIZE, 0) == -1) ERROR("Error while sending calc answer\n");


}

void stoped(struct msg* buf){

	for(int i = 0; i < clientAmount;i++){
		if(clients[i][0] == buf->senderPID) {
			for (int j = i; j < clientAmount;j++){
				clients[j][0]=clients[j+1][0];
				clients[j][1]=clients[j+1][1];
			}

			clientAmount--;
			break;
	}	}
}



int main(){

	if (atexit(deleteQueue) == -1) ERROR("Error while atexiting\n");
	if (signal(SIGINT, intHandler) == SIG_ERR) ERROR("Error while recieving SIGINT \n");

	char* path = getenv("HOME");
	if (path == NULL) ERROR("Error while getting HOME \n");

	key_t key = ftok(path, SERVER_ID);
	if (key == -1) ERROR("Error while making key \n");

	serverID = msgget(key,IPC_CREAT|IPC_EXCL|0666);
	if (serverID == -1) {ERROR("Error while creating serwer queque\n");}
	else printf("Server queue was created\n");

	msg buf;
	struct msqid_ds check;
	while(1){
		if (state == 0){
			if(msgctl(serverID,IPC_STAT, &check) == -1)ERROR("Error while checking state of queue\n");
			if(check.msg_qnum==0) break;
		}
		if (msgrcv(serverID, &buf, MSG_SIZE,0,0) == -1) ERROR("Error while server receiving a comm \n");

	switch(buf.mtype){
		case REG_MSG:
			registr(&buf);
			break;
		case MIRROR_MSG:
			mirror(&buf);
			break;
		case CALC_MSG:
			calc(&buf);
			break;
		case TIME_MSG:
			time(&buf);
			break;
		case END_MSG:
			state = 0;
			break;
		case STOP_MSG:
			stoped(&buf);
			break;
		default:
			break;
		}

	}
	return 0;
}
