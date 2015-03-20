LIB_PATH = $(CURDIR)/../lib
all: revwords
revwords: revwords.c $(LIB_PATH)/helpers.so
	gcc revwords.c -I$(LIB_PATH) $(LIB_PATH)/helpers.so -o revwords
$(LIB_PATH)/helpers.so:
	cd $(LIB_PATH) && $(MAKE)
clean:
	rm -rf revwords
