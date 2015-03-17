#include <helpers.h>
#include <stdlib.h>
#define BUFFER_SIZE 4096

char buf[BUFFER_SIZE], prev[BUFFER_SIZE], to_write[BUFFER_SIZE + 1];
size_t prev_unwritten_end = 0;
size_t buf_start = 0;
size_t buf_end = 0;
size_t to_write_size = 0;

void write_reversed(int end, int space_after)
{
    to_write_size = 0;
    while (buf_start < end)
    {
        to_write[to_write_size++] = buf[--end]; 
    }
    while (prev_unwritten_end > 0)
    {
        to_write[to_write_size++] = prev[--prev_unwritten_end];
    }
    if (space_after)
    {
        to_write[to_write_size++] = ' ';
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
            if (prev_unwritten_end > 0)
            {
                write_reversed(0, 0);
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
                    write_reversed(i, 1);
                    buf_start = i + 1;                    
                }
                i++;
            }            
            i = buf_start;
            while (i < buf_end)
            {
                prev[prev_unwritten_end++] = buf[i++];
            }            
        }
    }   
}
