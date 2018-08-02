#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <semaphore.h>

#define ERROR(string) {printf(string);exit(1);}

void read_confi(char*);
void prepare();
void start_threads();
void join_threads();
void *producer(void *fvoid);
void *consumer(void *fvoid);
int check_length(int len);
void sig_handler(int signo);
void ending();

int finished, P, K, N,search_type, print_type, nk, L, c_index = 0, p_index = 0;
char  file_name[FILENAME_MAX] ;
FILE *file;
char **buffer;
sem_t *sems;
pthread_t *k_threads, *p_threads;



int main(int argc, char* argv[]){

	if (argc != 2) ERROR("Wrong amount of args!\n");
	read_confi(argv[1]);
	prepare();
	start_threads();
	join_threads();
	ending();


	return 0;
}

void read_confi(char *file){

	FILE *file_con = fopen(file, "r");
	if (file_con == NULL) ERROR("Error while reading conf file!\n");
	fscanf(file_con, "%d %d %d %s %d %d %d %d", &P, &K, &N, file_name, &L, &search_type, &print_type, &nk);
	printf("Configurations: %d %d %d %s %d %d %d %d\n", P, K, N, file_name, L, search_type, print_type, nk);
	fclose(file_con);

}

void prepare(){

	file = fopen(file_name, "r");
	if (file == NULL) ERROR("Error while opening file for reading\n");
	buffer = calloc(N, sizeof(char*));

	sems = calloc(N+3, sizeof(sem_t));
	for(int i = 0; i < N+2; i++) {
		sem_init (&sems[i], 0, 1);
	}
  sem_init(&sems[N+2], 0, N);

	p_threads = calloc(P, sizeof(pthread_t));
	k_threads = calloc(P, sizeof(pthread_t));

	signal(SIGINT, sig_handler);

}

void start_threads(){

	for (int i = 0; i < P; i++){
		pthread_create(&p_threads[i], NULL, producer, NULL);
	}

	for (int i = 0; i < K; i++){
		pthread_create(&k_threads[i], NULL, consumer, NULL);
	}


}

void join_threads(){

	for (int i = 0; i < P; i++){
		pthread_join(p_threads[i], NULL);
	}
	finished = 1;

	for (int i = 0; i < K; i++){
		pthread_join(k_threads[i], NULL);
	}

}

void *producer(void *fvoid) {

	char line[LINE_MAX];
	int index;

	while(fgets(line, LINE_MAX, file)!=NULL){

		printf("\nproducer %ld takes reading line\n", pthread_self());
		sem_wait(&sems[N]);

    sem_wait(&sems[N+2]);
		index = p_index;
		p_index =(p_index + 1) % N;
		printf("%d\n", p_index);


		sem_wait(&sems[index]);
    sem_post(&sems[N]);
		printf("producer %ld takes buffer[%d]\n", pthread_self(), index);
		buffer[index] =  calloc((strlen(line)+1), sizeof(char));
		strcpy(buffer[index], line);
		printf("Line is copied by producer %ld to buffer[%d]\n", pthread_self(), index);

	  sem_post(&sems[index]);


	}

	printf("The whole text is completed!\n");
	return NULL;
}

void *consumer(void *fvoid){

	char *line;
	int index;

	while(1){

		printf("Consumer %ld takes reading line\n", pthread_self());
		sem_wait(&sems[N+1]);

		while (buffer[c_index] == NULL){
      sem_post(&sems[N+1]);

			if (finished){
				return NULL;
			}
			sem_wait(&sems[N+1]);
		}

		index = c_index;
		c_index =(c_index + 1) % N;

		sem_wait(&sems[index]);
		sem_post(&sems[N+1]);
		line = buffer[index];
		buffer[index] = NULL;
    sem_post(&sems[N+2]);
		sem_post(&sems[index]);


		if (check_length(strlen(line)))
			printf("buffer[%d]:%s\n", index, line);

		free(line);
	}
}

int check_length(int len){

	return search_type == (len > L ? 1: len < 0 ? -1:0);

}

void ending(){

	fclose(file);

	for (int i = 0; i < N; i++)
		free(buffer[i]);
	free(buffer);

	for (int i = 0; i < N+3; i++)
		sem_destroy(&sems[i]);
	free(sems);



}

void sig_handler(int signo){

	for (int i = 0; i < P; i++)
		pthread_cancel(p_threads[i]);

	for (int i = 0; i < K; i++)
		pthread_cancel(k_threads[i]);

	ending();
	exit(0);

}
