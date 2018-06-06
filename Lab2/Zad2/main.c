#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ftw.h>
#include <sys/wait.h>


const char format[] = "%Y-%m-%d %H:%M:%S";
char buffer[PATH_MAX];
int accessmodes[] ={S_IRUSR,S_IWUSR,S_IXUSR,S_IRGRP,S_IWGRP,S_IXGRP,S_IROTH,S_IWOTH,S_IXOTH};
char* operand;
time_t date;
char* datestr;

char charik[] = {'r','w','x','r','w','x','r','w','x'};


void printFile(const char* Path,const struct stat* buf){


printf("\n\nDirectory of file: %s\n", Path);
strftime(buffer, PATH_MAX, format, localtime(&buf->st_mtime));
printf("The date of the last modyfikation: %s\n", buffer);
printf("Size of file: %d\n",(int)buf->st_size);
printf("Rights: ");
for ( int i = 0; i<9; i++)
	printf("%c", buf->st_mode & accessmodes[i] ? charik[i]: '-');
}



int searching(char* path, char* operand, time_t date){

char* Path = calloc(PATH_MAX,sizeof(char));
pid_t pid;

DIR * dir = opendir(path);
if(dir == NULL){
	printf("Error while opening a dir\n");
	return 1;
	}

struct dirent* DirEntry; 
struct stat buf;

while(DirEntry=readdir(dir)){

	strcpy(Path,path);
	strcat(Path, "/");
	strcat(Path, DirEntry -> d_name);
	
        if (strcmp(DirEntry->d_name, ".") == 0 || strcmp(DirEntry->d_name, "..") == 0) 
            continue;
	
	lstat(Path, &buf);
	if(S_ISREG(buf.st_mode)){
	if (strcmp(operand,"<")==0 && difftime(date,buf.st_mtime)>0) printFile(Path,&buf);
	else if (strcmp(operand,">")==0 && difftime(date,buf.st_mtime)<0) printFile(Path,&buf);
	else if(strcmp(operand,"=")==0 && difftime(buf.st_mtime,date)==3600) printFile(Path,&buf);
	
        }
	else if(S_ISDIR(buf.st_mode)) {
		pid = fork();
		if(pid<0) {
		printf("Error creating new proces:(\n");
		return 1;
		}
		else if (pid == 0){
                searching(Path, operand,date);
		exit(0);
		}
		else wait(NULL);
    		
		}
		     
		



}


closedir(dir);
return 0;
}


int foonftw(const char *fpath, const struct stat *buf,int typeflag, struct FTW *ftwbuf){

 if (typeflag != FTW_F) {
        return 0;
    }


int timing = difftime(date,buf->st_mtime);
if (!((strcmp(operand,"<")==0 && timing>0) || (strcmp(operand,">")==0 && timing<0) ||(strcmp(operand,"=")==0 && timing==3600)))
	return 0;
else {
	printFile(fpath,buf);
	return 0;
	}



}




void check_searching(char* path, char* operand, time_t date){

if (searching(path,operand, date) != 0)
	printf("\nError while searching\n");
else 
	printf("\nSuper!Works\n");

}



int main(int argc, char** argv){

if (argc < 4 ){
	printf("Few args\n");
	return 0;
	}


char* path = argv[1];
operand = argv[2];
datestr = argv[3];
struct tm tm;


if (strptime(datestr, format, &tm) == NULL) return 0;
date = mktime(&tm);
check_searching(path,operand,date);
printf("\n\n\n\nWith nftw: \n");
nftw(path,foonftw,10, FTW_PHYS);


return 0;
}
