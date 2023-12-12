CC=gcc
LDFLAGS=-O3 -Wall -Wpedantic -Wextra

all: main.c main.h serial.o parallel.o
	$(CC) $(LDFLAGS) main.c serial.o parallel.o -o diagonal -lpthread

serial.o: serial.c serial.h
	$(CC) $(LDFLAGS) -c serial.c

parallel.o: parallel.c parallel.h
	$(CC) $(LDFLAGS) -c parallel.c

clean:
	rm diagonal *.o
