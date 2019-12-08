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
	int socketFD;
	if (!ServerInitializing(socketFD, ServerPort))
		return EXIT_FAILURE;
	ServerFunc funcNumber;
	int connectFD;
	while (true)
	{
		if (!WaitForNewConnection(socketFD, connectFD))
			return EXIT_FAILURE;
		recv(connectFD, &funcNumber, sizeof(ServerFunc), 0);
		char path[pathLength];
		FILE *fileFD;
		switch (funcNumber)
		{
		//a file is requested
		case ReceiveFile:
		{
			printf("got connection to send a file...\n");
			if (!ReceivePath(connectFD, path, pathLength))
				break;
			if ((fileFD = OpenFile(path, "r")) == nullptr)
				break;
			SendFile(fileFD, 4096, connectFD);
		}
		break;
		case GetFolderContent:
		{
		}
		break;
		default:
			break;
		}

		if (fileFD != nullptr)
			fclose(fileFD);
		printf("connection is shutting down...\n");
		shutdown(connectFD, SHUT_RDWR);
		close(connectFD);
	}

	close(socketFD);
	return 0;
}

FILE *OpenFile(const char *path, const char *mod)
{
	FILE *fileFD = fopen(path, mod);
	if (fileFD == nullptr)
	{
		printf("can't open the file %s\n", path);
		return nullptr;
	}
	return fileFD;
}

bool ReceivePath(int connectFD, char *path, int length)
{
	if (recv(connectFD, path, length, 0) == -1)
	{
		perror("can't read the data\n");
		return false;
	}
	return true;
}

bool SendFile(FILE *fileFD, int fileBufSize, int connectFD)
{
	printf("start sending the file...\n");
	char fileBuf[fileBufSize];
	ssize_t readBytes;
	ssize_t sentBytes;
	while ((readBytes = fread(&fileBuf, 1, fileBufSize, fileFD)) > 0)
	{
		sentBytes = send(connectFD, &fileBuf, readBytes, 0);
		if (readBytes != sentBytes)
		{
			perror("can't send the file\n");
			return false;
		}
	}
	printf("send OK\n");
	return true;
}

bool WaitForNewConnection(int socketFD, int &connectFD)
{
	printf("start waiting for a new connection...\n");
	connectFD = accept(socketFD, 0, 0);
	if (connectFD < 0)
	{
		perror("can't accept a connection\n");
		close(socketFD);
		return false;
	}
	return true;
}

bool ServerInitializing(int &socketFD, int port)
{
	struct sockaddr_in sockAddr;
	socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (socketFD == -1)
	{
		perror("can't create a socket\n");
		return false;
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
		return false;
	}

	if (listen(socketFD, 10) == -1)
	{
		perror("can't start listening for connections on the socket\n");

		close(socketFD);
		return false;
	}

	return true;
}