#include "helpers.h"

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
