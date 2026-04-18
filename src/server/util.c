#include "util.h"

#include <libsocket.h>
#include <stdio.h>

void handlesockerr(const char *funcname)
{
    printf("%s error: %s. Application aborted.\n", funcname, socket_strerror(socket_getlasterror()));
    abort();
}