#include <stdbool.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>

typedef struct BlockArray{
	char** array;
	int arraySize;
	int blockSize;
	bool isStatic;
} BlockArray; 

extern char staticArray[1000][1000];

BlockArray *createArray(int arraySize, int blockSize , bool isStatic);

void addBlock(BlockArray* blarray, int index, char* block);

void deleteBlock(BlockArray* blarray, int index);

void deleteArray(BlockArray* blarray);

int blockToAscii(char* block);

char* findAscii(BlockArray* blarray, int index);

