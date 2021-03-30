#pragma once
#include <WinSock2.h>
#include <stdio.h>
#include "ServerSettings.h"

SOCKET GetSocket(bool shouldbind = true, int bindport = 9999);