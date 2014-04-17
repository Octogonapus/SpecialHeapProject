compiler= gcc
flags= -Wall -g 

all: test

test: main.o softheap.o
	$(compiler) $(flags) main.o softheap.o -o test -lm

main.o: main.c
	$(compiler) $(flags) -c main.c

softheap.o: softheap.c
	$(compiler) $(flags) -c softheap.c

clean:
	rm -fr *.o *.gch *~ test a.out
