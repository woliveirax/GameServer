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

class ThreadManager {
private:
	enum class ThreadIdentity : unsigned int
	{
		mainCommunicationThread = 0,
		keepAliveThread
	};

	HANDLE mainCommunicationThread;
	HANDLE keepAliveThread;
	HANDLE consoleThread;

	void startMainCommsThread()
	{
		mainCommunicationThread = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			,       // thread function name
			NULL,          // argument to thread function 
			0,                      // use default creation flags 
			NULL);   // returns the thread identifier 

		if (mainCommunicationThread == NULL)
		{
			throw ("Failed to start communication thread\n");
		}
	}

public:
	ThreadManager()
	{
		mainCommunicationThread = nullptr;
		keepAliveThread = nullptr;
	};

	void startThreads()
	{
		startMainCommsThread();
	};

	void waitForThreads()
	{
		HANDLE threads[2];
		threads[0] = mainCommunicationThread;
		threads[1] = keepAliveThread;

		WaitForMultipleObjects(2, threads, true, INFINITE);
	};

	~ThreadManager()
	{
		CloseHandle(mainCommunicationThread);
		CloseHandle(keepAliveThread);
	};
};

class ClientManager {
private:
	std::unique_ptr<GameClient[]> clients;

public:
	ClientManager() {
		clients = std::unique_ptr<GameClient[]>(new GameClient[ServerSettings::MAX_PLAYERS]);
	}

	unsigned int AddClient(const int ip, const unsigned short port);
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

	~CommunicationManager()
	{
		closesocket(sock);
		WSACleanup();
	}

	static void sendDataSingleClient(const char* &buffer);
	static void broadcastData(const char* &buffer);
	static void recvData(char* &buffer, unsigned int &bytes_received);
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

LPVOID CommunicationHandler(LPVOID client_manager);
