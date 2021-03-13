#pragma once
#include <iostream>
#include "enet/include/enet/enet.h"
#include <string.h>

typedef unsigned char BYTE;
typedef unsigned int DWORD;

int GetMessageTypeFromPacket(ENetPacket *packet)
{
    int result;

    if (packet->dataLength > 3u)
    {
        result = *(packet->data);
    }
    else
    {
        std::cout << "Bad packet length, ignoring message" << std::endl;
        result = 0;
    }
    return result;
}

int count_players(ENetHost *server, ENetPeer *peer)
{
    ENetPeer *currentPeer;
    int count = 0;
    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
        {
            continue;
        }
        if (currentPeer->address.host == peer->address.host)
        {
            count++;
        }
    }
    return count;
}

void sendData(ENetHost *server, ENetPeer *peer, int num = 1, char *data = 0, int len = 0)
{
    ENetPacket *packet = enet_packet_create(0, len + 5, ENET_PACKET_FLAG_RELIABLE);
    memcpy(packet->data, &num, 4);
    if (data != NULL)
    {
        memcpy(packet->data + 4, data, len);
    }
    char zero = 0;
    memcpy(packet->data + 4 + len, &zero, 1);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(server);
}

char *GetTextPointerFromPacket(ENetPacket *packet)
{
    char zero = 0;
    memcpy(packet->data + packet->dataLength - 1, &zero, 1);
    return (char *)(packet->data + 4);
}

std::vector<string> explode(const string &delimiter, const string &str)
{
    std::vector<string> arr;

    int strleng = str.length();
    int delleng = delimiter.length();
    if (delleng == 0)
        return arr; //no change

    int i = 0;
    int k = 0;
    while (i < strleng)
    {
        int j = 0;
        while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
            j++;
        if (j == delleng) //found delimiter
        {
            arr.push_back(str.substr(k, i - k));
            i += delleng;
            k = i;
        }
        else
        {
            i++;
        }
    }
    arr.push_back(str.substr(k, i - k));
    return arr;
}

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

BYTE *packPlayerMoving(PlayerMoving *dataStruct)
{
    BYTE *data = new BYTE[64];
    for (int i = 0; i < 64; i++)
    {
        data[i] = 0;
    }
    memcpy(data, &dataStruct->packetType, 4);
    memcpy(data + 4, &dataStruct->netID, 4);
    memcpy(data + 12, &dataStruct->characterState, 4);
    memcpy(data + 20, &dataStruct->plantingTree, 4);
    memcpy(data + 24, &dataStruct->x, 4);
    memcpy(data + 28, &dataStruct->y, 4);
    memcpy(data + 32, &dataStruct->XSpeed, 4);
    memcpy(data + 36, &dataStruct->YSpeed, 4);
    memcpy(data + 44, &dataStruct->punchX, 4);
    memcpy(data + 48, &dataStruct->punchY, 4);
    return data;
}

void SendPacketRaw(int a1, void *packetData, size_t packetDataSize, void *a4, ENetPeer *peer, int packetFlag)
{
    ENetPacket *p;

    if (peer) // check if we have it setup
    {
        if (a1 == 4 && *((BYTE *)packetData + 12) & 8)
        {
            p = enet_packet_create(0, packetDataSize + *((DWORD *)packetData + 13) + 5, packetFlag);
            int four = 4;
            memcpy(p->data, &four, 4);
            memcpy((char *)p->data + 4, packetData, packetDataSize);
            memcpy((char *)p->data + packetDataSize + 4, a4, *((DWORD *)packetData + 13));
            enet_peer_send(peer, 0, p);
        }
        else
        {
            p = enet_packet_create(0, packetDataSize + 5, packetFlag);
            memcpy(p->data, &a1, 4);
            memcpy((char *)p->data + 4, packetData, packetDataSize);
            enet_peer_send(peer, 0, p);
        }
    }
}

void onPeerConnect(ENetHost *server, ENetPeer *peer)
{
    ENetPeer *currentPeer;

    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (peer != currentPeer)
        {
            if (pinfo(peer)->currentWorld == pinfo(currentPeer)->currentWorld)
            {
                string netIdS = std::to_string(pinfo(currentPeer)->netID);
                string userids = std::to_string(pinfo(currentPeer)->userID);
                Packets::onSpawn(peer, "spawn|avatar\nnetID|" + netIdS + "\nuserID|" + userids + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(pinfo(currentPeer)->x) + "|" + std::to_string(pinfo(currentPeer)->y) + "\nname|``" + pinfo(currentPeer)->name + "``\ncountry|" + pinfo(currentPeer)->country + "\ninvis|0\nmstate|0\nsmstate|0\n");
                string netIdS2 = std::to_string(pinfo(peer)->netID);
                string userid = std::to_string(pinfo(peer)->userID);
                Packets::onSpawn(currentPeer, "spawn|avatar\nnetID|" + netIdS2 + "\nuserID|" + userid + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(pinfo(peer)->x) + "|" + std::to_string(pinfo(peer)->y) + "\nname|``" + pinfo(peer)->name + "``\ncountry|" + pinfo(peer)->country + "\ninvis|0\nmstate|0\nsmstate|0\n");
            }
        }
    }
}
