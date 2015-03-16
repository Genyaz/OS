#include <helpers.h>
#include <stdlib.h>
#define BUFFER_SIZE 4096

char buf[BUFFER_SIZE], prev[BUFFER_SIZE], to_write[BUFFER_SIZE + 1];
size_t prev_unwritten_start = BUFFER_SIZE;
size_t prev_unwritten_end = BUFFER_SIZE;
size_t buf_start = BUFFER_SIZE;
size_t buf_end = BUFFER_SIZE;
size_t to_write_size = 0;
int written = 0;

void write_reversed(int end)
{
    if (written)
    {
        to_write_size = 1;
        to_write[0] = ' ';
    }
    else
    {
        to_write_size = 0;
        written = 1;
    }
    while (buf_start < end)
    {
        to_write[to_write_size++] = buf[--end]; 
    }
    while (prev_unwritten_start < prev_unwritten_end)
    {
        to_write[to_write_size++] = prev[--prev_unwritten_end];
    }
    write_(STDOUT_FILENO, to_write, to_write_size);
}     

int main()
{
    while (1)
    {
        buf_start = 0;
        buf_end = read_until(STDIN_FILENO, buf, BUFFER_SIZE, ' ');
        if (buf_end == -1)
        {
            exit(EXIT_FAILURE);
        }
        else if (buf_end == 0)
        {
            if (prev_unwritten_start < prev_unwritten_end)
            {
                write_reversed(0);
            }
            exit(EXIT_SUCCESS);
        }
        else
        {
            int i = 0;
            while (i < buf_end)
            {
                if (buf[i] == ' ')
                {
                    write_reversed(i);
                    buf_start = i + 1;                    
                }
                i++;
            }
            prev_unwritten_start = buf_start;
            prev_unwritten_end = buf_end;
            i = buf_start;
            while (i < buf_end)
            {
                prev[i] = buf[i];
                i++;
            }
        }
    }   
}
