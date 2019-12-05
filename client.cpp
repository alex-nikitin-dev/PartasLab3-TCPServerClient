#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

	for (int buf = 10; buf != -1;)
	{
		scanf("%d", &buf);
		if (send(socketDesc, &buf, 4, 0) < 0)
		{
			printf("sending failed\n");
			break;
		}
		printf("sending ok\n");
	}

	shutdown(socketDesc, SHUT_RDWR);
	close(socketDesc);
	return 0;
}