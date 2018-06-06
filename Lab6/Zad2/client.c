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
int numID = -1,state = 1;
mqd_t serverID = - 1;
mqd_t clientID = -1;
char path[10];

void deleteQueue(){

	if (clientID>-1){
		if (numID >= 0){
			msg buf;
			buf.senderPID = getpid();
			buf.mtype = STOP_MSG;
			if (mq_send(serverID, (char*) &buf, MSG_SIZE, 0) == -1) ERROR("Error while sending message to server\n");
		}
	if (mq_close(serverID) == -1) {ERROR("Error while closing server queue\n");}
	else {printf("Server queue was closed\n");}

	if (mq_close(clientID) == -1) {ERROR("Error while closing server queue\n");}
	else {printf("Clientr queue was closed\n");}
	
	if (mq_unlink(path) == -1) {ERROR("Error while deleting client queue\n");}
	else {printf("Client queue was deleted\n");}


	}
}




void intHandler(){

	exit(1);

}

void regist(){

	msg buf;
	buf.mtype = REG_MSG;
	buf.senderPID = getpid();
	
	if (mq_send(serverID, (char*) &buf,MSG_SIZE, 0) == -1) ERROR("Error while sending clientKey to server\n");
	if (mq_receive(clientID, (char*) &buf, MSG_SIZE, 0) == -1) ERROR("Error while client recieving numID\n");
	if(sscanf(buf.count, "%d",&numID) == -1) ERROR("Error\n");
	if (numID < 0 ) ERROR("NO more clients allowed\n");
	printf("Client got his numberID : %d\n", numID);	 
	

}


void time(msg* buf){

	buf->mtype = TIME_MSG;
	if (mq_send(serverID, (char*)buf, MSG_SIZE, 0) == -1) ERROR("Error while sending message to server\n");
	if (mq_receive(clientID, (char*)buf, MSG_SIZE, 0) == -1) ERROR("Error while recieving message from server\n");
	printf("%s\n", buf->count);
}

void end(msg* buf){

	buf->mtype = END_MSG;
	if (mq_send(serverID, (char*)buf, MSG_SIZE, 0) == -1) ERROR("Error while sending message to server\n");


}

void mirror(msg* buf){

	buf->mtype = MIRROR_MSG;
	printf("Enter string:\n");
	if (fgets(buf->count, MAX_COUNT, stdin) == NULL){
		printf("Error while reading mirror\n");
		return;
	}
	if (mq_send(serverID, (char*)buf, MSG_SIZE, 0) == -1) ERROR("Error while sending mirror msg to server\n");
	if (mq_receive(clientID, (char*)buf, MSG_SIZE,0) == -1) ERROR("Error while client recieving MIRROR_MSG from server\n");
	printf("%s\n", buf->count);

}

void stoped(msg* buf){

	buf->mtype = STOP_MSG;
	if (mq_send(serverID, (char*)buf, MSG_SIZE, 0) == -1) ERROR("Error while sending message to server\n");
	state = 0;
	numID = -1;
}

void calc(msg* buf){

	buf->mtype = CALC_MSG;
	printf("Enter string for counting:\n");
	if (fgets(buf->count, MAX_COUNT, stdin) == NULL){
		printf("Error while reading mirror\n");
		return;
	}
	if (mq_send(serverID, (char*)buf, MSG_SIZE, 0) == -1) ERROR("Error while sending CALC_MSG to server\n");
	if (mq_receive(clientID, (char*)buf, MSG_SIZE,0) == -1) ERROR("Error while client recieving CALC_MSG from server\n");
	printf("%s\n", buf->count);
	
	
}

int main(){


	if (atexit(deleteQueue) == -1) ERROR("Error while atexiting\n");
	if (signal(SIGINT, intHandler) == SIG_ERR) ERROR("Error while recieving SIGINT \n");

	sprintf(path, "/%d", getpid());

	serverID = mq_open(serverPath, O_WRONLY);
	if (serverID == -1) ERROR("Error while opening server queue\n");
	struct mq_attr attr;
	attr.mq_maxmsg = MAX_MSG_SIZE;
        attr.mq_msgsize = MSG_SIZE;

	

	clientID = mq_open(path,O_CREAT|O_EXCL,0666,&attr);
	if (clientID == -1) {ERROR("Error while creating client queque\n");}
	else printf("Client queue was created\n");


	regist();

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
