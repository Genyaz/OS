LIB_PATH = $(CURDIR)/../lib
all: bufcat
bufcat: bufcat.c $(LIB_PATH)/libbufio.so
	gcc bufcat.c -I$(LIB_PATH) $(LIB_PATH)/libbufio.so -o bufcat
$(LIB_PATH)/libbufio.so:
	cd $(LIB_PATH) && $(MAKE) libbufio.so
clean:
	rm -rf bufcat
