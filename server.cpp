#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define port 1100

int main(void)
{
	struct sockaddr_in sockAddr;
	int socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (socketFD == -1)
	{
		perror("can't create a socket\n");
		exit(EXIT_FAILURE);
	}
	//fill structure with 0
	memset(&sockAddr, 0, sizeof(sockAddr));

	sockAddr.sin_family = PF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(socketFD, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1)
	{
		perror("can't bind the socket to address\n");

		close(socketFD);
		exit(EXIT_FAILURE);
	}

	if (listen(socketFD, 10) == -1)
	{
		perror("can't start listening for connections on the socket\n");

		close(socketFD);
		exit(EXIT_FAILURE);
	}

	int buf = 0;
	while (true)
	{
		printf("start waiting for a new connection...\n");
		int connectFD = accept(socketFD, 0, 0);
		if (connectFD < 0)
		{
			perror("can't accept a connection\n");
			close(socketFD);
			exit(EXIT_FAILURE);
		}

		while (buf != -1)
		{
			if (read(connectFD, &buf, 4) < 0)
			{
				perror("can't read the data\n");
				break;
			}
			else
			{
				printf("received:%i\n", buf);
			}
		}

		printf("connection shut down\n");
		buf = 0;
		shutdown(connectFD, SHUT_RDWR);
		close(connectFD);
	}

	return 0;
}