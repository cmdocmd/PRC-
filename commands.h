#pragma once

#include <iostream>

#include "enet/enet.h"
#include "server.h"

void commands(std::string cch, ENetPeer *peer, ENetHost *Host, Server *server);