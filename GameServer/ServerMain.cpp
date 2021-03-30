#include "Server.h"

int main(int argc, char** argv)
{
	//TODO: Load server settings
	Server& server = Server::getInstance();

	server.start();

	return 0;
}