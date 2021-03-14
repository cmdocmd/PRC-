#pragma once
#include <iostream>
#include "enet/include/enet/enet.h"
#include <string.h>
#include "player.cpp"
#include "GamePacketBuilder.cpp"

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

std::vector<BYTE> packPlayerMoving(PlayerMoving *dataStruct)
{
    std::vector<BYTE> data(64);
    memcpy(&data[0], &dataStruct->packetType, 4);
    memcpy(&data[4], &dataStruct->netID, 4);
    memcpy(&data[12], &dataStruct->characterState, 4);
    memcpy(&data[20], &dataStruct->plantingTree, 4);
    memcpy(&data[24], &dataStruct->x, 4);
    memcpy(&data[28], &dataStruct->y, 4);
    memcpy(&data[32], &dataStruct->XSpeed, 4);
    memcpy(&data[36], &dataStruct->YSpeed, 4);
    memcpy(&data[44], &dataStruct->punchX, 4);
    memcpy(&data[48], &dataStruct->punchY, 4);
    return data;
}

PlayerMoving *unpackPlayerMoving(BYTE *data)
{
    PlayerMoving *dataStruct = new PlayerMoving;
    dataStruct->packetType = *(int *)(data);
    dataStruct->netID = *(int *)(data + 4);
    dataStruct->characterState = *(int *)(data + 12);
    dataStruct->plantingTree = *(int *)(data + 20);
    dataStruct->x = *(float *)(data + 24);
    dataStruct->y = *(float *)(data + 28);
    dataStruct->XSpeed = *(float *)(data + 32);
    dataStruct->YSpeed = *(float *)(data + 36);
    dataStruct->punchX = *(int *)(data + 44);
    dataStruct->punchY = *(int *)(data + 48);
    return dataStruct;
}

void SendPacketRaw(int a1, std::vector<BYTE> packetData, size_t packetDataSize, void *a4, ENetPeer *peer, int packetFlag)
{
    ENetPacket *p;

    if (peer) // check if we have it setup
    {
        if (a1 == 4 && *((BYTE *)&packetData[12]) & 8)
        {
            p = enet_packet_create(0, packetDataSize + *((DWORD *)&packetData[13]) + 5, packetFlag);
            int four = 4;
            memcpy(p->data, &four, 4);
            memcpy((char *)p->data + 4, &packetData[0], packetDataSize);
            memcpy((char *)p->data + packetDataSize + 4, a4, *((DWORD *)&packetData[13]));
            enet_peer_send(peer, 0, p);
        }
        else
        {
            p = enet_packet_create(0, packetDataSize + 5, packetFlag);
            memcpy(p->data, &a1, 4);
            memcpy((char *)p->data + 4, &packetData[0], packetDataSize);
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

BYTE *GetStructPointerFromTankPacket(ENetPacket *packet)
{
    unsigned int packetLenght = packet->dataLength;
    BYTE *result = NULL;
    if (packetLenght >= 0x3C)
    {
        BYTE *packetData = packet->data;
        result = packetData + 4;
        if (*(BYTE *)(packetData + 16) & 8)
        {
            if ((int)packetLenght < *(int *)(packetData + 56) + 60)
            {
                std::cout << "Packet too small for extended packet to be valid" << std::endl;
                std::cout << "Sizeof float is 4.  TankUpdatePacket size: 56" << std::endl;
                result = 0;
            }
        }
        else
        {
            int zero = 0;
            memcpy(packetData + 56, &zero, 4);
        }
    }
    return result;
}
void sendPData(ENetHost *server, ENetPeer *peer, PlayerMoving *data)
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
                data->netID = pinfo(peer)->netID;

                SendPacketRaw(4, packPlayerMoving(data), 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
            }
        }
    }
}
void updateAllClothes(ENetHost *server, ENetPeer *peer)
{
    ENetPeer *currentPeer;
    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (pinfo(peer)->currentWorld == pinfo(currentPeer)->currentWorld)
        {
            GamePacket p3 = GamePacketBuilder()
                                .appendString("OnSetClothing")
                                .appendFloat(pinfo(peer)->hair, pinfo(peer)->shirt, pinfo(peer)->pants)
                                .appendFloat(pinfo(peer)->feet, pinfo(peer)->face, pinfo(peer)->hand)
                                .appendFloat(pinfo(peer)->back, pinfo(peer)->mask, (pinfo(peer))->neck)
                                .appendIntx(pinfo(peer)->skinColor)
                                .appendFloat(pinfo(peer)->ances, 0.0f, 0.0f)
                                .build();
            memcpy(p3.data + 8, &((pinfo(peer))->netID), 4);
            p3.send(currentPeer);

            GamePacket p4 = GamePacketBuilder()
                                .appendString("OnSetClothing")
                                .appendFloat(pinfo(currentPeer)->hair, pinfo(currentPeer)->shirt, pinfo(currentPeer)->pants)
                                .appendFloat(pinfo(currentPeer)->feet, pinfo(currentPeer)->face, pinfo(currentPeer)->hand)
                                .appendFloat(pinfo(currentPeer)->back, pinfo(currentPeer)->mask, (pinfo(currentPeer))->neck)
                                .appendIntx(pinfo(currentPeer)->skinColor)
                                .appendFloat(pinfo(currentPeer)->ances, 0.0f, 0.0f)
                                .build();
            memcpy(p4.data + 8, &((pinfo(currentPeer))->netID), 4);
            p4.send(peer);
        }
    }
}

static inline void ltrim(string &s)
{
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](int ch) {
                return !isspace(ch);
            }));
}

static inline void rtrim(string &s)
{
    s.erase(find_if(s.rbegin(), s.rend(), [](int ch) {
                return !isspace(ch);
            }).base(),
            s.end());
}

static inline void trim(string &s)
{
    ltrim(s);
    rtrim(s);
}

static inline string trimString(string s)
{
    trim(s);
    return s;
}

int countSpaces(string &str)
{
    int count = 0;
    int length = str.length();
    for (int i = 0; i < length; i++)
    {
        int c = str[i];
        if (isspace(c))
            count++;
    }
    return count;
}

ulong _byteswap_ulong(ulong x)
{
    x = ((x & 0xFFFF0000FFFF0000) >> 16) | ((x & 0x0000FFFF0000FFFF) << 16);
    return ((x & 0xFF00FF00FF00FF00) >> 8) | ((x & 0x00FF00FF00FF00FF) << 8);
}

void sendInventory(ENetPeer *peer)
{
    int inventoryLen = pinfo(peer)->inv.size();
    int packetLen = 66 + (inventoryLen * 4) + 4;
    std::vector<BYTE> data2(packetLen);
    int MessageType = 0x4;
    int PacketType = 0x9;
    int NetID = -1;
    int CharState = 0x8;

    memcpy(&data2[0], &MessageType, 4);
    memcpy(&data2[4], &PacketType, 4);
    memcpy(&data2[8], &NetID, 4);
    memcpy(&data2[16], &CharState, 4);
    int endianInvVal = _byteswap_ulong(inventoryLen);
    memcpy(&data2[66 - 4], &endianInvVal, 4);
    endianInvVal = _byteswap_ulong(pinfo(peer)->invSize);
    memcpy(&data2[66 - 8], &endianInvVal, 4);
    int val = 0;
    for (int i = 0; i < inventoryLen; i++)
    {
        val = 0;
        val |= pinfo(peer)->inv[i].itemID;
        val |= pinfo(peer)->inv[i].itemCount << 16;
        val &= 0x00FFFFFF;
        val |= 0x00 << 24;
        memcpy(&data2[(i * 4) + 66], &val, 4);
    }
    ENetPacket *packet3 = enet_packet_create((void *)&data2[0],
                                             packetLen,
                                             ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet3);
}

void Add_ITEM(ENetPeer *peer, int id, int count, bool send = false)
{
    PlayerInventory inv;
    inv.itemID = id;
    inv.itemCount = count;
    pinfo(peer)->inv.push_back(inv);
    if (send)
    {
        sendInventory(peer);
    }
}