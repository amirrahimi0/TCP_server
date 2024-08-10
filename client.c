#include "client.h"

int main()
{
	int clientSocket;
	struct sockaddr_in serverAddr;
	char buffer[1024];
	int PORT = 9000;

	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSocket < 0)
	{
		perror("[-]Client Socket Creation Failed");
		exit(1);
	}
	printf("[+]Client Socket Created Successfully.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("[-]Connection to Server Failed");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		printf("input = ");
		if (fgets(buffer, sizeof(buffer), stdin) != NULL)
		{
			buffer[strcspn(buffer, "\n")] = 0;
			send(clientSocket, buffer, strlen(buffer), 0);
		}
	}

	return 0;
}