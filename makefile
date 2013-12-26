all : ls

ls: ls.o
	gcc -o ls ls.o
ls.o:ls.c
	gcc -c -std=c99 ls.c
