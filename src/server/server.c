#include "server.h"

#include <stdio.h>
#include <stdlib.h>

#include "../util.h"

Socket **cls = NULL;
size_t cls_count = 0;

bool acceptconn(const Socket *serv);
void broadcast(const char *data, size_t len);
void cleanconns(void);

void server(const Socket *serv)
{
    while (getc(stdin) == -1)
    {
        while (acceptconn(serv));

        for (size_t i = 0; i < cls_count; i++)
        {
            Socket *cl = cls[i];
            
            unsigned long avail;
            socket_ioctl(cl, AvailableDataToRead, &avail);
            if (avail > 0)
            {
                char *msg = malloc_s(sizeof(char) * avail);
                socket_recv(cl, msg, avail, RECV_NOFLAGS);
                broadcast(msg, avail);
            }
        }

        cleanconns();
    }
}

bool acceptconn(const Socket *serv)
{
    Socket *cl = socket_accept(serv);
    if (!cl) return false;

    puts("accepted!");

    cls = (Socket **)realloc_s(cls, sizeof(Socket *) * (cls_count + 1));
    cls[cls_count] = cl;
    cls_count++;

    int enable = true;
    if (!socket_ioctl(cl, NonBlockingIO, &enable)) handlesockerr("socket_ioctl(NonBlockingIO, true)");
    enable = true;
    if (!socket_setopt(cl, SocketLevel, Socket_KeepAliveConnection, &enable, sizeof(enable))) handlesockerr("socket_setopt(SocketLevel, Socket_KeepAliveConnection)");

    puts("accepted conn");

    return true;
}

void cleanconns(void)
{
    Socket **new_cls = NULL;
    size_t new_cls_count = 0;

    for (size_t i = 0; i < cls_count; i++)
    {
        Socket *cl = cls[i];
        if (issockconnected(cl))
        {
            new_cls = (Socket **)realloc_s(new_cls, sizeof(Socket *) * (new_cls_count + 1));
            new_cls[new_cls_count] = cl;
            new_cls_count++;
        }
    }

    cls = new_cls;
    cls_count = new_cls_count;
}

void broadcast(const char *data, size_t len)
{
    for (size_t i = 0; i < cls_count; i++)
    {
        Socket *cl = cls[i];

        printf("Broadcast message: ");
        for (size_t j = 0; j < len; j++) putchar(data[j]);

        socket_send(cl, data, len);
    }
}
