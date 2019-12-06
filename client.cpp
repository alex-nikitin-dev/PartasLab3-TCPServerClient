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
	int socketDesc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (socketDesc == -1)
	{
		perror("can't create a socket\n");
		return EXIT_FAILURE;
	}
	//fill structure with 0
	memset(&sockAddr, 0, sizeof(sockAddr));

	sockAddr.sin_family = PF_INET;
	sockAddr.sin_port = htons(1100);
	//convert address to byte-form
	int binaryAddr = inet_pton(PF_INET, "192.168.1.110", &sockAddr.sin_addr);

	if (binaryAddr < 0)
	{
		perror("family address is incorrect\n");
		close(socketDesc);
		return EXIT_FAILURE;
	}
	else if (!binaryAddr)
	{
		perror("IP address is incorrect\n");
		close(socketDesc);
		return EXIT_FAILURE;
	}

	if (connect(socketDesc, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1)
	{
		perror("can't connect to the server\n");
		close(socketDesc);
		return EXIT_FAILURE;
	}

	ServerFunc func = ReceiveFile;
	send(socketDesc, &func, sizeof(ServerFunc), 0);
	send(socketDesc, "11.jpg", 7, 0);

	char buf[4096];
	ssize_t receivedBytes;
	//nt fileFD = open("2.txt", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

	FILE *fileFD = fopen("111.jpg", "wb");
	fclose(fileFD);
	fileFD = fopen("111.jpg", "ab+");

	while ((receivedBytes = recv(socketDesc, buf, 4096, 0)) > 0)
	{
		//write(fileFD, &buf, receivedBytes);

		//int wr = write(fileFD, aaa, 2);
		int wr = fwrite(buf, 1, receivedBytes, fileFD);

		//printf("Receive file ok %s, bytes=%ld, wr=%d\n", buf, receivedBytes, wr);
	}
	fclose(fileFD);

	shutdown(socketDesc, SHUT_RDWR);
	close(socketDesc);
	return 0;
}