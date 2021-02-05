#pragma once

enum class DefaultServerSettings : unsigned short
{
	DEFAULT_MAX_PLAYERS = 32,
	DEFAULT_TICK_RATE = 60, //Hz
	DEFAULT_PORT = 9999,
};

class ServerSettings
{
public:
	static unsigned char MAX_PLAYERS;
	static unsigned char TICK_RATE;
	static unsigned short PORT;
};

const unsigned int SOCKET_BUFFER_SIZE = 1024;