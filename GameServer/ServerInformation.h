#pragma once
class Client
{
public:
	unsigned int address;
	unsigned short port;
	float last_response;

	Client() : address(0), port(0), last_response(0) {};

	void updateLastResponse();
};

class GameClient : Client
{
public:
	char username[MAX_USERNAME_SIZE];

	//Player status
	float x;
	float y;
	float facing;
	float speed;

	//Action
	bool up, down, left, right;

	GameClient()
		: username()
	{
		x = y = facing = speed = 0;
		up = down = left = right = false;
	}
};
