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

	ServerFunc funcNumber;
	char path[4096];
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
			if (recv(connectFD, &path, 4096, 0) == -1)
			{
				perror("can't read the data\n");
				break;
			}

			//int fileFD = open(path, O_RDONLY, S_IRUSR | S_IWUSR);
			FILE *fileFD = fopen("11.jpg", "r");
			// if (fileFD == -1)
			// {
			// 	printf("can't open the requested file,%s\n", path);
			// 	break;
			// }

			int fileBufSize = 4096;
			char fileBuf[4096];
			ssize_t readBytes;
			ssize_t sentBytes;
			while ((readBytes = fread(&fileBuf, 1, fileBufSize, fileFD)) > 0)
			{
				sentBytes = send(connectFD, &fileBuf, readBytes, 0);
				if (readBytes != sentBytes)
				{
					perror("send file error\n");
					break;
				}
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