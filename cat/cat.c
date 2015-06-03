#include <helpers.h>

#define BUF_SIZE 4096

int main()
{
    char buf[BUF_SIZE];
    while (1)
    {
        ssize_t read = read_(STDIN_FILENO, buf, BUF_SIZE);
        if (read == 0)
        {
            return 0;
        } 
        else if (read == -1 || write_(STDOUT_FILENO, buf, read) == -1) 
        {
            return 1;
        }
    }
}
