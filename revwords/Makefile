all: revwords
revwords: revwords.c
	gcc -std=c99 -Wl,-rpath=$(shell readlink -f ../lib) -o revwords -I ../lib revwords.c -L../lib -lhelpers
clean:
	rm -rf revwords
