#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>

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
pthread_mutex_t *mutex;
pthread_t *k_threads, *p_threads;
pthread_cond_t r_cond, w_cond;


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

	mutex = calloc(N+2, sizeof(pthread_mutex_t));
	for(int i = 0; i < N+2; i++) {
		pthread_mutex_init (&mutex[i], NULL);
	}

	p_threads = calloc(P, sizeof(pthread_t));
	k_threads = calloc(P, sizeof(pthread_t));

	pthread_cond_init(&w_cond, NULL);
	pthread_cond_init(&r_cond, NULL);

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
		pthread_mutex_lock(&mutex[N]);

		while (buffer[p_index] != NULL)
			pthread_cond_wait(&r_cond, &mutex[N]);

		index = p_index;
		p_index =(p_index + 1) % N;
		printf("%d\n", p_index);


		pthread_mutex_lock(&mutex[index]);
		printf("producer %ld takes buffer[%d]\n", pthread_self(), index);
		buffer[index] =  calloc((strlen(line)+1), sizeof(char));
		strcpy(buffer[index], line);
		printf("Line is copied by producer %ld to buffer[%d]\n", pthread_self(), index);

		pthread_mutex_unlock(&mutex[N]);
		pthread_cond_broadcast(&w_cond);
		pthread_mutex_unlock(&mutex[index]);


	}

	printf("The whole text is completed!\n");
	return NULL;
}

void *consumer(void *fvoid){

	char *line;
	int index;

	while(1){

		printf("Consumer %ld takes reading line\n", pthread_self());
		pthread_mutex_lock(&mutex[N+1]);

		while (buffer[c_index] == NULL){
			if (finished){
				pthread_mutex_unlock(&mutex[N+1]);
				return NULL;
			}
			pthread_cond_wait(&w_cond, &mutex[N+1]);
		}

		index = c_index;
		c_index =(c_index + 1) % N;

		pthread_mutex_lock(&mutex[index]);
		pthread_mutex_unlock(&mutex[N+1]);
		line = buffer[index];
		buffer[index] = NULL;
		pthread_cond_broadcast(&r_cond);
		pthread_mutex_unlock(&mutex[index]);


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

	for (int i = 0; i < N+2; i++)
		pthread_mutex_destroy(&mutex[i]);
	free(mutex);

	pthread_cond_destroy(&r_cond);
	pthread_cond_destroy(&w_cond);

}

void sig_handler(int signo){

	for (int i = 0; i < P; i++)
		pthread_cancel(p_threads[i]);

	for (int i = 0; i < K; i++)
		pthread_cancel(k_threads[i]);

	ending();
	exit(0);

}
