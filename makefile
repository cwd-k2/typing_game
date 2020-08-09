CC=clang

main: main.c
	$(CC) main.c -std=gnu18 -lncurses -o main
