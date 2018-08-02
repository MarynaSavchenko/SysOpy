#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include "sockety.h"
#define ERROR(text){printf(text);exit(EXIT_FAILURE);}

void get_message();
void handle_request();
void init(char*, char*, char*);
void handler(int);
void end();
void register_server();
void send_message(int);


int SOCKET;
char *name;


int main(int argc, char const *argv[]) {


  if (argc!=4) ERROR("Wrong amount of args!\n");
  atexit(end);
  strcpy(name, argv[1]);
  init(name, argv[2], argv[3]);
  register_server();
  get_message();

  return 0;
}

void get_message(){
  int message_type;
  while(1){
    if ((read(SOCKET, &message_type, sizeof(int)))!= sizeof(int))
      ERROR("Error while reading message!\n");
    switch(message_type){
      case REQUEST:
        handle_request();
        break;
      case ASK:
        send_message(ANSWER);
        break;
      default:
      break;
    }
  }
}

void handle_request(){

  oper msg;
  double result;
  char buf[132];

  if ((read(SOCKET, &msg, sizeof(oper)))!= sizeof(oper))
    ERROR("Error while sending!\n");

    sprintf(buf, "%lf %c %lf | bc", msg.arg1, msg.operator, msg.arg2);
    FILE *file = popen(buf, "r");
    fread(buf, 1, 132, file);
    pclose(file);
    sscanf(buf, "%lf", &result);

  send_message(RESULT);
  if ((write(SOCKET, &result, sizeof(double))!= sizeof(double))
   ERROR("Error while writing!\n");

}

void init(char* name, char* type, char* adr){

  signal(SIGINT, handler);

switch((strcmp(type, "WEB") == 0 ? WEB : strcmp(type, "LOCAL") == 0 ? LOCAL : -1)){
    case WEB:{

      strtok(adr, ":");
      int port_num = atoi(strtok(NULL, ":"));
      uint32_t ip = inet_addr(adr);
      if (ip == -1) ERROR("Wrong ip!\n");


      struct sockaddr_in my_address;
      memset(&my_address, 0, sizeof(struct sockaddr_in));

      my_address.sin_family = AF_INET;
      my_address.sin_addr.s_addr = htoln(ip);
      my_address.sin_port = htons(port_num);

      if ((SOCKET = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        ERROR("Error while creating socket!\n");

      if ((connect(SOCKET, (const struct sockaddr *) &my_address, sizeof(my_address))) < 0)
        ERROR("Error while connecting to socket!\n");

      break;
    }
    case LOCAL:{

      struct sockaddr_un my_address;


      my_address.sun_family = AF_UNIX;
      strcpy(my_address.sun_family, adr);

      if ((SOCKET = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        ERROR("Error while creating socket!\n");

      if ((connect(SOCKET, (const struct sockaddr *) &my_address, sizeof(my_address))) < 0)
        ERROR("Error while connecting to socket!\n");

      break;
    }
    default:
      ERROR("Wrong type. Only WEB or LOCAL are accepted\n");
      break;

  }
}

void handler(int signo){
  exit(1);
}

void end(){

  if (shutdown(SOCKET, SHUT_RDWR) == -1)
    ERROR("Error while shuting down SOCKET!\n");

  if (close(SOCKET) == -1)
    ERROR("Error while closing SOCKET!\n");
}

void register_server(){

  send_message(REGISTER);

  int message_type;
  if (read(SOCKET, &message_type, sizeof(int)) != 1)
    ERROR("Error while reading from SOCKET!\n");

  switch(message_type){
    case ENOUGH:
      ERROR("Too many clients already!\n");
    case WRONG_NAME:
      ERROR("This name exists already!\n");
    case REGISTERED:{
      printf("Registered!\n");
      break;
    }
    default:
    ERROR("Wrong message_type while registering!\n");
  }

}

void send_message(int message_type){

  int message_size = strlen(name) + 1;
  if ((write(SOCKET, &message_type, sizeof(int))) != sizeof(int))
    ERROR("Error while sending message!\n");
  if ((write(SOCKET, &message_size, sizeof(int))) != sizeof(int))
    ERROR("Error while sending message!\n");
  if ((write(SOCKET, name, message_size)) != sizeof(int))
    ERROR("Error while sending message!\n");

}
