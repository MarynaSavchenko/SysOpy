#include "barberShop.h"

void FifoInit(Fifo *fifo, int num){
	
	fifo->max = num;
	fifo->head = -1;
	fifo->tail = 0;
	fifo->chair = 0;

}

int isFifoEmpty(Fifo *fifo){
	if (fifo->head == -1) return 1;
	else return 0;
}

int isFifoFull(Fifo *fifo){
	if (fifo->tail == fifo->head) return 1;
	else return 0;
}


pid_t popFifo(Fifo *fifo){

	if (isFifoEmpty(fifo) == 1) return -1;

	fifo->chair = fifo->queue[fifo->head++];
	if (fifo -> head == fifo-> max) fifo-> head = 0;
	if (fifo -> head == fifo -> tail) fifo -> head = -1;
	return fifo->chair;
}

int pushFifo(Fifo *fifo, pid_t el){
	if (isFifoFull(fifo) ==1 ) return -1;
	if (isFifoEmpty(fifo) == 1)
		fifo->head = fifo->tail = 0;
	fifo->queue[fifo->tail++] = el;
	if (fifo->tail == fifo->max) fifo->tail = 0;
	return 0;
}

long Time(){
	struct timespec marker;
    	if (clock_gettime(CLOCK_MONOTONIC, &marker) == -1) ERROR("Getting time failed\n");
    	return marker.tv_nsec / 1000;
}


