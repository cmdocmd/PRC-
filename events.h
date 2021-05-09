#pragma once
#include <iostream>
#include "enet/enet.h"

void Connect(ENetPeer *peer, ENetHost *Host);
void Recieve(Server *server, ENetHost *Host, ENetPacket *packet, ENetPeer *peer, std::string cch);
void Disconnect(ENetHost *Host, ENetPeer *peer);