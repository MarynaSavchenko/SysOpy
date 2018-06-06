#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
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
	
char *possible = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
size_t stringLen = strlen(possible);        
char *randomString = malloc((blockSize + 1)*sizeof(char));
	
for( int i = 0; i < blockSize; i++)
	randomString[i] = possible[rand()%stringLen];
	
randomString[blockSize] = 10;
	
return randomString;
}

	


int generate(char* fileName, int recordSize, int recordAmount){

FILE * file = fopen(fileName, "w+");

if (file == NULL){
	printf("Cant create a file\n");
	return 1;
        }
	
for ( int i = 0; i < recordAmount; i++){
	char* str = generateString(recordSize);
	fwrite(str, sizeof(char), recordSize+1, file);
	free(str);
        }

fclose(file);
return 0;
}

void generate_check(char *fileName, int size, int length){

if (generate(fileName, size, length) != 0 )
	printf("Problem with generating\n");
else printf("Well-generated\n");

}



int lib_sort(char *fileName, int recordSize, int recordAmount){

FILE * file = fopen(fileName, "r+");
if ( file == NULL) {
	printf("Cant open file for sorting\n");
	return 1;
        }
char* rec1 = calloc((recordSize+1),sizeof(char));
char* rec2 = calloc((recordSize+1),sizeof(char));
long int offset = (recordSize+1)*sizeof(char);


for(int i = 0; i < recordAmount-1; i++){
	fseek(file, i*offset,0);
	if(fread(rec1,sizeof(char), (recordSize+1), file) != (recordSize +1)) return 1;
	
	for(int j = i+1;j < recordAmount; j++){
		fseek(file, j*offset,0);
	        if(fread(rec2,sizeof(char), (recordSize+1), file) != (recordSize+1)) return 1;
		
		if(rec2[0]<=rec1[0]){
			fseek(file, i*offset,0);
			if(fwrite(rec2,sizeof(char), (recordSize+1), file)!=(recordSize+1)) return 1;
			fseek(file, j*offset,0);
			if(fwrite(rec1,sizeof(char), recordSize+1, file)!=(recordSize+1)) return 1;
			char *tmp = rec1;
                        rec1 = rec2;
                        rec2 = tmp;
			}

                }
        }

fclose(file);
free(rec1);
free(rec2);
return 0;

}



void lib_sort_check(char *fileName, int size, int length){

if (lib_sort(fileName, size, length) != 0 )
	printf("Problem with sorting");
else printf("Well-sotred\n");


}




int sys_sort(char *fileName, int recordSize, int recordAmount){

int file = open(fileName, O_RDWR);
char* rec1 = calloc((recordSize+1),sizeof(char));
char* rec2 = calloc((recordSize+1),sizeof(char));
long int offset = (recordSize+1)*sizeof(char);


for(int i = 0; i < recordAmount-1; i++){
	lseek(file, i*offset,0);
	if(read(file, rec1, sizeof(char)*(recordSize+1)) != (recordSize +1)) return 1;
	
	for(int j = i+1;j < recordAmount; j++){
		lseek(file, j*offset,0);
	        if(read(file, rec2,sizeof(char)*(recordSize+1)) != (recordSize+1)) return 1;
		
		if(rec2[0]<=rec1[0]){
			lseek(file, i*offset,0);
			if(write(file, rec2,sizeof(char)*(recordSize+1))!=(recordSize+1)) return 1;
			lseek(file, j*offset,0);
			if(write(file, rec1,sizeof(char)*(recordSize+1))!=(recordSize+1)) return 1;
			char *tmp = rec1;
                        rec1 = rec2;
                        rec2 = tmp;
			}

                }
        }
close(file);
free(rec1);
free(rec2);
return 0;

}



void sys_sort_check(char *fileName, int size, int length){

if (sys_sort(fileName, size, length) != 0 )
	printf("Problem with sorting");
else printf("Well-sorted\n");

}


int lib_copy(char* file1, int recordSize, int recordAmount, char* file2){

char* buf = calloc(recordSize+1, sizeof(char));
long int offset = (recordSize + 1)* sizeof(char);
FILE * outFile = fopen(file1, "r");
if ( outFile == NULL) {
	printf("Cant open outFile for copying\n");
	return 1;
        }

FILE * toFile = fopen(file2,"a");
if ( toFile == NULL) {
	printf("Cant open toFile for copying\n");
	return 1;
        }

for ( int i = 0; i < recordAmount; i++){

	fseek(outFile, i*offset, 0);	
	if ( fread(buf, sizeof(char), recordSize+1, outFile) != recordSize + 1) 
		return 1;
	if(fwrite(buf, sizeof(char), recordSize+1, toFile)!= recordSize +1) return 1;
}


fclose(outFile);
fclose(toFile);
free(buf);
return 0;

}


void lib_copy_check(char* file1, int size, int length, char* file2){

if (lib_copy(file1, size, length, file2) != 0 )
	printf("Problem with copying");
else printf("Well-coped\n");

}


int sys_copy(char* file1, int recordSize, int recordAmount, char* file2){

char* buf = calloc(recordSize+1, sizeof(char));
long int offset = (recordSize + 1)* sizeof(char);
int outFile = open(file1, O_RDONLY);
if ( outFile < 0 ) {
	printf("Cant open outFile for copying\n");
	return 1;
        }

int toFile = open(file2,O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
if ( toFile < 0) {
	printf("Cant open toFile for copying\n");
	return 1;
        }

for ( int i = 0; i < recordAmount; i++){

	lseek(outFile, i*offset, 0);	
	if ( read(outFile, buf, sizeof(char)*recordSize+1) != recordSize + 1) 
		return 1;
	if( write(toFile, buf, sizeof(char)*recordSize+1) != recordSize+1) return 1;
}


close(outFile);
close(toFile);
free(buf);
return 0;

}


void sys_copy_check(char* file1, int size, int length, char* file2){

if (sys_copy(file1, size, length, file2) != 0 )
	printf("Problem with copying\n");
else printf("Well-coped\n");

}



int main(int argc, char** argv){

srand(time(0));
clock_t* startTimeReal = malloc(sizeof(clock_t)); 
clock_t* endTimeReal = malloc(sizeof(clock_t));
struct tms* startTimeProc = malloc(sizeof(struct tms));
struct tms* endTimeProc = malloc(sizeof(struct tms));


if ( argc < 5 ) {
	printf("Few args\n");
	return 1;
}

int recordSize = atoi(argv[3]);
int recordAmount = atoi(argv[4]);

FILE  * file = fopen("raport.txt", "a");
if (file ==NULL) {
printf("File cant be opened\n");
return 1;
}

if(strcmp(argv[1],"generate") == 0)
	generate_check(argv[2],recordSize, recordAmount);

else if(strcmp(argv[1],"sort") == 0){
	
	if(strcmp(argv[5], "lib") == 0) {

		fprintf(file,"\n\nSorting file with recordSize: %d , recordAmount: %d , mode: library \n", recordSize, recordAmount);
        	initTime(startTimeReal, startTimeProc);
		lib_sort_check(argv[2], recordSize, recordAmount);
		printf("\nTime for sorting file in library mode:\n");
		fprintf(file,"\nTime for sorting file in library mode:\n");
		printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);
		}

	else if(strcmp(argv[5], "sys") == 0){

		fprintf(file,"\n\nSorting file with recordSize: %d , recordAmount: %d , mode: system \n", recordSize, recordAmount);
        	initTime(startTimeReal, startTimeProc);
		sys_sort_check(argv[2], recordSize, recordAmount);
		printf("\nTime for sorting file in system mode:\n");
		fprintf(file,"\nTime for sorting file in system mode:\n");
		printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);
		}

	else {printf("Choose system or library mode, pls\n");return 1;}
}

else if( strcmp(argv[1],"copy") == 0){
	if(argc < 7) {printf("There is no second file\n");return 1;}
	if(strcmp(argv[5], "lib") == 0) {

		fprintf(file,"\n\nCoping file in: %s with recordSize: %d , recordAmount: %d , mode: library \n",argv[6],recordSize,recordAmount);
        	initTime(startTimeReal, startTimeProc);
		lib_copy_check(argv[2], recordSize, recordAmount, argv[6]);
		printf("\nTime for coping file in library mode:\n");
		fprintf(file,"\nTime for coping file in library mode:\n");
		printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);
		}

	else if(strcmp(argv[5], "sys") == 0) {
		
		fprintf(file,"\n\nCoping file in: %s with recordSize: %d , recordAmount: %d , mode: system \n",  argv[6], recordSize,recordAmount);
        	initTime(startTimeReal, startTimeProc);		
		sys_copy_check(argv[2], recordSize, recordAmount,argv[6]);
		printf("\nTime for coping file in system mode:\n");
		fprintf(file,"\nTime for coping file in system mode:\n");
		printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);
		}

	else {printf("Choose system or library mode, pls\n");return 1;}
}
else {printf("No right task\n");return 1;}

fclose(file);
return 0;
}
