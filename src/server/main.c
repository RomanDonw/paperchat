#include <stdio.h>
#include <libsocket.h>
#include <getopt.h>

#include "server.h"
#include "../util.h"

int main(int argc, char *argv[])
{
    char *addr = "0.0.0.0";
    unsigned short port = 12780;

    int p;
    while ((p = getopt(argc, argv, "a:p:")) != -1)
    {
        switch (p)
        {
            case 'a':
                addr = optarg;
                break;

            case 'p':
                sscanf(optarg, "%hd", &port);
                break;
        }
    }

    Socket *serv = socket_open(IPv4, Stream, TCP);
    if (!serv) handlesockerr("socket_open");

    if (!socket_bind(serv, addr, port)) handlesockerr("socket_bind");
    if (!socket_listen(serv, 5)) handlesockerr("socket_listen");

    printf("Opened server at %s:%d.\n", addr, port);
    puts("Press Enter to stop the server.");

    server(serv);

    if (!socket_close(serv)) handlesockerr("socket_close");

    return 0;
}