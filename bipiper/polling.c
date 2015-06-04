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
#include <poll.h>
#include <errno.h>

#define BUF_SIZE 256
#define MAX_CLIENTS 127
#define BACK_LOG 100

typedef struct {
    int closed1to2;
    int closed2to1;
    buf_t* buf1to2;
    buf_t* buf2to1;
} mypipe;

void initpipe(mypipe* p)
{
    p->closed1to2 = 0;
    p->closed2to1 = 0;
    p->buf1to2->size = 0;
    p->buf2to1->size = 0;
}

void copypipe(mypipe * from, mypipe* to)
{
    to->closed1to2 = from->closed1to2;
    to->closed2to1 = from->closed2to1;
    to->buf1to2 = from->buf1to2;
    to->buf2to1 = from->buf2to1;
}

void swappipes(mypipe* a, mypipe* b)
{
    mypipe tmp;
    copypipe(a, &tmp);
    copypipe(b, a);
    copypipe(&tmp, b);
}

void copypollfd(struct pollfd* from, struct pollfd* to)
{
    to->fd = from->fd;
    to->events = from->events;
    to->revents = from->revents;
}

void swappollfds(struct pollfd* a, struct pollfd* b)
{
    struct pollfd tmp;
    copypollfd(a, &tmp);
    copypollfd(b, a);
    copypollfd(&tmp, b);
}

int handle(mypipe* p, struct pollfd* fd1, struct pollfd* fd2)
{
    if (((fd1->revents & POLLERR) != 0) || ((fd2->revents & POLLERR) != 0))
    {
        return -1;
    }
    if (!p->closed2to1 && ((fd1->revents & POLLOUT) != 0) && p->buf2to1->size > 0)
    {
        if (buf_flush(fd1->fd, p->buf2to1, 1) == -1) return -1;
    }
    if (!p->closed1to2 && ((fd2->revents & POLLOUT) != 0) && p->buf1to2->size > 0)
    {
        if (buf_flush(fd2->fd, p->buf1to2, 1) == -1) return -1;
    }
    if (!p->closed1to2 && ((fd1->revents & POLLIN) != 0) && p->buf1to2->size < p->buf1to2->capacity)
    {
        int prev = p->buf1to2->size;
        if (buf_fill(fd1->fd, p->buf1to2, p->buf1to2->size + 1) == -1) return -1;
        if (prev == p->buf1to2->size)
        {
            p->closed1to2 = 1;
            shutdown(fd1->fd, SHUT_RD);
            //shutdown(fd2->fd, SHUT_WR);
        }
    }
    if (!p->closed2to1 && ((fd2->revents & POLLIN) != 0) && p->buf2to1->size < p->buf2to1->capacity)
    {
        int prev = p->buf2to1->size;
        if (buf_fill(fd2->fd, p->buf2to1, p->buf2to1->size + 1) == -1) return -1;
        if (prev == p->buf2to1->size)
        {
            p->closed2to1 = 1;
            shutdown(fd2->fd, SHUT_RD);
            //shutdown(fd1->fd, SHUT_WR);
        }
    }
    if (p->closed1to2 && p->closed2to1) return -1;
    fd1->revents = 0;
    fd2->revents = 0;
    fd1->events = 0;
    fd2->events = 0;
    if (!p->closed1to2 && p->buf1to2->size < p->buf1to2->capacity) fd1->events = POLLIN;
    if (!p->closed2to1 && p->buf2to1->size < p->buf2to1->capacity) fd2->events = POLLIN;
    if (!p->closed2to1 && p->buf2to1->size > 0) fd1->events = fd1->events | POLLOUT;
    if (!p->closed1to2 && p->buf1to2->size > 0) fd2->events = fd2->events | POLLOUT;
    return 0;
}

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
    struct pollfd fds[2 * MAX_CLIENTS + 2];
    mypipe pipes[MAX_CLIENTS];
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        pipes[i].buf1to2 = buf_new(BUF_SIZE);
        pipes[i].buf2to1 = buf_new(BUF_SIZE);
        if (pipes[i].buf1to2 == NULL || pipes[i].buf2to1 == NULL) 
        {
            close(sfd1);
            close(sfd2);
            exit(EXIT_FAILURE);
        }
    }
    int connected = 0;
    int fd1 = 0;
    fds[0].fd = sfd1;
    fds[0].revents = 0;
    fds[1].fd = sfd2;
    fds[1].revents = 0;
    if (listen(sfd1, BACK_LOG) == -1) exit(EXIT_FAILURE);
    if (listen(sfd2, BACK_LOG) == -1)
    {
        close(sfd1);
        exit(EXIT_FAILURE);
    }
    //write(STDOUT_FILENO, "Connection prepared\n", 20);
    while (1)
    {
        if (connected == MAX_CLIENTS)
        {
            fds[0].events = 0;
            fds[1].events = 0;
        }
        else if (fd1 == 0)
        {
            fds[0].events = POLLIN;
            fds[1].events = 0;
        }
        else
        {
            fds[0].events = 0;
            fds[1].events = POLLIN;
        }
        if (poll(fds, 2 + 2 * connected, -1) == -1 && errno != EINTR)
        {
            for (i = 0; i < 2 + 2 * connected; i++) close(fds[i].fd);
            exit(EXIT_FAILURE);
        }
        //write(STDOUT_FILENO, "Poll returned\n", 14);
        // handle old connections
        for (i = 0; i < connected; i++)
        {
            if (handle(pipes + i, fds + 2 + 2 * i, fds + 3 + 2 * i) == -1)
            {
                close(fds[2 + 2 * i].fd);
                close(fds[3 + 2 * i].fd);
                swappipes(pipes + i, pipes + connected - 1);
                swappollfds(fds + 2 + 2 * i, fds + 2 + 2 * (connected - 1));
                swappollfds(fds + 3 + 2 * i, fds + 3 + 2 * (connected - 1));
                connected--;
            }
        }
        // handle new connections
        if (connected < MAX_CLIENTS)
        {
            if (fd1 == 1 && ((fds[1].revents & POLLIN) != 0))
            {
                //write(STDOUT_FILENO, "Second client connected\n", 24);
                struct sockaddr q;
                socklen_t sz = sizeof(q);
                int client1 = accept(sfd1, &q, &sz);
                int client2 = accept(sfd2, &q, &sz);
                if (client1 != -1 && client2 != -1)
                {
                    initpipe(pipes + connected);
                    fds[2 + 2 * connected].fd = client1;
                    fds[2 + 2 * connected].events = POLLIN | POLLERR;
                    fds[2 + 2 * connected].revents = 0;
                    fds[3 + 2 * connected].fd = client2;
                    fds[3 + 2 * connected].events = POLLIN | POLLERR;
                    fds[3 + 2 * connected].revents = 0;
                    connected++;
                }
                else
                {
                    if (client1 != -1) close(client1);
                    if (client2 != -1) close(client2);
                }
                fd1 = 0;
                fds[0].revents = 0;
                fds[1].revents = 0;
            }
            if (fd1 == 0 && ((fds[0].revents & POLLIN) != 0))
            {
                //write(STDOUT_FILENO, "First client connected\n", 23);
                fd1 = 1;
            }
        }
    }
}
