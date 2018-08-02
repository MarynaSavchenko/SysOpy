#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include "sockety.h"
#define ERROR(text){printf(text);exit(EXIT_FAILURE);}

void *command_routine(void *);
void handle_connection(int);
void *ask_routine(void *);
void handle_message(int);
void reg_client(char *, int);
void remove_socket(int);
void remove_client(int);
void unreg_client(char *);
void init(char *, char *);
void end();
void handler(int);


int in(void *const a, void *const pbase, size_t total_elems, size_t size, __compar_fn_t cmp) {
    char *base_ptr = (char *) pbase;
    if (total_elems > 0) {
        for (int i = 0; i < total_elems; ++i) {
            if ((*cmp)(a, (void *) (base_ptr + i * size)) == 0) return i;
        }
    }
    return -1;
}

int compare_name(char *name, Client *client){
    return strcmp(name, client->name);
}


int web_socket;
int local_socket;
int epoll;
int *path;

pthread_t ask;
pthread_t command;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
Client clients[MAX_CLIENT];
int client_num = 0;


int main(int argc, char **argv){

  if (argc != 3) ERROR("Wrong amount of args!\n");
  atexit(end);
  init(atgv[1], argv[2]);

  struct epoll_event event;
  while (1) {
    if (epoll_wait(epoll, &event, 1, -1) == -1)
      ERROR("Error while waiting epoll!\n");

    if (event.data.fd < 0)
        handle_connection(-event.data.fd);
    else
        handle_message(event.data.fd);
}

  return 0;
}

void *command_routine(void *varg){

  srand(time(NULL));
  int message_type =  REQUEST;
  oper msg;
  char buf[132];
  while(1){
    printf("Enter message:\n");
    fgets(buf, 132, stdin);
    if (sscanf(buf, "%lf %c %lf", &msg.arg1, &msg.op, &msg.arg2) !=3){
      printf("Wrong message!\n");
      continue;
    }

    if (msg.op!='+' && msg.op!='-' && msg.op!='*' && msg.op!='/'){
      printf("Wrong operator\n");
      continue;
    }
    pthread_mutex_lock(&client_mutex;)
    if (client_num==0){
      printf("No clients\n");
      continue;
    }
    int i = rand() % client_num;
    if (write(clients[i].fd, &message_type, sizeof(int)) != sizeof(int)){
      printf("Error while sending!\n");
      continue;
    }
    if (write(clients[i].fd, &msg, sizeof(oper))!=sizeof(oper)){
      printf("Error while sending!\n");
      continue;
    }
    pthread_mutex_unlock(&client_mutex);

  }
}


void handle_connection(int socket){

  int client = accept(socket, NULL, NULL);
  if (client == -1)
    ERROR("Error while accepting client!\n");

  struct epoll_event event;
  event.events = EPOLLPRI | EPOLlIN;
  event.data.fd = client;

  if (epoll_ctl(epoll, EPOLl_CTL_ADD, client, &event) == -1)
    ERROR("Error  while adding client to epoll!\n");

}

void *ask_routine(void *varg) {

  int message_type = ASK;
  while(1){
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_num; i++){
      if (client[i].state != 0){
        remove_client(i--);
      }else{
        if (write(clients[i].fd, &message_type, sizeof(int))!= 1)
          ERROR("Error while writing!\n");
          clients[i].state++;
      }

    }
    pthread_mutex_unlock(&client_num);

  }
}

void handle_message(int socket){

  int message_type;
  int message_size;

  if (read(socket, &message_type, sizeof(int)) != sizeof(int))
    ERROR("Error while reading!\n");
  if (read(socket, &message_size, sizeof(int)) != sizeof(int))
      ERROR("Error while reading!\n");
  char *client = malloc(sizeof(int));

  switch(message_type){
    case REGISTER:{
      if ((read(socket, client, message_size)) != message_size)
        ERROR("Error while reading client!\n");
      reg_client(client, socket);
      break;
    }
    case UNREGISTER:{
      if ((read(socket, client, message_size)) != message_size)
        ERROR("Error while reading client!\n");
      unreg_client(client);
      break;
    }
    case RESULT:{
      double res;
      if ((read(socket, client, message_size)) != message_size)
        ERROR("Error while reading client!\n");
      if ((read(socket, &res, message_size)) != message_size)
          ERROR("Error while reading result!\n");
      break;
    }
    case ANSWER:{
      if ((read(socket, client, message_size)) != message_size)
        ERROR("Error while reading client!\n");
      pthread_mutex_lock(&client_mutex);
      int i = in(client, clients, (size_t) client_num, sizeof(Client), compare_name);
      if (i>=0) clients[i].state--;
      pthread_mutex_unlock(&client_mutex);
      break;
    }
    default: ERROR("Wrong message_typ!\n");
  }
  free(client);
}

void reg_client(char *client, int socket){

  pthread_mutex_lock(&client_mutex);
  int message_type;

  if (client_num == MAX_CLIENT){
    message_type = ENOUGH;
    if ((write(socket, &message_type, sizeof(int))) != sizeof(int))
      ERROR("Error while writing ENOUGH message!\n");
    remove_socket(socket);
  }else{
    int cl = in(client, clients, (size_t) client_num,sizeof(Client), compare_name );
    if (cl != -1){
      message_type = WRONG_NAME;
      if ((write(socket, &message_type, sizeof(int)) != sizeof(int))
        ERROR("Error while writing WRONG_NAME!\n");
      remove_socket(socket);
      }else{
      clients[client_num].fd = socket;
      clients[client_num].name = malloc(strlen(client)+1);
      clients[client_num].state = 0;
      strcpy(clients[client_num++].name, client);
      message_type = REGISTERED;
      if ((write(socket, &message_type, sizeof(int)))!= sizeof(int))
        ERROR("Error while writing REGISTERED");
    }
  }
  pthread_mutex_unlock(&client_mutex);
}

void remove_socket(int socket){

  if (epoll_ctl(epoll, EPOLL_CTL_DEL, socket, NULL) == -1)
    ERROR("Error while deleting socket!\n");

  if (shutdown(socket, SHUT_RDWR) == -1)
    ERROR("Error while shuting down!\n");

  if (close(socket) == -1)
    ERROR("Error while closing socket!\n");

}

void remove_client(int num){

  remove_socket(clients[num].fd);

  free(clients[num].name);

  client_num--;
  for (int j = num; j<client_num; j++)
    clients[j] = clients[j+1];

}

void unreg_client(char *name){

  pthread_mutex_lock(&client_mutex);
  int i = in(name, clients, (size_t)client_num, sizeof(Client), compare_name);
  if (i>=0)
    remove_client(i);
  pthread_mutex_unlock;

}



void init(char *num, char *path){

  signal(SIGINT, handler);
  int port_num = atoi(num);

  struct sockaddr_in my_address;
  memset(&my_address, 0, sizeof(struct sockaddr_in));

  my_address.sin_family = AF_INET;
  my_address.sin_addr.s_addr = htonl(INADDR_ANY);
  my_address.sin_port = htons(port_num);

  if ((web_socket =  socket(AF_INET, SOCK_STREAM, 0)) < 0 ) ERROR("Error while creating a socket!\n");

  if (bind(web_socket, (const struct sockaddr*) &my_address, sizeof(my_address)))
    ERROR("Error while binding!\n");

  if (listen(web_socket, 64) == -1)
    ERROR("Error while listening!\n");

  struct sockaddr_un local_address;
  local_address.sun_family = AF_UNIX;

  strcopy(local_address.sun_path, path);

  if ((local_socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
      ERROR("Error while creating local_socket!\n");

  if (bind(local_socket, (const struct sockaddr *) &local_address, sizeof(local_address)))
      ERROR("Error while binding local_socket!\n");

  if (listen(local_socket, 64) == -1)
      ERROR("Error while listening!\n");

  struct epoll_event event;
  event.events = EPOLLIN | EPOLLPRI;

  if ((epoll = epoll_create1(0)) == -1)
    ERROR("Error while creating epoll!\n");

  event.data.fd = -web_socket;
  if(epoll_ctl(epoll, EPOLL_CTL_ADD, web_socket, &event) == -1)
    ERROR("Error while adding web_socket to epoll!\n");

  event.data.fd = -local_socket;
  if(epoll_ctl(epoll, EPOLL_CTL_ADD, local_socket, &event) == -1)
    ERROR("Error while adding local_socket to epoll!\n");

  if (pthread_create(&ask, NULL, ask_routine, NULL) != 0)
    ERROR("Error while creating ask_thread!\n");

  if (pthread_create(&command, NULL, command_routine, NULL) != 0)
    ERROR("Error while creating command_thread!\n");

}

void end(){

  pthread_cancel(ask);
  pthread_cancel(command);
  if ((close(web_socket)) == -1)
    ERROR("Error while closing web_socket!\n");
  if ((close(local_socket)) == -1)
    ERROR("Error while closing local_socket!\n");
  if ((unlink(path)) == -1)
    ERROR("Error while unlinking path!\n");
  if ((close(epoll)) == -1)
    ERROR("Error while closing epoll!\n");
}

void handler(int signo){
  exit(1);
}
