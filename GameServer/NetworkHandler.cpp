#pragma once
#include "NetworkHandler.h"

SOCKET GetSocket(int shouldbind, int bindport)
{
	//Socket protocol initialization
	WORD winsock_ver = 0x202;
	WSADATA winsock_data;

	if (WSAStartup(winsock_ver, &winsock_data))
	{
		printf("WSAStartup failed: %d", WSAGetLastError());
		return INVALID_SOCKET;
	}

	//Defining packets
	int address_family = AF_INET;
	int type = SOCK_DGRAM;
	int protocol = IPPROTO_UDP;
	SOCKET sock = socket(address_family, type, protocol);

	if (sock == INVALID_SOCKET)
	{
		printf("Socket creation failed: %d", WSAGetLastError());
		return INVALID_SOCKET;
	}

	if (shouldbind)
	{
		//Bind port
		SOCKADDR_IN local_address;
		local_address.sin_family = AF_INET;
		local_address.sin_port = htons(bindport);
		local_address.sin_addr.s_addr = INADDR_ANY;

		if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
		{
			printf("Bind failed: %d", WSAGetLastError());
			return INVALID_SOCKET;
		}
	}

	return sock;
}