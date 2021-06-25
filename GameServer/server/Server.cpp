#pragma once
#include "Server.h"

//Wait for threads to finish, release memory and resources.
void Shutdown()
{
	info-> = false;
	//TODO: wait for threads if there's any

	//TODO: Release and close thread handles

	//TODO: Release and close sync resources
}

//used to calculate time since last tick
float time_since(const LARGE_INTEGER& t, const LARGE_INTEGER& frequency)
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	return float(now.QuadPart - t.QuadPart) / float(frequency.QuadPart);
}

//Function to sync tickrate
void waitForTick(const bool& sleep_granularity_was_set, const LARGE_INTEGER& tick_start, const LARGE_INTEGER& frequency)
{
	float time_taken_s = time_since(tick_start, frequency);

	while (time_taken_s < ServerSettings::SECONDS_PER_TICK)
	{
		if (sleep_granularity_was_set)
		{
			DWORD time_to_wait_ms = DWORD((ServerSettings::SECONDS_PER_TICK - time_taken_s) * 1000);
			if (time_to_wait_ms > 0)
			{
				Sleep(time_to_wait_ms);
			}
		}

		time_taken_s = time_since(tick_start, frequency);
	}
}

void HandleClientInput(const char& client_input, SharedInfo* const info)
{
	switch (client_input)
	{
		//TODO: Replace ascii codes by enumerators instead
	case 'w':
		info->player_state[0].y++;
		break;

	case 'a':
		info->player_state[0].x--;
		break;

	case 's':
		info->player_state[0].y--;
		break;

	case 'd':
		info->player_state[0].x++;
		break;

	case 'q':
		info->run = 0;
		break;

	default:
		printf("unhandled input: %c \n", client_input);
	}
}

void ThreadManager::CommunicationHandler()
{
	Server& server = Server::getInstance();
	char buffer[SOCKET_BUFFER_SIZE];

	UINT sleep_glanularity_ms = 1;
	bool sleep_glanularity_set = timeBeginPeriod(sleep_glanularity_ms) == TIMERR_NOERROR;

	LARGE_INTEGER clock_frequency;
	QueryPerformanceFrequency(&clock_frequency);

	while (server.run)
	{
		//Obtain start time of tick, to synchronize tick rate at the end of the cycle
		LARGE_INTEGER tick_start_time;
		QueryPerformanceCounter(&tick_start_time);

		//Receive message from client
		int flags = 0;
		SOCKADDR_IN from;
		int from_size = sizeof(from);

		int bytes_received = recvfrom(info->sock, buffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);
		if (bytes_received == SOCKET_ERROR)
		{
			printf("recvfrom return SOCKET_ERROR, WSAGetLastError() %d", WSAGetLastError());
			break;
		}

		printf("%d.%d.%d.%d:%d - %d\n", from.sin_addr.S_un.S_un_b.s_b1, from.sin_addr.S_un.S_un_b.s_b2,
			from.sin_addr.S_un.S_un_b.s_b3, from.sin_addr.S_un.S_un_b.s_b4, ntohs(from.sin_port), buffer[0]);

		char client_input = buffer[0];

		//TODO: HandlePacket
		HandleClientInput(client_input, info);


		//TODO: Move reply to handle package code
		//Client Reply
		int write_index = 0;	//Also used as the buffer length

		memcpy(&buffer[write_index], &info->player_state[0].x, sizeof(info->player_state[0].x));
		write_index += sizeof(info->player_state[0].x);

		memcpy(&buffer[write_index], &info->player_state[0].y, sizeof(info->player_state[0].y));
		write_index += sizeof(info->player_state[0].y);

		memcpy(&buffer[write_index], &info->run, sizeof(info->run));
		write_index += sizeof(info->run);

		//Reply to client
		flags = 0;
		SOCKADDR* to = (SOCKADDR*)&from;
		int to_length = sizeof(from);

		if (sendto(info->sock, buffer, write_index, flags, to, to_length) == SOCKET_ERROR)
		{
			printf("failed to send packet: %d", WSAGetLastError());
			break;
		}

		waitForTick(sleep_glanularity_set, tick_start_time, clock_frequency);
	}
}