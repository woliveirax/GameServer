#include "../GameServer/NetworkHandler.h"
#include <Ws2tcpip.h>
#include <conio.h>

int main()
{
	SOCKET sock = GetSocket(0);
	if (sock == INVALID_SOCKET)
	{
		printf("Error initializing socket: %d", WSAGetLastError());
		return -1;
	}

	while (true)
	{
		SOCKADDR_IN server_address;
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(9999);
		inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

		char message[SOCKET_BUFFER_SIZE];
		message[0] = _getch();

		int flags = 0;
		if (sendto(sock, message, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&server_address, sizeof(server_address)) ==
			SOCKET_ERROR)
		{
			printf("sendto failed: %d", WSAGetLastError());
			return -1;
		}

		SOCKADDR* from = (SOCKADDR *) &server_address;
		int from_size = sizeof(*from);

		int bytes_received = recvfrom(sock, message, SOCKET_BUFFER_SIZE, flags, from, &from_size);
		if (bytes_received == SOCKET_ERROR)
		{
			printf("recvfrom return SOCKET_ERROR, WSAGetLastError() %d", WSAGetLastError());
			break;
		}

		//Prepare client reply
		int write_index = 0;	//Also used as the buffer length
		int x, y, run;
		x = y = run = 0;

		memcpy(&x, &message[write_index], sizeof(int));
		write_index += sizeof(int);

		memcpy(&y, &message[write_index], sizeof(int));
		write_index += sizeof(int);

		memcpy(&run, &message[write_index], sizeof(int));

		printf("Received from server: X:%d, Y:%d, is_running: %d\n", x, y, run);
		if (!run)
		{
			break;
		}
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}