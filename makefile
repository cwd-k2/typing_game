CC=gcc

main: main.c
	$(CC) main.c -lncurses -o main
