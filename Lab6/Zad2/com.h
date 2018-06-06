#ifndef COM_H
#define COM_H

#include <unistd.h>
#include <sys/types.h>
#define MAX_CLIENT 10
#define MAX_COUNT 2048
#define SERVER_ID 123
#define MAX_MSG_SIZE 10
#define MSG_SIZE  sizeof(msg) - sizeof(long)
#define serverPath "/server"
typedef enum mtype{
REG_MSG = 1, TIME_MSG = 2, MIRROR_MSG  = 3, CALC_MSG = 4, END_MSG = 5, STOP_MSG = 6
}mtype;



typedef struct msg{
	long mtype;
	pid_t senderPID;
	char count[MAX_COUNT];
}msg;



#endif


