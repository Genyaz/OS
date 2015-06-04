all: forking polling
forking: forking.c
	clang -std=c99 -Wl,-rpath=$(shell readlink -f ../lib) -o forking -I ../lib forking.c -L../lib -lbufio
polling: polling.c
	clang -std=c99 -Wl,-rpath=$(shell readlink -f ../lib) -o polling -I ../lib polling.c -L../lib -lbufio
clean:
	rm -rf forking
	rm -rf polling
