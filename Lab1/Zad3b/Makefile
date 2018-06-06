NAME = blockArray

all: O O0 O1 O2 O3 Os clean

O: 
	make static LVL=-O
	make shared LVL=-O
	make dynamic LVL=-O
	make testy LVL=-O
O0: 
	make static LVL=-O0
	make shared LVL=-O0
	make dynamic LVL=-O0
	make testy LVL=-O0
O1: 
	make static LVL=-O1
	make shared LVL=-O1
	make dynamic LVL=-O1
	make testy LVL=-O1
O2: 
	make static LVL=-O2
	make shared LVL=-O2
	make dynamic LVL=-O2
	make testy LVL=-O2
O3: 
	make static LVL=-O3
	make shared LVL=-O3
	make dynamic LVL=-O3
	make testy LVL=-O3
Os: 
	make static LVL=-Os
	make shared LVL=-Os
	make dynamic LVL=-Os
	make testy LVL=-Os

static:
	
	gcc $(LVL) -c $(NAME).c -o $(NAME).o 
	ar rcs lib$(NAME).a $(NAME).o
	gcc $(LVL) main.c lib$(NAME).a -o staticMain$(LVL)
	


shared:
	
	gcc $(LVL) -c -Wall -fPIC $(NAME).c -o $(NAME).o 
	gcc $(LVL) -shared -fPIC $(NAME).o -o lib$(NAME).so 
	gcc $(LVL) main.c -L. -Wl,-rpath=. -l$(NAME) -o sharedMain$(LVL)
	

dynamic: 
	
	gcc $(LVL) -c -Wall -fPIC $(NAME).c -o $(NAME).o 
	gcc $(LVL) -shared -fPIC $(NAME).o -o lib$(NAME).so 
	gcc $(LVL) main.c -L. -Wl,-rpath=. -l$(NAME) -o dynamicMain$(LVL) -ldl 
		

			
clean:

	rm -f *.o *.a *.so 

testy:
	echo "\n\nDYNAMIC, level : $(LVL)\n" >> raport2.txt
	./dynamicMain$(LVL) 100000 50 dynamic 400 100 1000 
	./dynamicMain$(LVL) 10000 50 dynamic 400 100 1000
	./dynamicMain$(LVL) 1000 50 dynamic 400 100 1000
	./dynamicMain$(LVL) 100 50 dynamic 400 100 1000
	
	echo "\n\nSTATIC, level : $(LVL)\n" >> raport2.txt
	./staticMain$(LVL) 100000 50 dynamic 400 100 1000
	./staticMain$(LVL) 10000 50 dynamic 400 100 1000
	./staticMain$(LVL) 1000 50 dynamic 400 100 1000
	./staticMain$(LVL) 100 50 dynamic 400 100 1000
	
	echo "\n\nSHARED, level : $(LVL)\n" >> raport2.txt
	./sharedMain$(LVL) 100000 50 dynamic 400 100 1000
	./sharedMain$(LVL) 10000 50 dynamic 400 100 1000
	./sharedMain$(LVL) 1000 50 dynamic 400 100 1000
	./sharedMain$(LVL) 100 50 dynamic 400 100 1000
	
	
	
