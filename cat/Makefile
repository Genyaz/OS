LIB_PATH = $(CURDIR)/../lib
all: cat
cat: cat.c $(LIB_PATH)/libhelpers.so
	gcc cat.c -I$(LIB_PATH) $(LIB_PATH)/libhelpers.so -o cat
$(LIB_PATH)/libhelpers.so:
	cd $(LIB_PATH) && $(MAKE)
clean:
	rm -rf cat
