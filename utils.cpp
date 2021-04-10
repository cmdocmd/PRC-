#pragma once
#include <iostream>
#include "enet/include/enet/enet.h"
#include <string.h>
#include "player.cpp"
#include "GamePacketBuilder.cpp"
#include <memory>
#include <bits/stdc++.h>

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
    std::vector<BYTE> data(56);
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
        if (a1 == 4 && *(&packetData[12]) & 8)
        {
            p = enet_packet_create(0, packetDataSize + *(((DWORD *)packetData.data()) + 13) + 5, packetFlag);
            int four = 4;
            memcpy(p->data, &four, 4);
            memcpy((char *)p->data + 4, &packetData[0], packetDataSize);
            //  memcpy((char *)p->data + packetDataSize + 4, a4, *(((DWORD *)packetData.data()) + 13));
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

bool isHere(ENetPeer *peer, ENetPeer *peer2)
{
    return pinfo(peer)->currentWorld == pinfo(peer2)->currentWorld;
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
            if (isHere(peer, currentPeer))
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

bool LEGAL_ITEM(ENetPeer *peer, int id)
{
    bool legal = false;
    for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
    {
        if (pinfo(peer)->inv[i].itemID == id)
        {
            legal = true;
        }
    }
    return legal;
}

void Respawn(ENetPeer *peer, bool die)
{
    int netid = pinfo(peer)->netID;
    if (die == false)
    {
        Packets::onkill(peer);
    }
    GamePacket p2x = GamePacketBuilder()
                         .appendString("OnSetFreezeState")
                         .appendInt(0)
                         .build();
    memcpy(p2x.data + 8, &netid, 4);
    int respawnTimeout = 2000;
    int deathFlag = 0x19;
    memcpy(p2x.data + 24, &respawnTimeout, 4);
    memcpy(p2x.data + 56, &deathFlag, 4);
    p2x.send(peer);
    Packets::onfreezestate(peer, 2);
    GamePacket p2 = GamePacketBuilder()
                        .appendString("OnSetPos")
                        .appendFloat(pinfo(peer)->cpx, pinfo(peer)->cpy)
                        .build();
    memcpy(p2.data + 8, &(pinfo(peer)->netID), 4);
    respawnTimeout = 2000;
    memcpy(p2.data + 24, &respawnTimeout, 4);
    memcpy(p2.data + 56, &deathFlag, 4);
    p2.send(peer);
    GamePacket p2a = GamePacketBuilder()
                         .appendString("OnPlayPositioned")
                         .appendString("audio/teleport.wav")
                         .build();
    memcpy(p2a.data + 8, &netid, 4);
    respawnTimeout = 2000;
    memcpy(p2a.data + 24, &respawnTimeout, 4);
    memcpy(p2a.data + 56, &deathFlag, 4);
    p2a.send(peer);
}

int getState(player *info)
{
    int val = 0;
    val |= info->canWalkInBlocks << 0;
    val |= info->canDoubleJump << 1;
    val |= info->isInvisible << 2;
    val |= info->noHands << 3;
    val |= info->noEyes << 4;
    val |= info->noBody << 5;
    val |= info->devilHorns << 6;
    val |= info->goldenHalo << 7;
    val |= info->isFrozen << 11;
    val |= info->isCursed << 12;
    val |= info->isDuctaped << 13;
    val |= info->haveCigar << 14;
    val |= info->isShining << 15;
    val |= info->isZombie << 16;
    val |= info->isHitByLava << 17;
    val |= info->haveHauntedShadows << 18;
    val |= info->haveGeigerRadiation << 19;
    val |= info->haveReflector << 20;
    val |= info->isEgged << 21;
    val |= info->havePineappleFloag << 22;
    val |= info->haveFlyingPineapple << 23;
    val |= info->haveSuperSupporterName << 24;
    val |= info->haveSupperPineapple << 25;
    return val;
}

void sendState(ENetHost *server, ENetPeer *peer)
{
    ENetPeer *currentPeer;
    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (isHere(peer, currentPeer))
        {
            PlayerMoving data;
            data.packetType = 0x14;
            data.characterState = 0; // animation
            data.x = 1000;
            data.y = 100;
            data.punchX = 0;
            data.punchY = 0;
            data.XSpeed = 300;
            data.YSpeed = 600;
            data.netID = pinfo(peer)->netID;
            data.plantingTree = getState(pinfo(peer));
            std::vector<BYTE> raw = packPlayerMoving(&data);
            int var = 8421376;
            memcpy(&raw[1], &var, 3);
            char str[(sizeof raw) + 1];
            memcpy(str, &raw[0], sizeof raw);
            str[sizeof raw] = 0;
            SendPacketRaw(4, raw, 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
        }
    }
}

void sendDrop(ENetHost *server, ENetPeer *peer, int netID, int x, int y, int item, int count, BYTE specialEffect)
{
    if (item > static_cast<int>(itemDefs.size()))
    {
        return;
    }
    if (item < 0)
    {
        return;
    }
    ENetPeer *currentPeer;
    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (isHere(peer, currentPeer))
        {
            PlayerMoving data;
            data.packetType = 14;
            data.x = x;
            data.y = y;
            data.netID = netID;
            data.plantingTree = item;
            float val = count; // item count
            BYTE val2 = specialEffect;

            std::vector<BYTE> raw = packPlayerMoving(&data);
            memcpy(&raw[16], &val, 4);
            memcpy(&raw[1], &val2, 1);

            SendPacketRaw(4, raw, 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
        }
    }
}

void sendTake(ENetHost *server, ENetPeer *peer, int netID, float x, float y, int item)
{
    ENetPeer *currentPeer;
    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (isHere(peer, currentPeer))
        {
            PlayerMoving data;
            data.packetType = 14;
            data.x = x;
            data.y = y;
            data.netID = netID;
            data.plantingTree = item;
            SendPacketRaw(4, packPlayerMoving(&data), 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
        }
    }
}

void remove_clothes_if_wearing(ENetPeer *peer, ENetHost *server, int id)
{
    switch (itemDefs[id].clothingType)
    {
    case ClothTypes::BACK:
        if (pinfo(peer)->back == id)
        {
            pinfo(peer)->back = 0;
            pinfo(peer)->canDoubleJump = false;
        }
        sendState(server, peer);
        break;
    case ClothTypes::FACE:
        if (pinfo(peer)->face == id)
        {
            pinfo(peer)->face = 0;
        }
        break;
    case ClothTypes::FEET:
        if (pinfo(peer)->feet == id)
        {
            pinfo(peer)->feet = 0;
        }
        break;
    case ClothTypes::MASK:
        if (pinfo(peer)->mask == id)
        {
            pinfo(peer)->mask = 0;
        }
        break;
    case ClothTypes::HAND:
        if (pinfo(peer)->hand == id)
        {
            pinfo(peer)->hand = 0;
        }
        break;
    case ClothTypes::HAIR:
        if (pinfo(peer)->hair == id)
        {
            pinfo(peer)->hair = 0;
        }
        break;
    case ClothTypes::NECKLACE:
        if (pinfo(peer)->neck == id)
        {
            pinfo(peer)->neck = 0;
        }
        break;
    case ClothTypes::ANCES:
        if (pinfo(peer)->ances == id)
        {
            pinfo(peer)->ances = 0;
        }
        break;
    default:
        break;
    }
}

int getPlayersCountInWorld(ENetHost *server, string name)
{
    int count = 0;
    ENetPeer *currentPeer;
    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (pinfo(currentPeer)->currentWorld == name)
            count++;
    }
    return count;
}

void sendPlayerLeave(ENetHost *server, ENetPeer *peer, player *info)
{
    ENetPeer *currentPeer;
    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (isHere(peer, currentPeer))
        {
            GamePacket p = GamePacketBuilder()
                               .appendString("OnRemove")
                               .appendString("netID|" + std::to_string(info->netID) + "\n")
                               .build();
            p.send(peer);
            p.send(currentPeer);
            if (peer != currentPeer)
                Packets::consoleMessage(currentPeer, "`5<`w" + info->name + "`` left, `w" + std::to_string(getPlayersCountInWorld(server, info->currentWorld) - 1) + "`` others here>``");
        }
    }
}

void sendmusic(ENetPeer *peer, string music)
{
    string text = "action|play_sfx\nfile|audio/" + music + ".wav\ndelayMS|0\n";
    std::vector<BYTE> data(5 + text.length());
    BYTE zero = 0;
    int type = 3;
    memcpy(&data[0], &type, 4);
    memcpy(&data[4], text.c_str(), text.length());
    memcpy(&data[4 + text.length()], &zero, 1);
    ENetPacket *packet2 = enet_packet_create((void *)&data[0],
                                             5 + text.length(),
                                             ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet2);
}

void time_left(ENetPeer *peer, int n, string w)
{
    int day = n / (24 * 3600);
    n = n % (24 * 3600);
    int hour = n / 3600;
    n %= 3600;
    int minutes = n / 60;
    n %= 60;
    int seconds = n;
    Packets::consoleMessage(peer, "`4Sorry this account is " + w + "`o. and will be removed in `w" + std::to_string(day) + " days, " + std::to_string(hour) + " hours, " + std::to_string(minutes) + " mins, " + std::to_string(seconds) + " secs. `ocontact server developer for more info.");
}

void sendWrench(ENetPeer *peer, bool owner, bool mod, bool dev, bool mySelf)
{
    if (dev)
    {
        if (mySelf)
        {
        }
        else
        {
        }
    }
    else if (mod)
    {
        if (mySelf)
        {
        }
        else
        {
        }
    }
    else if (owner)
    {
        if (mySelf)
        {
        }
        else
        {
        }
    }
    else
    {
        if (mySelf)
        {
        }
        else
        {
        }
    }
}
  