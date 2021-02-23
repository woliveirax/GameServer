#include "ServerLogic.h"

int main(int argc, char** argv)
{
	printf("Server is starting...\n");
	//TODO: Load server settings

	//Init structures
	SharedInfo info = {};

	InitServer(&info);
	if (!info.is_running)
	{
		printf("Failed to start server\n");
		return -1;
	}

	printf("Server is running...\n");

	//TODO: Start thread, replace this for thread call
	CommunicationHandler(&info);

	//TODO: Server Console  ( maybe add to a separate thread )
	//TODO: Add thread join


	ShutdownServer(&info);
	printf("Closing Server\n");

	return 0;
}