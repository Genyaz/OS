all: simplesh
simplesh: simplesh.c
	gcc -std=c99 -Wl,-rpath=$(shell readlink -f ../lib) -o simplesh -I ../lib simplesh.c -L../lib -lhelpers
clean:
	rm -rf simplesh
