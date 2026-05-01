#include <stdio.h>
#include <libsocket.h>
#include <getopt.h>
#include <stdlib.h>

#include "../util.h"
#include "client.h"

#define USERNAME_MAXLEN 32

int main(int argc, char *argv[])
{
    char *addr = NULL;
    char username[USERNAME_MAXLEN + 1];
    bool hasusername = false;
    unsigned short port = 12780;

    int p;
    while ((p = getopt(argc, argv, "a:n:p:")) != -1)
    {
        switch (p)
        {
            case 'a':
                addr = optarg;
                break;

            case 'n':
                memcpy(username, optarg, USERNAME_MAXLEN);
                username[USERNAME_MAXLEN] = '\0';
                hasusername = true;
                break;

            case 'p':
                sscanf(optarg, "%hd", &port);
                break;
        }
    }

    if (!addr)
    {
        puts("Not specified required parameter -a <server IPv4 address>.");
        return 1;
    }

    if (!hasusername)
    {
        puts("Not specified required parameter -n <username up to 32 bytes>.");
        return 1;
    }

    Socket *cl = socket_open(IPv4, Stream, TCP);
    if (!cl) handlesockerr("socket_open");

    IPv4Address ipaddr;
    if (!socket_parseaddr(&ipaddr, IPv4, addr)) handlesockerr("socket_parseaddr");
    SocketIPv4Address saddr;
    if (!socket_packsockaddr(&saddr, IPv4, &ipaddr, port)) handlesockerr("socket_packsockaddr");

    //bool nonblock = true;
    //if (!socket_ioctl(cl, NonBlockingIO, &nonblock)) handlesockerr("socket_ioctl(NonBlockingIO)");
    int keepalvconn = true;
    if (!socket_setopt(cl, SocketLevel, Socket_KeepAliveConnection, &keepalvconn, sizeof(keepalvconn))) handlesockerr("socket_setopt(SocketLevel, Socket_KeepAliveConnection)");

    printf("Connecting to %s:%d with username \"%s\"...\n", addr, port, username);
    if (!socket_connect(cl, &saddr, sizeof(saddr))) handlesockerr("socket_connect");
    puts("Connected!");

    if (!socket_send(cl, username, USERNAME_MAXLEN, SOCKET_SEND_NOFLAGS)) handlesockerr("socket_send");

    client(cl);

    if (!socket_close(cl)) handlesockerr("socket_close");

    return 0;
}