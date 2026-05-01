#include "client.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#include "../util.h"

bool enabled = true;

void *recvloop(void *arg)
{
	Socket *cl = *((Socket **)arg);

	while (enabled)
	{
		ssize_t avail = socket_recv(cl, NULL, 0, SOCKET_RECV_FLAG_PEEK);
		if (avail <= 0) { enabled = false; break; }
		
		char *msg = malloc_s(avail + 1);

		ssize_t readbytes = socket_recv(cl, msg, avail, SOCKET_RECV_NOFLAGS);
		if (readbytes <= 0) { enabled = false; break; }

		msg[readbytes] = '\0';
		printf("%s", msg);

		free(msg);
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
