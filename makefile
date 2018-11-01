all: dirread.o
	gcc dirread.o

dirread.o: dirread.c
	gcc -c dirread.c

run:
	./a.out

clear:
	rm *.o

