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
#include "client.h"
int main(int argc, char *argv[])
{

	int socketFD;

	if (argc == 2 && strcmp(argv[1], "help") == 0)
	{
		printf("1 - get-file, 2 - ip, 3 - port, 4 - pathToReseive, 5 - pathToSave\n");
		printf("1 - show-dir, 2 - ip, 3 - port, 4 - pathFolderToShow\n");
		return EXIT_SUCCESS;
	}
	else if (argc == 6 && strcmp(argv[1], "get-file") == 0)
	{
		if (!ClientInitialize(socketFD, argv, ReceiveFile))
			return EXIT_FAILURE;
		ReceiveFileFromServer(socketFD, argv[4], argv[5], pathLength, 4096);
	}
	else if (argc == 5 && strcmp(argv[1], "show-dir") == 0)
	{
		if (!ClientInitialize(socketFD, argv, GetFolderContent))
			return EXIT_FAILURE;
		GetFolderContentFromServer(socketFD, argv[4], pathLength, 4096);
	}
	else
	{
		printf("The arguments is inappropriate. See help\n");
		return EXIT_FAILURE;
	}

	shutdown(socketFD, SHUT_RDWR);
	close(socketFD);
	return 0;
}

bool ClientInitialize(int &socketFD, char *argv[], ServerFunc func)
{
	char ip[255];
	int port;
	strcpy(ip, argv[2]);
	port = atoi(argv[3]);
	if (!ClientInitialize(socketFD, ip, port))
		return false;
	if (!SendMenuFlag(socketFD, func))
		return false;
	return true;
}
bool GetFolderContentFromServer(int socketFD, char *path, int pathSize, int bufferSizeToReceiving)
{
	printf("Start getting content of the path %s from server...\n", path);
	if (!SendString(socketFD, path))
		return false;
	bool receiveAnything = false;
	char buf[bufferSizeToReceiving];
	ssize_t receivedBytes;
	while ((receivedBytes = recv(socketFD, buf, bufferSizeToReceiving, 0)) > 0)
	{
		int wr = write(1, buf, receivedBytes);
		receiveAnything = true;
	}
	if (receivedBytes == -1 || receiveAnything == false)
	{
		printf("Can't receive content of the path %s from the server\n", path);
		close(socketFD);
		return false;
	}
	printf("\nThe content of the path %s has beed received successful\n", path);
	return true;
}
bool SendString(int socketFD, char *str)
{
	if (send(socketFD, str, strlen(str) + 1, 0) < 0)
	{
		printf("Can't send the sting %s to the server\n", str);
		close(socketFD);
		return false;
	}
	return true;
}
bool ReceiveFileFromServer(int socketFD, char *pathToReceive, char *pathToSave, int pathSize, int bufferSizeToReceiving)
{
	printf("Start receiving the file %s from server...\n", pathToReceive);

	if (!SendString(socketFD, pathToReceive))
		return false;

	char buf[bufferSizeToReceiving];
	ssize_t receivedBytes;
	FILE *fileFD = fopen(pathToSave, "wb");
	if (fileFD == nullptr)
	{
		printf("Can't open file %s to rewrite\n", pathToSave);
		close(socketFD);
		return false;
	}
	fclose(fileFD);
	fileFD = fopen(pathToSave, "ab+");
	if (fileFD == nullptr)
	{
		printf("Can't open file %s to append\n", pathToSave);
		close(socketFD);
		return false;
	}
	bool fileExists = false;
	while ((receivedBytes = recv(socketFD, buf, bufferSizeToReceiving, 0)) > 0)
	{
		int wr = fwrite(buf, 1, receivedBytes, fileFD);
		fileExists = true;
	}
	if (receivedBytes == -1 || fileExists == false)
	{
		printf("Can't receive file %s from the server\n", pathToReceive);
		remove(pathToSave);
		fclose(fileFD);
		close(socketFD);
		return false;
	}
	printf("The file %s has beed received successful and saved as %s\n", pathToReceive, pathToSave);
	fclose(fileFD);
}

bool SendMenuFlag(int socketFD, ServerFunc menuFunc)
{
	if (send(socketFD, &menuFunc, sizeof(ServerFunc), 0) < 0)
	{
		perror("Can't send function flag to the server\n");
		close(socketFD);
		return false;
	}
	return true;
}
bool ClientInitialize(int &socketFD, char *ip, int port)
{
	sockaddr_in sockAddr;
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
	//convert address to byte-form
	int binaryAddrResult = inet_pton(PF_INET, ip, &sockAddr.sin_addr);

	if (binaryAddrResult < 0)
	{
		perror("family address is incorrect\n");
		close(socketFD);
		return false;
	}
	else if (!binaryAddrResult)
	{
		perror("IP address is incorrect\n");
		close(socketFD);
		return false;
	}

	if (connect(socketFD, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1)
	{
		perror("can't connect to the server\n");
		close(socketFD);
		return false;
	}
	return true;
}

void ShowArgs(int argc, char *argv[])
{
	printf("argc=%d\n", argc);
	for (int i = 0; i < argc; i++)
	{
		printf("arg[%d]=%s\n", i, argv[i]);
	}
}