all: cat
cat: cat.c
	gcc -std=c99 -Wl,-rpath=$(shell readlink -f ../lib) -o cat -I ../lib cat.c -L../lib -lhelpers
clean:
	rm -rf cat
