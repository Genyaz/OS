LIB_PATH = $(CURDIR)/../lib
all: cat
cat: cat.c $(LIB_PATH)/helpers.so
	gcc cat.c -I$(LIB_PATH) $(LIB_PATH)/helpers.so -o cat
$(LIB_PATH)/helpers.so:
	cd $(LIB_PATH) && $(MAKE)
clean:
	rm -rf cat
