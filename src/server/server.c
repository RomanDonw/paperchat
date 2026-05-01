#include "server.h"

#include <stdio.h>
#include <stdlib.h>

#include "../util.h"

#define USERNAME_MAXLEN 32

struct
{
    Socket *sock;

    SocketIPv4Address sockaddr;
    char addrstr[IPV4ADDRSTRSIZE];
    unsigned short port;

    char username[USERNAME_MAXLEN + 1];
} typedef Client;

Client *clients = NULL;
size_t clientscount = 0;

Client *acceptconn(const Socket *serv);
void broadcast(const char *data, size_t len, const Client *ignrsocs, size_t ignrsockscount);
void cleanconns(void);

void server(const Socket *serv)
{
    while (true)
    {
        Client *cl;
        while (cl = acceptconn(serv))
        {
            //puts("in accept conn");

            /*
            bool enable = true;
            if (!socket_ioctl(cl->sock, NonBlockingIO, &enable)) handlesockerr("socket_ioctl(NonBlockingIO, true)");
            int keepaliveconn = true;
            if (!socket_setopt(cl->sock, SocketLevel, Socket_KeepAliveConnection, &keepaliveconn, sizeof(keepaliveconn))) handlesockerr("socket_setopt(SocketLevel, Socket_KeepAliveConnection)");
            */

            ssize_t readbytes = socket_recv(cl->sock, cl->username, USERNAME_MAXLEN, SOCKET_RECV_NOFLAGS);
            if (readbytes <= 0) continue;
            cl->username[readbytes] = '\0';

            flushrecvbuff(cl->sock);
        }

        for (size_t i = 0; i < clientscount; i++)
        {            
            unsigned long avail;
            socket_ioctl(clients[i].sock, AvailableDataToRead, &avail);
            if (avail > 0)
            {
                char *recvmsg = malloc_s(avail + 1);
                socket_recv(clients[i].sock, recvmsg, avail, SOCKET_RECV_NOFLAGS);
                recvmsg[avail] = '\0';

                size_t sendmsgsz = avail + USERNAME_MAXLEN + 8 + 1;
                char *sendmsg = malloc_s(sendmsgsz);
                size_t sendmsglen = sprintf_s(sendmsg, sendmsgsz, "[%s]: %s", clients[i].username, recvmsg);

                broadcast(sendmsg, sendmsglen, &clients[i], 1);

                free(sendmsg);
                free(recvmsg);
            }
        }

        cleanconns();
    }
}

Client *acceptconn(const Socket *serv)
{
    SocketIPv4Address addr;
    socklen_t addrsz = sizeof(addr);
    Socket *cl = socket_accept(serv, &addr, &addrsz);
    if (!cl) return NULL;
    if (addrsz != sizeof(addr)) { puts("addrsz != sizeof(addr)"); abort(); }

    IPv4Address ipaddr;
    unsigned short port;
    if (!socket_unpacksockaddr(&addr, IPv4, &ipaddr, &port)) handlesockerr("socket_unpacksockaddr");

    clients = (Client *)realloc_s(clients, sizeof(Client) * (clientscount + 1));
    clients[clientscount].sock = cl;
    clients[clientscount].sockaddr = addr;
    clients[clientscount].port = port;
    if (!socket_addrtostr(&addr, IPv4, clients[clientscount].addrstr, sizeof(clients[clientscount].addrstr))) handlesockerr("socket_addrtostr");

    printf("accecpted client %s:%d\n", clients[clientscount].addrstr, clients[clientscount].port);

    return &clients[clientscount++];
}

void cleanconns(void)
{
    Client *newclients = NULL;
    size_t newclientscount = 0;

    for (size_t i = 0; i < clientscount; i++)
    {
        if (issockconnected(clients[i].sock))
        {
            newclients = (Client *)realloc_s(newclients, sizeof(Client) * (newclientscount + 1));
            newclients[newclientscount] = clients[i];
            newclientscount++;
        }
        else printf("disconnected client %s:%d\n", clients[i].addrstr, clients[i].port);
    }

    free(clients);
    clients = newclients;
    clientscount = newclientscount;
}

void broadcast(const char *data, size_t len, const Client *ignrsocks, size_t ignrsockscount)
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
                if (ignrsocks[j].sock == clients[i].sock) { skipsock = true; break; }
            }
            if (skipsock) continue;
        }
        
        socket_send(clients[i].sock, data, len, SOCKET_SEND_NOFLAGS);
    }
}