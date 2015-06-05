#define _POSIX_SOURCE
#include <bufio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 4096
#define BACK_LOG 100

int main(int argc, char** argv)
{
	if (argc < 3) exit(EXIT_FAILURE);
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if (getaddrinfo(NULL, argv[1], &hints, &result) != 0) {
         exit(EXIT_FAILURE);
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd == -1)
            continue;
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(sfd);
    }

    if (rp == NULL) {
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    if (listen(sfd, BACK_LOG) == -1)
    {
        close(sfd);
        exit(EXIT_FAILURE);
    }
	while (1)
	{
        struct sockaddr_in client;
        socklen_t sz = sizeof(client);
        int listener = accept(sfd, (struct sockaddr*)&client, &sz);
        if (listener != -1)
        {
            int pid = fork();
            if (pid == -1)
            {
                close(sfd);
                close(listener);
                exit(EXIT_FAILURE);
            }
            if (pid == 0)
            {
                close(sfd);
                buf_t* buffer = buf_new(BUF_SIZE);
                if (buffer == NULL)
                {
                    close(listener);
                    exit(EXIT_FAILURE);
                }
                int file = open(argv[2], O_RDONLY);
                if (file == -1)
                {
                    buf_free(buffer);
                    close(listener);
                    exit(EXIT_FAILURE);
                }
                while (1)
                {
                    if (buf_fill(file, buffer, BUF_SIZE) == -1)
                    {
                        close(listener);
                        close(file);
                        buf_free(buffer);
                        exit(EXIT_FAILURE);
                    }
                    int required = BUF_SIZE;
                    if (buffer->size < BUF_SIZE)
                    {
                        required = buffer->size;
                    }
                    if (buf_flush(listener, buffer, required) == -1)
                    {
                        close(listener);
                        close(file);
                        buf_free(buffer);
                        exit(EXIT_FAILURE);
                    }
                    if (required < BUF_SIZE)
                    {
                        close(listener);
                        close(file);
                        buf_free(buffer);
                        exit(EXIT_SUCCESS);
                    }
                }
            }
            else
            {
                close(listener);
            }
        }
    }
}
