all: revwords
revwords: revwords.c ../lib/helpers.so
	gcc revwords.c -I../lib ../lib/helpers.so -o revwords
../lib/helpers.so:
	cd ../lib && $(MAKE)
clean:
	rm -rf revwords
