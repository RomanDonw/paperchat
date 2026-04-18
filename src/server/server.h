#ifndef PROCESSCONNECTIONS_H
#define PROCESSCONNECTIONS_H

#include <libsocket.h>

Socket **clients = NULL;
size_t clients_count = 0;

void processconnections(const Socket *serv);
void closeall(void);

#endif