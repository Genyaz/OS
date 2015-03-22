LIB_PATH = $(CURDIR)/../lib
all: revwords
revwords: revwords.c $(LIB_PATH)/libhelpers.so
	gcc revwords.c -I$(LIB_PATH) -L$(LIB_PATH) -lhelpers -o revwords
$(LIB_PATH)/libhelpers.so:
	cd $(LIB_PATH) && $(MAKE)
clean:
	rm -rf revwords
