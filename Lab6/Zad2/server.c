#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include "com.h"

#define ERROR(text){printf(text);exit(EXIT_FAILURE);}


int clientAmount = 0, state = 1;
int clients[MAX_CLIENT][2];
mqd_t serverID = -1;


void intHandler(){

	exit(1);

}
int findID(struct msg* buf){

	for(int i = 0; i < clientAmount;i++)
		if(clients[i][0] == buf->senderPID) return clients[i][1];
	return -1;
}
void deleteQueue(){

	for(int i = 0; i < clientAmount;i++){
			if (mq_close(clients[i][1]) == -1) ERROR("Error while closing queue\n");
			if (kill(clients[i][0],SIGINT) == -1) ERROR("Error while sending SIGINT to client\n");
			
	}

	if (serverID>-1){
		if (mq_close(serverID) == -1) {ERROR("Error closing server queue\n");}
		else {printf("Server queue closed\n");}
		if (mq_unlink(serverPath ) == -1) {ERROR("Error deleting server queue\n");}
		else {printf("Server queue was deleted\n");}
	}
}

void registr(struct msg* buf){

	int clientPID = buf -> senderPID;
	buf->mtype = REG_MSG;
	buf->senderPID= getpid();
	char clientPath[30];
	sprintf(clientPath, "/%d", clientPID);

	int clientID = mq_open(clientPath,O_WRONLY);
	if (clientID == -1) ERROR("Error while getting clientID");

	if (clientAmount > MAX_CLIENT - 1) {
		printf("Max amount of clients already exists\n");
		sprintf(buf->count, "%d", -1);
		if (mq_send(clientID, (char*) buf, MSG_SIZE,0) == -1) ERROR("Error while sending clientID to client\n");
		if (mq_close(clientID) == -1) ERROR("Error while closing client queue\n");
	}
	else {
		clients[clientAmount][0] = clientPID;
		clients[clientAmount][1] = clientID;
		clientAmount++;
		sprintf(buf->count, "%d", clientAmount - 1);
		if (mq_send(clientID, (char*) buf, MSG_SIZE,0) == -1) ERROR("Error while sending clientID to client\n");
	}
	
	
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
	if(mq_send(clientID, (char*)buf, MSG_SIZE,0) == -1) ERROR("Error while sending time\n");
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

	if (mq_send(clientID, (char*)buf, MSG_SIZE, 0) == -1) ERROR("Error while mirroring \n"); 
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
	if (mq_send(clientID, (char*)buf, MSG_SIZE, 0) == -1) ERROR("Error while sending calc answer\n");


}

void stoped(struct msg* buf){

	for(int i = 0; i < clientAmount;i++){
		if(clients[i][0] == buf->senderPID) {
			if (mq_close(clients[i][1]) == -1){ ERROR("Error while closing queue\n");}
			else {printf("Client queue was closed\n");}
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

	struct mq_attr attr;
	attr.mq_maxmsg = MAX_MSG_SIZE;
        attr.mq_msgsize = MSG_SIZE;

	

	serverID = mq_open(serverPath,O_CREAT|O_EXCL,0666,&attr);
	if (serverID == -1) {ERROR("Error while creating serwer queque\n");}
	else printf("Server queue was created\n");

	msg buf;
	struct mq_attr check;
	while(1){
		if (state == 0){
			if(mq_getattr(serverID,&check) == -1)ERROR("Error while checking state of queue\n");
			if(check.mq_curmsgs == 0) break;
		} 
		if (mq_receive(serverID, (char*) &buf, MSG_SIZE,NULL) == -1) ERROR("Error while server receiving a comm \n");
	
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


