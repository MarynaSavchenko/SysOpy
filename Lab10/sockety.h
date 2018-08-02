#ifndef sockety
#define sockety

#define MAX_CLIENT 30

typedef struct oper{
  char operator;
  double arg1;
  double arg2;
}oper;

typedef struct Client{
  int fd;
  char *name;
  int state;
}Client;

typedef enum message_type{
  ENOUGH,
  WRONG_NAME,
  REGISTER,
  UNREGISTER,
  REGISTERED,
  RESULT,
  REQUEST,
  ASK,
  ANSWER,
}message_type;

typedef enum connect_type{
  WEB,
  LOCAL,
}connect_type;

#endif
