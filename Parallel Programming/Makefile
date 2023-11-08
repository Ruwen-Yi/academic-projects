CC=gcc
FLAGS=-O3 -Wall -Wextra -Wpedantic

all:parallel serial

parallel: parallel.c
	$(CC) $(FLAGS) parallel.c -o parallel -l OpenCL

serial: serial.c
	$(CC) $(FLAGS) serial.c -o serial

clean:
	rm -f parallel serial
