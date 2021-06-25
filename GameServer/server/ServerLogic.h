#pragma once
#include "NetworkHandler.h"
#include "ServerSettings.h"
#include "ServerInformation.h"
#include <timeapi.h>

struct SharedInfo
{
	int is_running;
	SOCKET sock;

	IP* client_address;
	PlayerState* player_state;
	PlayerInput* player_input;
	float* last_response;
};

int InitServer(SharedInfo* info);
void ShutdownServer(SharedInfo* info);
void CommunicationHandler(void* info_ptr);
