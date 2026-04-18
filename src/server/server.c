#include "server.h"

#include <stdio.h>
#include <stdlib.h>

Socket **cls = NULL;
size_t cls_count = 0;

bool acceptconn(const Socket *serv);
void broadcast(const char *str);
void cleanup();

void server(const Socket *serv)
{
    while (getc(stdin) == -1)
    {
        while (acceptconn(serv));

        for (size_t i = 0; i < cls_count; i++)
        {
            Socket *cl = cls[i];
            
            
        }

        cleanup();
    }
}

bool acceptconn(const Socket *serv)
{
    Socket *cl = socket_accept(serv);
    if (!cl) return false;

    {
        Socket **new_cls = (Socket **)realloc(cls, sizeof(Socket *) * (cls_count + 1));
        if (!new_cls) { puts("Memory allocation failed. Application terminated."); abort(); }
        cls = new_cls;
        cls_count++;
    }

    cls[cls_count - 1] = cl;

    int enable = true;
    if (!socket_ioctl(cl, NonBlockingIO, &enable)) 

    return true;
}