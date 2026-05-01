#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdbool.h>
#include <libsocket.h>

void handlesockerr(const char *funcname);
bool issockconnected(const Socket *socket);
void flushrecvbuff(const Socket *socket);

void *malloc_s(size_t size);
void *realloc_s(void *ptr, size_t size);

#endif