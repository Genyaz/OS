all: cat
cat: cat.c ../lib/helpers.so
	gcc cat.c -I../lib ../lib/helpers.so -o cat
../lib/helpers.so:
	cd ../lib && $(MAKE)
clean:
	rm -rf cat
