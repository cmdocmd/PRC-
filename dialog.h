#pragma once

#include <iostream>

#include "enet/enet.h"
#include "server.h"

void dialog(Server *server, std::string cch, ENetPeer *peer, ENetHost *Host);