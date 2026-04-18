#include "util.h"

#include <libsocket.h>
#include <stdio.h>

void handlesockerr(const char *funcname)
{
    printf("%s error: %s. Application aborted.\n", funcname, socket_strerror(socket_getlasterror()));
    abort();
}

void *malloc_s(size_t size)
{
    void *ret = malloc(size);
    if (!ret) { puts("Memory allocation failed. Application terminated."); abort(); }
    return ret;
}

void *realloc_s(void *ptr, size_t size)
{
    void *ret = realloc(ptr, size);
    if (!ret) { puts("Memory allocation failed. Application terminated."); abort(); }
    return ret;
}