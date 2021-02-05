#pragma once
struct IP
{
	unsigned int address;
	unsigned short port;
};

struct PlayerState
{
	float x, y, facing, speed;
};

struct PlayerInput
{
	int up, down, left, right;
};