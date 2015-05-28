#ifndef HELPERS_H
#define HELPERS_H

#include <unistd.h>
extern ssize_t read_(int fd, void* buf, size_t count);
extern ssize_t write_(int fd, void* buf, size_t count);
extern ssize_t read_until(int fd, void * buf, size_t count, char delimiter);
extern int spawn(const char* file, char * const argv[]);

typedef struct execargs_t
{
	char* file;
	char** args;
} execargs_t;

execargs_t * execargs_new(char* file, char* args[]);
int exec(execargs_t * args);
int runpiped(execargs_t ** programs, size_t n);
void hdl();

#endif
