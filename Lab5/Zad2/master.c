#include <sys/types.h> 
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ERROR(text){printf(text);exit(EXIT_FAILURE);}
#define maxline 100
int main(int argc, char** argv){

	if (argc != 2) ERROR("Wrong amount of args :( \n");
	if (mkfifo(argv[1], S_IWUSR|S_IRUSR) == -1)ERROR("Error while making fifo :( \n");
	FILE* pipe = fopen(argv[1],"r");
	if (!pipe) ERROR("Error while opening pipe :( \n");
	char buff[maxline]; 
	printf("djsd\n");
	while(fgets(buff, maxline,pipe) != 0){
		write(1, buff, strlen(buff));
	}
	printf("Master finished reading \n");
	fclose(pipe);
	if (remove(argv[1])) ERROR("Error deleting pipe \n");
	return 0;
}
