#include "helpers.h"
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>

ssize_t read_(int fd, void* buf, size_t count) 
{
    size_t already_read = 0;
    ssize_t read_now = 0;
    while (already_read < count)
    {
        if ((read_now = read(fd, buf + already_read, count - already_read)) == -1) 
        {
            return -1;
        }
        else if (read_now == 0)
        {
            return already_read;
        }
        already_read += read_now;
    }
    return count;
}

ssize_t write_(int fd, void* buf, size_t count)
{
    size_t written = 0;
    ssize_t written_now = 0;
    while (written < count)
    {        
        if ((written_now = write(fd, buf + written, count - written)) == -1)
        {
            return -1;
        }
        written += written_now;
    }
    return count;
}

ssize_t read_until(int fd, void * buf, size_t count, char delimiter)
{
    size_t already_read = 0;
    ssize_t read_now = 0;
    char* ch_buf = (char*)buf;
    while (already_read < count)
    {
        if ((read_now = read(fd, buf + already_read, count - already_read)) == -1)
        {
            return -1;
        }
        else if (read_now == 0)
        {
            return already_read;
        }
        else
        {
            int delimiter_in_read = 0;
            for (; read_now > 0; read_now--)
            {
                delimiter_in_read |= ch_buf[already_read] == delimiter;
                already_read++;
            }
            if (delimiter_in_read)
            {
                return already_read;
            }
        }
    }
    return count;
}

int spawn(const char* file, char * const argv[])
{
    int pid;
    while ((pid = fork()) == -1)
    {
        if (errno != EAGAIN)
        {
            exit(EXIT_FAILURE);
        }
    }
    if (pid != 0)
    {
        int status;
        while (wait(&status) != pid)
        {
            if (errno != EINTR)
            {
                exit(EXIT_FAILURE);
            }
        }
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        execvp(file, argv);
        exit(EXIT_FAILURE);
    }      
}
