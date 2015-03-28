CC = gcc
CFLAGS = -fPIC -c
TARGET_LIB1 = libhelpers.so
TARGET_LIB2 = libbufio.so
all: $(TARGET_LIB1) $(TARGET_LIB2)
$(TARGET_LIB1): helpers.c helpers.h
	$(CC) $(CFLAGS) helpers.c
	$(CC) -shared -o $(TARGET_LIB1) helpers.o
	rm -rf helpers.o
$(TARGET_LIB2): bufio.c bufio.h
	$(CC) $(CFLAGS) bufio.c
	$(CC) -shared -o $(TARGET_LIB2) bufio.o
	rm -rf bufio.o
clean:
	rm -rf $(TARGET_LIB1)
	rm -rf $(TARGET_LIB1)

	
