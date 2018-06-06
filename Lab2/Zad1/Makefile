all: 4 512 4096 8192

4:
	make testy SIZE=4 AMOUNT=1500

512:
	make testy SIZE=512 AMOUNT=1500

4096:
	make testy SIZE=4096 AMOUNT=1000

8192:
	make testy SIZE=8192 AMOUNT=1000
	


testy:

	./main generate original$(SIZE) $(SIZE) $(AMOUNT) 
	./main copy original$(SIZE) $(SIZE) $(AMOUNT) lib lib$(SIZE)
	./main copy original$(SIZE) $(SIZE) $(AMOUNT) sys sys$(SIZE)
	./main sort lib$(SIZE) $(SIZE) $(AMOUNT) lib 
	./main sort sys$(SIZE) $(SIZE) $(AMOUNT) sys 
