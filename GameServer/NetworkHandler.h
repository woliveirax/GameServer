//This file might as well become a dll in the future

#pragma once
#include <WinSock2.h>
#include <stdio.h>

#define PORT 9999
#define SOCKET_BUFFER_SIZE 1024

SOCKET GetSocket(int shouldbind = 1, int bindport = PORT);