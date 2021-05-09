#pragma once

#include <iostream>
#include <vector>
#include "enet/enet.h"
#include "players.h"

typedef unsigned char BYTE;

struct PlayerMoving
{
    int packetType;
    int netID;
    float x;
    float y;
    int characterState;
    int plantingTree;
    float XSpeed;
    float YSpeed;
    int punchX;
    int punchY;
    int secondnetID;
};

char *get_text(ENetPacket *packet);
int GetMessageTypeFromPacket(ENetPacket *packet);
int count_players(ENetHost *Host, ENetPeer *peer);
void sendData(ENetHost *server, ENetPeer *peer);
std::vector<ENetPeer *> isHere(ENetHost *Host, ENetPeer *peer, bool me);
std::vector<ENetPeer*> All(ENetHost *Host);
std::vector<ENetPeer *> Mods(ENetHost *Host);
ENetPeer * player(ENetHost *Host, std::string username);
std::vector<std::string> explode(const std::string &delimiter, const std::string &str);
void onPeerConnect(ENetHost *Host, ENetPeer *peer);
bool is_digits(const std::string &str);
void sendInventory(ENetPeer *peer);
std::vector<BYTE> packPlayerMoving(PlayerMoving *dataStruct);
PlayerMoving *unpackPlayerMoving(BYTE *data);
void SendPacketRaw(int a1, std::vector<BYTE> packetData, size_t packetDataSize, void *a4, ENetPeer *peer, int packetFlag);
BYTE *GetStructPointerFromTankPacket(ENetPacket *packet);
void sendPData(ENetHost *Host, ENetPeer *peer, PlayerMoving *data);
void updateAllClothes(ENetHost *Host, ENetPeer *peer);
int getState(Players *info);
void sendState(ENetHost *Host, ENetPeer *peer);
