#pragma once
enum class Client_Message : unsigned char
{
	Join,
	Leave,
	Votekick,
	VoteBan,
	Input
};

enum class Server_Message : unsigned char
{
	Message,
	RemovePlayer, //Serve as kick or ban, will contaiin the reason for kick or ban, and the time, can be permanent.
	JoinResult,
	State
};

enum class Action : unsigned char
{
	Forward,
	Backward,
	Left,
	Right,
	fire
};