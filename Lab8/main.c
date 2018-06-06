#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <sys/times.h>

#define ERROR(error) {printf(error);exit(1);}

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })


int threads, W, H, C, **I,**J;
float **K;
void save_picture(char *path);
void *routine(void *args);
void read_filter(char *path);
void read_picture(char *path);
void initTime(clock_t* startTimeReal, struct tms* startTimeProc);
void printTime(clock_t* startTimeReal, struct tms* startTimeProc, clock_t* endTimeReal, struct tms* endTimeProc, FILE* file);



int main(int argc, char *argv[]){

	if (argc != 5) ERROR("Wrong amount of args\n");
	threads = atoi(argv[1]);
	char *in_picture = argv[2];	
	char *filter = argv[3];
	char *out_picture = argv[4];
	clock_t* startTimeReal = malloc(sizeof(clock_t)); 
	clock_t* endTimeReal = malloc(sizeof(clock_t));
	struct tms* startTimeProc = malloc(sizeof(struct tms));
	struct tms* endTimeProc = malloc(sizeof(struct tms));

	FILE  * file = fopen("raport.txt", "a");
	read_picture(in_picture);
	read_filter(filter);
	
	J = malloc(H*sizeof(int*));
	for (int i = 0; i < H; i++)
		J[i] = malloc(W*sizeof(int));

	pthread_t *T = malloc(threads*sizeof(pthread_t));
	initTime(startTimeReal, startTimeProc);
	for (int i = 0; i < threads; i++){
		int *arg = malloc(sizeof(int));
		*arg = i;
		pthread_create(T + i, NULL, routine, arg);
	}
	for (int i =0; i < threads; i++){
		void *x;
		pthread_join(T[i], &x);
	}
	printTime(startTimeReal, startTimeProc,endTimeReal,endTimeProc, file);
	save_picture(out_picture);
	fclose(file);
	return 0;
}

void printTime(clock_t* startTimeReal, struct tms* startTimeProc, clock_t* endTimeReal, struct tms* endTimeProc, FILE* file){
	
*endTimeReal = clock();
times(endTimeProc);
fprintf(file, "\n\nNumber of threads: %d\nPicture size: %d*%d\nFilter size: %d\n", threads,W,H,C);


double realTime = (double)(*endTimeReal - *startTimeReal)/CLOCKS_PER_SEC;
double userTime = (double)(endTimeProc -> tms_utime - startTimeProc -> tms_utime)/CLOCKS_PER_SEC;
double procTime = (double)(endTimeProc -> tms_stime - startTimeProc -> tms_stime)/CLOCKS_PER_SEC;

printf("realTime : %lf\n", realTime);
printf("userTime : %lf\n", userTime);
printf("procTime : %lf\n", procTime);

fprintf(file, "realTime : %f\n", realTime);
fprintf(file, "userTime : %f\n", userTime);
fprintf(file, "procTime : %f\n", procTime);

}

void initTime(clock_t* startTimeReal, struct tms* startTimeProc){
	
*startTimeReal = clock();  // Zwraca liczbę cykli, które upłynęły od chwili uruchomienia programu.
times(startTimeProc); //The times function returns information about a process’ consumption of processor time in a struct tms object, in addition to the process’ CPU time. 

}

void save_picture(char *path){

	FILE *file = fopen(path, "w");
	if (file == NULL) ERROR("Error while opening out_file\n");
	fprintf(file, "P2\n%d %d\n%d\n", W, H, 255);
	for (int i = 0; i < H; i++){
		for (int j = 0; j < W; j++)
			fprintf(file,"%d ", J[i][j]);
		fprintf(file,"\n");
	}
	fclose(file);
}

void *routine(void *args){

	int thread = *(int*)args;
	int start = thread * W / threads;
	int finish = (thread + 1) * W / threads;
	int c = (int) ceil((float)C/2.0);
	double s;

	for (int w = start; w < finish; w++){
		for (int h = 0; h < H; h++){
			s = 0;
			for (int i = 0; i < C; i++){
				for (int j = 0; j < C; j++){
					int res_h = min((H-1), max(0, h - c + j));
					int res_w = min((W-1),max(0, w - c + i));
					s += K[i][j] * I[res_h][res_w];
				}
			}
			J[h][w] = (int) round(s);
		}
	}

}


void read_picture(char *path){

	FILE *file;
	if ((file = fopen(path, "r")) == NULL) ERROR("Error while opening in_picture\n");
	if (fscanf(file,"%*s %d %d %*d",&W,&H) <= 0) ERROR("Error while scaning from in_picture\n");
	I = malloc(H*sizeof(int*));
	for(int i = 0; i < H; i++)
		I[i] = malloc(W * sizeof(int));
	for (int i = 0; i < H; i++)
		for (int j = 0; j < W; j++)
			if (fscanf(file,"%d",&I[i][j]) <= 0)ERROR("Error while scaning from in_picture\n") ;
	fclose(file);

}


void read_filter(char *path){

	FILE *file;
	if ((file = fopen(path, "r")) == NULL) ERROR("Error while opening filter\n");
	if (fscanf(file, "%d",&C) <= 0) ERROR("Error while reading from filter\n");
	K = malloc( C * sizeof(float*));
	for (int i = 0; i < C; i++)
		K[i] = malloc(C * sizeof(float));
	for (int i = 0; i < C; i++)
		for (int j = 0; j < C; j++)
			if (fscanf(file, "%f", &K[i][j]) <= 0) ERROR("Error while reading from filter\n");
	fclose(file);

}

