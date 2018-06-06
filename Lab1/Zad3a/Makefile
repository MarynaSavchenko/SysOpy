NAME = blockArray

all: static shared dynamic test clean

static:
	
	gcc -c $(NAME).c -o $(NAME).o
	ar rcs lib$(NAME).a $(NAME).o
	gcc main.c lib$(NAME).a -o staticMain
	


shared:
	
	gcc -c -Wall -fPIC $(NAME).c -o $(NAME).o
	gcc -shared -fPIC $(NAME).o -o lib$(NAME).so 
	gcc main.c -L. -Wl,-rpath=. -l$(NAME) -o sharedMain
	

dynamic: 
	
	gcc -c -Wall -fPIC $(NAME).c -o $(NAME).o
	gcc -shared -fPIC $(NAME).o -o lib$(NAME).so 
	gcc main.c -L. -Wl,-rpath=. -l$(NAME) -o dynamicMain -ldl 
		

			
clean:

	rm -f *.o *.a *.so 

test:
	echo "\n\nDYNAMIC:\n" > raport2.txt
	./dynamicMain 100000 50 dynamic 400 100 1000 
	./dynamicMain 10000 50 dynamic 400 100 1000
	./dynamicMain 1000 50 dynamic 400 100 1000
	./dynamicMain 100 50 dynamic 400 100 1000
	
	echo "\n\nSTATIC:\n" >> raport2.txt
	./staticMain 100000 50 dynamic 400 100 1000
	./staticMain 10000 50 dynamic 400 100 1000
	./staticMain 1000 50 dynamic 400 100 1000
	./staticMain 100 50 dynamic 400 100 1000
	
	echo "\n\nSHARED:\n" >> raport2.txt
	./sharedMain 100000 50 dynamic 400 100 1000
	./sharedMain 10000 50 dynamic 400 100 1000
	./sharedMain 1000 50 dynamic 400 100 1000
	./sharedMain 100 50 dynamic 400 100 1000
	
	
	
