#include "server.h"

#include <stdio.h>
#include <stdlib.h>

#include "../util.h"

struct
{
    Socket *sock;
    SocketIPv4Address addr;
} typedef ClientSocket;

ClientSocket *clients = NULL;
size_t clientscount = 0;

bool acceptconn(const Socket *serv);
void broadcast(const char *data, size_t len, const ClientSocket *ignrsocs, size_t ignrsockscount);
void cleanconns(void);

void server(const Socket *serv)
{
    while (true)
    {
        while (acceptconn(serv));

        for (size_t i = 0; i < clientscount; i++)
        {            
            unsigned long avail;
            socket_ioctl(clients[i].sock, AvailableDataToRead, &avail);
            if (avail > 0)
            {
                char *msg = malloc_s(sizeof(char) * avail);
                socket_recv(clients[i].sock, msg, avail, RECV_NOFLAGS);
                broadcast(msg, avail, &clients[i], 1);
                free(msg);
            }
        }

        cleanconns();
    }
}

bool acceptconn(const Socket *serv)
{
    Socket *cl = socket_accept(serv, NULL, NULL);
    if (!cl) return false;

    puts("accepted!");

    clients = (ClientSocket *)realloc_s(clients, sizeof(ClientSocket) * (clientscount + 1));
    clients[clientscount].sock = cl;
    clientscount++;

    int enable = true;
    //if (!socket_ioctl(cl, NonBlockingIO, &enable)) handlesockerr("socket_ioctl(NonBlockingIO, true)");
    //enable = true;
    if (!socket_setopt(cl, SocketLevel, Socket_KeepAliveConnection, &enable, sizeof(enable))) handlesockerr("socket_setopt(SocketLevel, Socket_KeepAliveConnection)");

    puts("accepted conn");

    return true;
}

void cleanconns(void)
{
    ClientSocket *newclients = NULL;
    size_t newclientscount = 0;

    for (size_t i = 0; i < clientscount; i++)
    {
        if (issockconnected(clients[i].sock))
        {
            newclients = (ClientSocket *)realloc_s(newclients, sizeof(ClientSocket) * (newclientscount + 1));
            newclients[newclientscount] = clients[i];
            newclientscount++;
        }
        else
        {
            // output address of disconnected socket, that need to be saved in custom wrapper ClientSocket struct.
            puts("client disconnected");
        }
    }

    free(clients);
    clients = newclients;
    clientscount = newclientscount;
}

void broadcast(const char *data, size_t len, Socket **ignrsocks, size_t ignrsockscount)
{
    printf("Broadcast message: ");
    for (size_t j = 0; j < len; j++) putchar(data[j]);

    for (size_t i = 0; i < clientscount; i++)
    {
        if (ignrsocks && ignrsockscount > 0)
        {
            bool skipsock = false;
            for (size_t j = 0; j < ignrsockscount; j++)
            {
                if (ignrsocks[j] == clients[i]) { skipsock = true; break; }
            }
            if (skipsock) continue;
        }
        
        socket_send(cl, data, len, SEND_NOFLAGS);
    }
}
