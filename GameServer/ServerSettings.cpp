#include "ServerSettings.h"

unsigned char ServerSettings::MAX_PLAYERS = (unsigned char)DefaultServerSettings::DEFAULT_MAX_PLAYERS;
unsigned char ServerSettings::TICK_RATE = (unsigned char)DefaultServerSettings::DEFAULT_TICK_RATE;
unsigned short ServerSettings::PORT = (unsigned short)DefaultServerSettings::DEFAULT_PORT;
float ServerSettings::SECONDS_PER_TICK = 1.0f / float(ServerSettings::TICK_RATE);