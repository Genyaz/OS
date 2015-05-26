all: filter
filter: filter.c
	gcc -std=c99 -Wl,-rpath=$(shell readlink -f ../lib) -o filter -I ../lib filter.c -L../lib -lhelpers
clean:
	rm -rf filter
