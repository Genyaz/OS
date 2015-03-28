#include "bufio.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct buf_t buf_t;
typedef int fd_t;

void assert(int to_check)
{
#ifdef DEBUG
	if (!to_check) abort();
#endif
}

buf_t* buf_new(size_t capacity)
{
	buf_t* res = malloc(sizeof(buf_t));
	if (res == NULL) return NULL;
	res->b = malloc(sizeof(char) * capacity);
	if (res->b == NULL) return NULL;
	res->size = 0;
	res->capacity = capacity;
	return res;
}

void buf_free(buf_t* buf)
{
	assert(buf != NULL);
	free(buf->b);
	free(buf);
}

size_t buf_capacity(buf_t* buf)
{
	assert(buf != NULL);
	return buf->capacity;
}

size_t buf_size(buf_t* buf)
{
	assert(buf != NULL);
	return buf->size;
}

ssize_t buf_fill(fd_t fd, buf_t* buf, size_t required)
{
	assert(buf != NULL);
	assert(required <= buf->capacity);
	while (buf->size < required)
	{
		ssize_t read_now = read(fd, buf->b + buf->size, buf->capacity - buf->size);
		if (read_now == 0) return buf->size;			
		if (read_now == -1)
		{
			if (errno != EINTR) return -1;
			read_now = 0;
		}
		buf->size += read_now;
	}
	return buf->size;
}

ssize_t buf_flush(fd_t fd, buf_t* buf, size_t required)
{
	assert(buf != NULL);
	ssize_t written = 0;
	if (buf->size < required) required = buf->size;
	while (written < required)
	{
		ssize_t written_now = write(fd, buf->b + written, buf->size - written);
		if (written_now == -1)
		{
			if (errno != EINTR)
			{
				buf->size -= written;
				memmove(buf->b, buf->b + written, buf->size);
				return -1;
			}
			written_now = 0;
		}
		written += written_now;
	}
	buf->size -= written;
	memmove(buf->b, buf->b + written, buf->size);
	return written;
}
