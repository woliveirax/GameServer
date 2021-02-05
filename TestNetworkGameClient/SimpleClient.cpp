#include "../GameServer/NetworkHandler.h"
#include <Ws2tcpip.h>

int main()
{
	SOCKET sock = GetSocket(0);
	if (sock == INVALID_SOCKET)
	{
		printf("Error initializing socket: %d", WSAGetLastError());
		return -1;
	}

	SOCKADDR_IN server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9999);
	inet_pton(AF_INET,"127.0.0.1", &server_address.sin_addr);

	char message[SOCKET_BUFFER_SIZE];
	gets_s(message, SOCKET_BUFFER_SIZE);

	int flags = 0;
	if (sendto(sock, message, SOCKET_BUFFER_SIZE, flags, (SOCKADDR *) &server_address, sizeof(server_address)) ==
		SOCKET_ERROR)
	{
		printf("sendto failed: %d", WSAGetLastError());
		return -1;
	}

	return 0;
}