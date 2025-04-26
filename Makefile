all: main

main: main.c transaction.c semaphore.c
	gcc -o main main.c transaction.c semaphore.c -lrt

clean:
	rm -f main