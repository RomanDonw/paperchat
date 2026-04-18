#include <stdio.h>
#include <libsocket.h>
#include <getopt.h>
#include <stdlib.h>

#include "../util.h"
#include "client.h"

int main(int argc, char *argv[])
{
    char *addr = NULL;
    const size_t MAX_USERNAME_LENGTH = 255;
    char username[MAX_USERNAME_LENGTH];
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
                memcpy(username, optarg, MAX_USERNAME_LENGTH);
                username[MAX_USERNAME_LENGTH] = '\0';
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
        puts("Not specified required parameter -n <username up to 255 bytes>.");
        return 1;
    }

    Socket *cl = socket_open(IPv4, Stream, TCP);
    if (!cl) handlesockerr("socket_open");

    printf("Connecting to %s:%d with username \"%s\"...\n", addr, port, username);
    if (!socket_connect(cl, addr, port)) handlesockerr("socket_connect");
    puts("Connected!");

    client(cl);

    if (!socket_close(cl)) handlesockerr("socket_close");

    return 0;
}