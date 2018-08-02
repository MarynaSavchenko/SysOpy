NAME = sockety

all:
	
	gcc -c $(NAME).c -o $(NAME).o
	ar rcs lib$(NAME).a $(NAME).o
	gcc server.c lib$(NAME).a -o server
	gcc client.c lib$(NAME).a -o client
