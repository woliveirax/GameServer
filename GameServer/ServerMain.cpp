#include "NetworkHandler.h"
#include "ServerSettings.h"
#include "ServerInformation.h"

struct SharedInfo
{
	int is_running;
	SOCKET sock;

	IP *client_address;
	PlayerState *player_state;
	PlayerInput *player_input;
	float *last_response;
};


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

int InitServer(SharedInfo * info)
{
	*info = {};

	info->sock = GetSocket(1, ServerSettings::PORT);
	if (info->sock == INVALID_SOCKET)
	{
		printf("failed to start socket: %d", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	info->client_address = (IP*) malloc(sizeof(IP) * ServerSettings::MAX_PLAYERS);
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

void CommunicationHandler(void* info_ptr)
{
	SharedInfo info = *(SharedInfo*)info_ptr;
	char buffer[SOCKET_BUFFER_SIZE];

	while (info.is_running)
	{
		int bytes_received = 0;
		int flags = 0;
		SOCKADDR_IN from;
		int from_size = sizeof(from);

		bytes_received = recvfrom(info.sock, buffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);
		if (bytes_received == SOCKET_ERROR)
		{
			printf("recvfrom return SOCKET_ERROR, WSAGetLastError() %d", WSAGetLastError());
			break;
		}

		char client_input = buffer[0];
		printf("%d.%d.%d.%d:%d - %s", from.sin_addr.S_un.S_un_b.s_b1, from.sin_addr.S_un.S_un_b.s_b2,
			from.sin_addr.S_un.S_un_b.s_b3, from.sin_addr.S_un.S_un_b.s_b4, ntohs(from.sin_port), buffer);
	
		switch (client_input)
		{
			case 'w':
				info.player_state[0].y++;
				break;

			case 'a':
				info.player_state[0].x--;
				break;

			case 's':
				info.player_state[0].y--;
				break;

			case 'd':
				info.player_state[0].x++;
				break;

			case 'q':
				info.is_running = 0;
				break;

			default:
				printf("unhandled input: %c \n", client_input);
		}


	}
}

int main(int argc, char** argv)
{
	//TODO: Load server settings


	//Init structures
	SharedInfo info;

	InitServer(&info);
	if (!info.is_running)
	{
		printf("Failed to start server\n");
		return -1;
	}

	//TODO: Start thread
	CommunicationHandler(&info);

	//TODO: Server Console  ( maybe add to a separate thread )
	//TODO: Add thread wait


	ShutdownServer(&info);
	printf("Closing Server\n");

	return 0;
}