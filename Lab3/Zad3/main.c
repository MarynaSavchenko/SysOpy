#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>


int setlimits(char* time, char* size){



struct rlimit rlimit_time;
rlimit_time.rlim_cur=atoi(time);
rlimit_time.rlim_max=atoi(time);

if (setrlimit(RLIMIT_CPU, &rlimit_time) != 0){
	printf("Error while setting time limit:(\n");
	return 1;
	}

struct rlimit rlimit_size;
rlimit_size.rlim_cur=atoi(size)*1024*1024;
rlimit_size.rlim_max=atoi(size)*1024*1024;

if (setrlimit(RLIMIT_AS, &rlimit_size) != 0){
	printf("Error while setting size limit:(\n");
	return 1;
	}





return 0;
}


void check_setlimits(char* time, char* size){

if (setlimits(time,size) == 0) printf("Everything works:)\n");
else printf("Error while setting limits:(\n");

}


int main(int argc, char** argv){
	
	int max_number_args = 64;
	char* token[max_number_args];
	char line[256];
	struct rlimit rlimit_time;

	if (argc != 4) {
		printf("Wrong amount of args:(\n");
		return 1;
	}

	FILE * file = fopen(argv[1], "r");
	if ( file == NULL) {
		printf("Error while opening the file:(\n");
		return 1;
	}
	struct rusage old_usage;
	getrusage(RUSAGE_CHILDREN, &old_usage);

	while (fgets(line, sizeof(line),file)){
		
		int number = 0;
		int child_status;
		
		while( (token[number] = strtok(number == 0 ? line :NULL," " )) != NULL)
			number++;
		token[number]='\0';
		for(int i = number-1; i < max_number_args; i++)
			token[i]=NULL;

		
                
		if ( number > max_number_args) {
			printf("Too much args:(\n");
			return 1;
		}
		//printf("%d\n", number);
		for( int i  =0 ; i <= number; i++)
			printf("number: %d,%s\n", i,token[i]);



		pid_t pid = fork();
		if (pid<0) {
			printf("Error creating a new proces:(\n");
			return 1;
		}
		else if (pid == 0) {
			


			check_setlimits(argv[2], argv[3]);
			execvp(token[0], token);
			printf("Error while exec\n");
			exit(1);
			}
		
			
		else
			wait(NULL);       	
    		 	
		struct rusage usage;
		getrusage(RUSAGE_CHILDREN, &usage);
		struct timeval utime;
		struct timeval stime;
		timersub(&usage.ru_utime, &old_usage.ru_utime, &utime);
		timersub(&usage.ru_stime, &old_usage.ru_stime, &stime);
		old_usage = usage;
		printf("User CPU time used: %d.%d\nSystem CPU time used: %d.%d\n\n",(int)utime.tv_sec, (int)utime.tv_usec,(int)stime.tv_sec,(int)stime.tv_usec);


			
} 	



fclose(file);
return 0;
}
