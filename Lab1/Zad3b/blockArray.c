#include "blockArray.h"
#include <limits.h>
#include <stdlib.h>

char staticArray[1000][1000];

BlockArray *createArray(int arraySize, int blockSize , bool isStatic){
	
	if (arraySize <= 0) return NULL;
	BlockArray* blarray = (BlockArray*) calloc(1, sizeof(BlockArray));
	if (isStatic) 
		blarray -> array =  (char**) staticArray;
	else 
		blarray -> array = (char**) calloc(arraySize, sizeof(char*));
	blarray -> arraySize = arraySize;
	blarray -> blockSize = blockSize;
	blarray -> isStatic = isStatic;
	return blarray;	
	
}

void addBlock(BlockArray* blarray, int index, char* block){
	
	if (blarray -> arraySize <= index) return;
	if (blarray -> blockSize <= strlen(block)) return;
	if (blarray -> array[index] != NULL) return;
	if (blarray -> isStatic) blarray -> array[index] = block;
	else blarray -> array[index] = (char*) calloc(blarray -> blockSize,sizeof(char));
strcpy(blarray -> array[index],block);

}

void deleteBlock(BlockArray* blarray, int index){
	
	if (blarray -> isStatic){
		blarray -> array[index] = "";
		return;
	}
	if (blarray -> array[index] == NULL) 
		return;
	free(blarray -> array[index]);
	blarray -> array[index] = NULL;
}

void deleteArray(BlockArray* blarray){
	
	for ( int i = 0; i < blarray -> arraySize; i++)
		deleteBlock(blarray, i);
	if(!blarray -> isStatic) {
		free(blarray -> array);
		free(blarray);
	}
}

int blockToAscii(char* block){
	
	int sum = 0;
	for (int i = 0; i < strlen(block);i++)
		sum += (int)block[i];
	return sum;
}


char* findAscii(BlockArray* blarray, int number){

int minDif = INT_MAX;
int dif = 0;
char* closestBlock = NULL;
char* block = NULL;

for(int i = 0; i < blarray -> arraySize; i++){
	block = blarray -> array[i];
	if (block != NULL) {
		dif = abs(blockToAscii(block) - number);
		if ( dif < minDif )
		{
			closestBlock = block;
			minDif = dif;
		}
	}
}
return closestBlock;
}
		
	

