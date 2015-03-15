CC = gcc
CFLAGS = -fPIC -c
TARGET_LIB = helpers.so
all: $(TARGET_LIB)
$(TARGET_LIB): helpers.c helpers.h
	$(CC) $(CFLAGS) helpers.c
	$(CC) -shared -o $(TARGET_LIB) helpers.o
	rm -rf helpers.o
clean:
	rm -rf helpers.so

	
