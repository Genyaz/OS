#ifndef BUFIO_H
#define BUFIO_H

#include <unistd.h>

typedef struct buf_t
{
	char* b;
	size_t capacity;
	size_t size;
} buf_t;

typedef int fd_t;

buf_t * buf_new(size_t capacity);
void buf_free(buf_t *);
size_t buf_capacity(buf_t *);
size_t buf_size(buf_t *);
ssize_t buf_fill(fd_t fd, buf_t * buf, size_t required);
ssize_t buf_flush(fd_t fd, buf_t *buf, size_t required);

#endif
