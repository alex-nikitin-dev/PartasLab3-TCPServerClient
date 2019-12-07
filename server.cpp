#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "interaction.h"
#include "server.h"
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

	ServerFunc funcNumber;
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

		recv(connectFD, &funcNumber, sizeof(ServerFunc), 0);

		switch (funcNumber)
		{
		//a file is requested
		case ReceiveFile:
		{
			printf("get connection to send a file...\n");
			char path[4096];
			if (recv(connectFD, &path, pathLength, 0) == -1)
			{
				perror("can't read the data\n");
				break;
			}
			FILE *fileFD = fopen(path, "r");
			if (fileFD == NULL)
			{
				printf("can't open the file %s\n", path);
				break;
			}
			printf("sending file %s...\n", path);
			if (!SendFile(fileFD, 4096, connectFD))
			{
				perror("can't send the file\n");
			}
			else
			{
				printf("send OK\n");
			}
			fclose(fileFD);
		}
		break;
		default:
			break;
		}

		printf("connection is shutting down...\n");
		shutdown(connectFD, SHUT_RDWR);
		close(connectFD);
	}

	return 0;
}

bool SendFile(FILE *fileFD, int fileBufSize, int connectFD)
{
	char fileBuf[fileBufSize];
	ssize_t readBytes;
	ssize_t sentBytes;
	while ((readBytes = fread(&fileBuf, 1, fileBufSize, fileFD)) > 0)
	{
		sentBytes = send(connectFD, &fileBuf, readBytes, 0);
		if (readBytes != sentBytes)
		{
			return false;
		}
	}
	return true;
}