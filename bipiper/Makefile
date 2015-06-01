all: forking
forking: forking.c
	clang -std=c99 -Wl,-rpath=$(shell readlink -f ../lib) -o forking -I ../lib forking.c -L../lib -lbufio
clean:
	rm -rf forking
