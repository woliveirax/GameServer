#pragma once
#include "ServerLogic.h"

//Wait for threads to finish, release memory and resources.
void ShutdownServer(SharedInfo* info)
{
	info->is_running = 0;
	//TODO: wait for threads if there's any

	//TODO: Release and close thread handles

	//TODO: Release and close sync resources

	//Free client related data structures
	free(info->player_state);
	free(info->player_input);
	free(info->last_response);
	free(info->client_address);

	//Free socket resources
	closesocket(info->sock);
	WSACleanup();
}

//Init server and allocate memory
int InitServer(SharedInfo* info)
{
	*info = {};

	info->sock = GetSocket(1, ServerSettings::PORT);
	if (info->sock == INVALID_SOCKET)
	{
		printf("failed to start socket: %d", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	info->client_address = (IP*)malloc(sizeof(IP) * ServerSettings::MAX_PLAYERS);
	if (info->client_address == NULL)
	{
		printf("failed to allocate space for clients addresses\n");
		ShutdownServer(info);
		return -1;
	}

	info->last_response = (float*)malloc(sizeof(float) * ServerSettings::MAX_PLAYERS);
	if (info->last_response == NULL)
	{
		printf("failed to allocate space for client last reponse time\n");
		ShutdownServer(info);
		return -1;
	}

	info->player_input = (PlayerInput*)malloc(sizeof(PlayerInput) * ServerSettings::MAX_PLAYERS);
	if (info->player_input == NULL)
	{
		printf("failed to allocate space for player input data\n");
		ShutdownServer(info);
		return -1;
	}

	info->player_state = (PlayerState*)malloc(sizeof(PlayerState) * ServerSettings::MAX_PLAYERS);
	if (info->player_state == NULL)
	{
		printf("failed to allocate space for player state data.\n");
		ShutdownServer(info);
		return -1;
	}
	info->is_running = 1;

	return 0;
}

//Calculate time since last tick
float time_since(const LARGE_INTEGER& t, const LARGE_INTEGER& frequency)
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	return float(now.QuadPart - t.QuadPart) / float(frequency.QuadPart);
}

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
		info->is_running = 0;
		break;

	default:
		printf("unhandled input: %c \n", client_input);
	}
}

void CommunicationHandler(void* info_ptr)
{
	SharedInfo* info = (SharedInfo*)info_ptr;
	char buffer[SOCKET_BUFFER_SIZE];

	//TODO: Init timer here
	UINT sleep_glanularity_ms = 1;
	bool sleep_glanularity_set = timeBeginPeriod(sleep_glanularity_ms) == TIMERR_NOERROR;

	LARGE_INTEGER clock_frequency;
	QueryPerformanceFrequency(&clock_frequency);

	while (info->is_running)
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

		//TODO: Change this later
		if (bytes_received > SOCKET_BUFFER_SIZE)
			buffer[bytes_received] = '\0';
		else
			buffer[SOCKET_BUFFER_SIZE - 1] = '\0';

		printf("%d.%d.%d.%d:%d - %d\n", from.sin_addr.S_un.S_un_b.s_b1, from.sin_addr.S_un.S_un_b.s_b2,
			from.sin_addr.S_un.S_un_b.s_b3, from.sin_addr.S_un.S_un_b.s_b4, ntohs(from.sin_port), buffer[0]);

		char client_input = buffer[0];
		HandleClientInput(client_input, info);

		//Prepare client reply
		int write_index = 0;	//Also used as the buffer length

		memcpy(&buffer[write_index], &info->player_state[0].x, sizeof(info->player_state[0].x));
		write_index += sizeof(info->player_state[0].x);

		memcpy(&buffer[write_index], &info->player_state[0].y, sizeof(info->player_state[0].y));
		write_index += sizeof(info->player_state[0].y);

		memcpy(&buffer[write_index], &info->is_running, sizeof(info->is_running));
		write_index += sizeof(info->is_running);

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