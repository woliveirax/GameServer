#pragma once
#include "NetworkHandler.h"
#include "ServerSettings.h"
#include "ServerInformation.h"
#include <timeapi.h>
#include <memory>

#include <mutex>
class Synchronization
{

public:
	std::mutex clientManagementMutex;
	std::mutex socketManagementMutex;
	std::mutex consoleWriteMutex;
};


#include <thread>
class ThreadManager {

private:
	std::thread mainCommunicationThread;
	std::thread keepAliveThread;

	void CommunicationHandler();

	void startMainCommsThread()
	{
		mainCommunicationThread = std::thread(&CommunicationHandler);
	}

public:
	void startThreads()
	{
		startMainCommsThread();
	};

	void waitForThreads()
	{
		mainCommunicationThread.join();
		keepAliveThread.join();
	};
};

class ClientManager {
private:
	std::unique_ptr<GameClient[]> clients;

public:
	ClientManager() {
		clients = std::unique_ptr<GameClient[]>(new GameClient[ServerSettings::MAX_PLAYERS]);
	}

	unsigned int AddClient(const unsigned int ip, const unsigned short port);
	Client& GetClient(const int id);
	void RemoveClient(const int id);
};

class CommunicationManager
{
private:
	SOCKET sock;

public:
	CommunicationManager()
	{
		sock = GetSocket(1, ServerSettings::PORT);
		if (sock == INVALID_SOCKET)
		{
			WSACleanup();
			throw ("failed to start socket: %d", WSAGetLastError());
		}
	}

	void sendDataSingleClient(const char* &buffer);
	void broadcastData(const char* &buffer);
	void recvData(char* &buffer, unsigned int &bytes_received);

	~CommunicationManager()
	{
		closesocket(sock);
		WSACleanup();
	}
};

class Server
{

private:
	Server()
	{
		run = true;
	};

public:
	bool run;
	CommunicationManager comms;
	Synchronization sync;

	ThreadManager threads;
	ClientManager clients;

	static Server& getInstance() {
		static Server instance;
		return instance;
	}

	void start();
	void stop();

	Server(const Server& other) = delete;
	Server& operator=(const Server&& other) = delete;
	Server& operator=(const Server& other) = delete;
};
