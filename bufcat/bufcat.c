#include <bufio.h>
#include <stdlib.h>
typedef struct buf_t buf_t;
#define BUF_SIZE 4096

int main()
{
    buf_t* buf = buf_new(BUF_SIZE);
    int reading = 1;    
    while (buf_fill(STDIN_FILENO, buf, BUF_SIZE / 2) > 0 
	&& buf_flush(STDOUT_FILENO, buf, buf_size(buf)) > 0);
}
