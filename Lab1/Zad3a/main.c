#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "blockArray.h"
#include <sys/times.h>



void initTime(clock_t* startTimeReal, struct tms* startTimeProc){
	
*startTimeReal = clock();  // Zwraca liczbę cykli, które upłynęły od chwili uruchomienia programu.
times(startTimeProc); //The times function returns information about a process’ consumption of processor time in a struct tms object, in addition to the process’ CPU time. 

}



void printTime(clock_t* startTimeReal, struct tms* startTimeProc, clock_t* endTimeReal, struct tms* endTimeProc, FILE* file){
	
*endTimeReal = clock();
times(endTimeProc);

double realTime = (double)(*endTimeReal - *startTimeReal)/CLOCKS_PER_SEC;
double userTime = (double)(endTimeProc -> tms_utime - startTimeProc -> tms_utime)/CLOCKS_PER_SEC;
double procTime = (double)(endTimeProc -> tms_stime - startTimeProc -> tms_stime)/CLOCKS_PER_SEC;

printf("realTime : %f\n", realTime);
printf("userTime : %f\n", userTime);
printf("procTime : %f\n", procTime);

fprintf(file, "realTime : %f\n", realTime);
fprintf(file, "userTime : %f\n", userTime);
fprintf(file, "procTime : %f\n", procTime);

}

char* generateString(int blockSize){
	
	char *possible = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
    size_t stringLen = strlen(possible);        
    char *randomString = malloc(stringLen*sizeof(char));
	int randomStringLen = blockSize - rand()%blockSize;
	
	for( int i = 0; i < randomStringLen; i++)
		randomString[i] = possible[rand()%stringLen];
	
	for (int i = randomStringLen;i<blockSize;i++)
		randomString[i] = '\0';
	
	return randomString;
}	

void deleteSomeBlocks(BlockArray* blarray, int start, int finish){

	for (int i = start; i < finish; i++)
		deleteBlock(blarray, i);
	
}

void addSomeBlocks(BlockArray* blarray, int start, int finish){
	
	for (int i = start; i < finish; i++)
		addBlock(blarray, i, generateString(blarray -> blockSize));
	
}
	
	


/*[1-size, 2-blocksize, 3-static/dynamic, 4-element, 5 - start, 6 - finish
*/
int main(int argc, char** argv) {

srand(time(0)); 

if (argc<6) {
printf("Few args\n");
return 0;
}

bool isStatic = false;
int arraySize = atoi(argv[1]);
int blockSize = atoi(argv[2]);
int element = atoi(argv[4]);
int start = atoi(argv[5]);
int finish = atoi(argv[6]);

if (strcmp(argv[3], "dynamic")) isStatic = false ;
else if  (strcmp(argv[3], "static")) isStatic = true;
else {
printf("The third arg must be 'static' or 'dynamik'\n");
return 0;
}
 
clock_t* startTimeReal = malloc(sizeof(clock_t)); 
clock_t* endTimeReal = malloc(sizeof(clock_t));
struct tms* startTimeProc = malloc(sizeof(struct tms));
struct tms* endTimeProc = malloc(sizeof(struct tms));

FILE  * file = fopen("raport2.txt", "a");
if (file ==NULL) {
printf("File cant be opened\n");
return 1;
}
fprintf(file,"\n\nCreating Array with arraySize : %d, blockSize : %d, isStatic: %d\n", arraySize, blockSize,isStatic);
initTime(startTimeReal, startTimeProc);
BlockArray* blarray = createArray(arraySize, blockSize, isStatic);
printf("Time for creating Array:\n");
fprintf(file,"\nTime for creating Array:\n");
printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);

initTime(startTimeReal, startTimeProc);
for (int i = 0; i < arraySize; i++)
	addBlock(blarray, i,generateString(blarray -> blockSize));
printf("Time for initialisation Array:\n");
fprintf(file,"\nTime for initialisation Array:\n");
printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);

initTime(startTimeReal, startTimeProc);
findAscii(blarray, element);
printf("Time for searching element in Array:\n");
fprintf(file,"\nTime for searching element in Array:\n");
printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);

initTime(startTimeReal, startTimeProc);
deleteSomeBlocks(blarray, start, finish);
printf("Time for deleting some blocks in Array:\n");
fprintf(file,"\nTime for deleting some blocks  in Array:\n");
printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);

initTime(startTimeReal, startTimeProc);
addSomeBlocks(blarray, start, finish);
printf("Time for adding some blocks  in Array:\n");
fprintf(file,"\nTime for adding some blocks in Array:\n");
printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);

initTime(startTimeReal, startTimeProc);
deleteSomeBlocks(blarray, start, finish);
addSomeBlocks(blarray, start, finish);
printf("Time for deleting and adding some blocks in Array:\n");
fprintf(file,"\nTime for for deleting and adding some blocks in Array:\n");
printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);

fclose(file);
return 0;
}


