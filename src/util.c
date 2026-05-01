#include "util.h"

#include <libsocket.h>
#include <stdio.h>
#include <stdlib.h>

const char *memallocerrorstr = "Memory allocation failed. Application terminated.";

bool issockconnected(const Socket *socket)
{
    char tmp;
    ssize_t avail = socket_recv(socket, &tmp, sizeof(tmp), SOCKET_RECV_FLAG_PEEK);

    if (avail > 0) return true;
    else if (avail < 0 && socket_getlasterror() == WouldBlock) return true;
    else return false;
}

void handlesockerr(const char *funcname)
{
    printf("%s error: %s. Application aborted.\n", funcname, socket_strerror(socket_getlasterror()));
    abort();
}

void *malloc_s(size_t size)
{
    void *ret = malloc(size);
    if (!ret) { puts(memallocerrorstr); abort(); }
    return ret;
}

void *realloc_s(void *ptr, size_t size)
{
    void *ret = realloc(ptr, size);
    if (!ret) { puts(memallocerrorstr); abort(); }
    return ret;
}

void flushrecvbuff(const Socket *socket)
{
    char buff[4096];
    while (socket_recv(socket, buff, sizeof(buff), SOCKET_RECV_NOFLAGS) > 0);
}