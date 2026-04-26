#include "client.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

bool enabled = true;

void *recvloop(void *arg)
{
	Socket *cl = *((Socket **)arg);

	while (enabled)
	{
		char msg[1024];
		socket_recv(cl, msg, 1023, SOCKET_RECV_NOFLAGS);
		msg[1023] = '\0';
		puts(msg);
	}

	return NULL;
}

void client(const Socket *cl)
{
	pthread_t thr_rcv;
	pthread_create(&thr_rcv, NULL, recvloop, &cl);

	char line[256];
	while (enabled)
	{
		if (!feof(stdin) && fgets(line, 256, stdin)) socket_send(cl, line, strlen(line), SOCKET_SEND_NOFLAGS);
	}

	enabled = false;
	pthread_join(thr_rcv, NULL);
}
