#ifndef BARBERSHOP_H
#define BARBERSHOP_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR(Error) {printf(Error); exit(1);}
#define BARBER_ID 11

const char *barberPath;
const char *checkPath;
const char *fifoPath;


typedef struct Fifo{
	int max;
	int head;
	int tail;
	pid_t chair;
	pid_t queue[1000];
}Fifo;

void FifoInit(Fifo *fifo, int num);

pid_t popFifo(Fifo *fifo);

int pushFifo(Fifo *fifo, pid_t el);

long Time();

#endif
