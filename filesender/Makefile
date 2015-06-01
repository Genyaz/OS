all: filesender
filesender: filesender.c
	clang -std=c99 -Wl,-rpath=$(shell readlink -f ../lib) -o filesender -I ../lib filesender.c -L../lib -lbufio
clean:
	rm -rf filesender
