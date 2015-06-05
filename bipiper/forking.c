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
    int sfd1, sfd2;
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
        sfd1 = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd1 == -1)
            continue;
        if (bind(sfd1, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(sfd1);
    }

    if (rp == NULL) {
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if (getaddrinfo(NULL, argv[2], &hints, &result) != 0) {
         exit(EXIT_FAILURE);
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd2 = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd2 == -1)
            continue;
        if (bind(sfd2, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(sfd2);
    }

    if (rp == NULL) {
        close(sfd1);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    if (listen(sfd1, BACK_LOG) == -1 || listen(sfd2, BACK_LOG) == -1)
    {
        close(sfd1);
        close(sfd2);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        struct sockaddr_in client;
        socklen_t sz = sizeof(client);
        int listener1 = accept(sfd1, (struct sockaddr*)&client, &sz);
        if (listener1 != -1)
        {
            while (1)
            {
                int listener2 = accept(sfd2, (struct sockaddr*)&client, &sz);
                if (listener2 != -1)
                {
                    int pid = fork();
                    if (pid == -1)
                    {
                        close(sfd1);
                        close(sfd2);
                        close(listener1);
                        close(listener2);
                        exit(EXIT_FAILURE);
                    }
                    if (pid == 0)
                    {
                        close(sfd1);
                        close(sfd2);
                        buf_t* buffer = buf_new(BUF_SIZE);
                        if (buffer == NULL)
                        {
                            close(listener1);
                            close(listener2);
                            exit(EXIT_FAILURE);
                        }
                        int closed = 0;
                        while (1)
                        {
                            if (buf_fill(listener1, buffer, 1) == -1)
                            {
                                close(listener1);
                                close(listener2);
                                buf_free(buffer);
                                exit(EXIT_FAILURE);
                            }
                            if (buffer->size == 0)
                            {
                                shutdown(listener1, SHUT_RD);
                                shutdown(listener2, SHUT_WR);
                                buf_free(buffer);
                                exit(EXIT_SUCCESS);
                            }
                            if (buf_flush(listener2, buffer, buffer->size) == -1)
                            {
                                close(listener1);
                                close(listener2);
                                buf_free(buffer);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    else
                    {
                        pid = fork();
                        if (pid == -1)
                        {
                            close(sfd1);
                            close(sfd2);
                            close(listener1);
                            close(listener2);
                            exit(EXIT_FAILURE);
                        }
                        if (pid == 0)
                        {
                            close(sfd1);
                            close(sfd2);
                            buf_t* buffer = buf_new(BUF_SIZE);
                            if (buffer == NULL)
                            {
                                close(listener1);
                                close(listener2);
                                exit(EXIT_FAILURE);
                            }
                            while (1)
                            {
                                if (buf_fill(listener2, buffer, 1) == -1)
                                {
                                    close(listener1);
                                    close(listener2);
                                    buf_free(buffer);
                                    exit(EXIT_FAILURE);
                                }
                                if (buffer->size == 0)
                                {
                                    shutdown(listener2, SHUT_RD);
                                    shutdown(listener1, SHUT_WR);
                                    buf_free(buffer);
                                    exit(EXIT_SUCCESS);
                                }
                                if (buf_flush(listener1, buffer, buffer->size) == -1)
                                {
                                    close(listener1);
                                    close(listener2);
                                    buf_free(buffer);
                                    exit(EXIT_FAILURE);
                                }
                            }
                        }
                        else
                        {
                            close(listener1);
                            close(listener2);
                            break;
                        }
                    }
                }
            }
        }
    }
}
