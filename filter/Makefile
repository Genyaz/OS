LIB_PATH = $(CURDIR)/../lib
all: filter
filter: filter.h filter.c $(LIB_PATH)/libhelpers.so
	gcc filter.h filter.c -I$(LIB_PATH) $(LIB_PATH)/libhelpers.so -o filter
$(LIB_PATH)/libhelpers.so:
	cd $(LIB_PATH) && $(MAKE)
clean:
	rm -rf filter
