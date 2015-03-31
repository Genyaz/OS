#include <bufio.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct buf_t buf_t;
#define BUF_SIZE 4096

int main()
{
    buf_t* buf = buf_new(BUF_SIZE);
    if (buf == NULL) exit(EXIT_FAILURE);
    int last_read;   
    while ((last_read = buf_fill(STDIN_FILENO, buf, BUF_SIZE / 2)) > 0 
	&& buf_flush(STDOUT_FILENO, buf, buf_size(buf)) > 0);
    if (last_read == 0) exit(EXIT_SUCCESS);
    exit(EXIT_FAILURE);
}
