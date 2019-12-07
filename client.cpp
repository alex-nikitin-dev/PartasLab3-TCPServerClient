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
int main()
{
	struct sockaddr_in sockAddr;
	int socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketFD == -1)
	{
		perror("can't create a socket\n");
		return EXIT_FAILURE;
	}
	//fill structure with 0
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_port = htons(port);
	//convert address to byte-form
	int binaryAddr = inet_pton(PF_INET, serverIP, &sockAddr.sin_addr);

	if (binaryAddr < 0)
	{
		perror("family address is incorrect\n");
		close(socketFD);
		return EXIT_FAILURE;
	}
	else if (!binaryAddr)
	{
		perror("IP address is incorrect\n");
		close(socketFD);
		return EXIT_FAILURE;
	}

	if (connect(socketFD, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1)
	{
		perror("can't connect to the server\n");
		close(socketFD);
		return EXIT_FAILURE;
	}

	const char *pathToReceive = "11.jpg";
	printf("Start receiving the file %s from server...\n", pathToReceive);
	ServerFunc func = ReceiveFile;
	if (send(socketFD, &func, sizeof(ServerFunc), 0) < 0)
	{
		perror("can't send function flag to the server\n");
		close(socketFD);
		return EXIT_FAILURE;
	}

	if (send(socketFD, pathToReceive, strlen(pathToReceive) + 1, 0) < 0)
	{
		perror("can't send path of the file to the server\n");
		close(socketFD);
		return EXIT_FAILURE;
	}

	char buf[4096];
	ssize_t receivedBytes;
	const char *pathToSave = "111.jpg";
	FILE *fileFD = fopen(pathToSave, "wb");
	if (fileFD == nullptr)
	{
		printf("can't open file %s to rewrite\n", pathToSave);
		close(socketFD);
		return EXIT_FAILURE;
	}
	fclose(fileFD);
	fileFD = fopen(pathToSave, "ab+");
	if (fileFD == nullptr)
	{
		printf("can't open file %s to append\n", pathToSave);
		close(socketFD);
		return EXIT_FAILURE;
	}

	while ((receivedBytes = recv(socketFD, buf, 4096, 0)) > 0)
	{
		if (receivedBytes == -1)
		{
			printf("can't receive file %s from the server\n", pathToReceive);
			fclose(fileFD);
			close(socketFD);
			return EXIT_FAILURE;
		}
		int wr = fwrite(buf, 1, receivedBytes, fileFD);
	}
	printf("The file %s has beed received successful and saved as %s\n", pathToReceive, pathToSave);
	fclose(fileFD);
	shutdown(socketFD, SHUT_RDWR);
	close(socketFD);
	return 0;
}