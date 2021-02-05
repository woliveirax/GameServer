#pragma once
#include <WinSock2.h>
#include <stdio.h>
#include "ServerSettings.h"

SOCKET GetSocket(int shouldbind = 1, int bindport = 9999);