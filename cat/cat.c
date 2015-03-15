#include <helpers.h>
#include <stdlib.h>

#define BUF_SIZE 4096

int main()
{
    char buf[BUF_SIZE];
    while (1)
    {
        ssize_t read = read_(STDIN_FILENO, buf, BUF_SIZE);
        if (read == 0)
        {
            exit(EXIT_SUCCESS);
        } 
        else if (read == -1 || write_(STDOUT_FILENO, buf, read) == -1) 
        {
            exit(EXIT_FAILURE);
        }
    }
}
