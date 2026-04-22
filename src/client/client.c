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
		socket_recv(cl, msg, 1023, RECV_NOFLAGS);
		msg[1023] = '\0';
	}

	return NULL;
}

void client(const Socket *cl)
{
	pthread_t thr_rcv;
	pthread_create(&thr_rcv, NULL, recvloop, &cl);

	while (enabled)
	{
		char *line = NULL;
		size_t len;
		if (getline(&line, &len, stdin) > 0)
		{
			socket_send(cl, line, len);
		}
		if (line) free(line);
	}

	pthread_join(thr_rcv, NULL);
}
