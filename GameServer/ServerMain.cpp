#include "NetworkHandler.h"

int main(int argc, char** argv)
{
	SOCKET sock = GetSocket();
	if (sock == INVALID_SOCKET)
	{
		printf("failed to start socket: %d", WSAGetLastError());
		return -1;
	}

	char buffer[SOCKET_BUFFER_SIZE+1];
	int bytes_received = 0;
	int flags = 0;
	SOCKADDR_IN from;
	int from_size = sizeof(from);

	bytes_received = recvfrom(sock, buffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);
	if (bytes_received == SOCKET_ERROR)
	{
		printf("recvfrom return SOCKET_ERROR, WSAGetLastError() %d", WSAGetLastError());
	}
	else
	{
		buffer[bytes_received] = 0;
		printf("%d.%d.%d.%d:%d - %s",
			from.sin_addr.S_un.S_un_b.s_b1,
			from.sin_addr.S_un.S_un_b.s_b2,
			from.sin_addr.S_un.S_un_b.s_b3,
			from.sin_addr.S_un.S_un_b.s_b4,
			ntohs(from.sin_port),
			buffer);
	}

	return 0;
}