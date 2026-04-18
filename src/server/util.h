#ifndef UTIL_H
#define UTIL_H

void handlesockerr(const char *funcname);

void *malloc_s(size_t size);
void *realloc_s(void *ptr, size_t size);

#endif