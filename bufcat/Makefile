all: bufcat.c
	gcc -std=c99 -Wl,-rpath=$(shell readlink -f ../lib) -o bufcat -I ../lib bufcat.c -L../lib -lbufio
clean:
	rm -rf bufcat
