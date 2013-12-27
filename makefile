all: ls2
	
ls2: ls2.o
	g++ -o ls2 ls2.o
ls2.o: ls2.c
	g++ -c ls2.c
clean:
	rm -f ls2.o ls2
