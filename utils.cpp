#include <iostream>
#include <string.h>
#include <vector>

#include "enet/enet.h"
#include "players.h"
#include "packets.h"
#include "utils.h"
#include "items.h"
#include "mysql.h"

typedef unsigned char BYTE;
typedef unsigned int DWORD;

#define STRINT(x, y) (*(int *)(&(x)[(y)]))
#define STR16(x, y) (*(uint16_t *)(&(x)[(y)]))

int gem_event = 0;

std::vector<int> cant_take{2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30}; //store items...

char *get_text(ENetPacket *packet)
{
    char zero = 0;
    memcpy(packet->data + packet->dataLength - 1, &zero, 1);
    return (char *)(packet->data + 4);
}

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

int count_players(ENetHost *Host, ENetPeer *peer)
{
    ENetPeer *currentPeer;
    int count = 0;
    for (currentPeer = Host->peers; currentPeer < &Host->peers[Host->peerCount]; ++currentPeer)
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

void sendData(ENetHost *Host, ENetPeer *peer)
{
    int num = 1, len = 0;
    char *data = 0;
    ENetPacket *packet = enet_packet_create(0, len + 5, ENET_PACKET_FLAG_RELIABLE);
    memcpy(packet->data, &num, 4);
    if (data != NULL)
    {
        memcpy(packet->data + 4, data, len);
    }
    char zero = 0;
    memcpy(packet->data + 4 + len, &zero, 1);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(Host);
}

std::vector<ENetPeer *> isHere(ENetHost *Host, ENetPeer *peer, bool me = true)
{
    ENetPeer *currentPeer;
    std::vector<ENetPeer *> here;
    if (me)
    {
        for (currentPeer = Host->peers; currentPeer < &Host->peers[Host->peerCount]; ++currentPeer)
        {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                continue;
            if (pinfo(peer)->currentWorld == pinfo(currentPeer)->currentWorld)
            {
                here.push_back(currentPeer);
            }
        }
    }
    else
    {
        for (currentPeer = Host->peers; currentPeer < &Host->peers[Host->peerCount]; ++currentPeer)
        {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                continue;
            if (peer != currentPeer)
            {
                if (pinfo(peer)->currentWorld == pinfo(currentPeer)->currentWorld)
                {
                    here.push_back(currentPeer);
                }
            }
        }
    }
    return here;
}

std::vector<ENetPeer *> All(ENetHost *Host)
{
    ENetPeer *currentPeer;
    std::vector<ENetPeer *> all;
    for (currentPeer = Host->peers; currentPeer < &Host->peers[Host->peerCount]; ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        all.push_back(currentPeer);
    }
    return all;
}

std::vector<ENetPeer *> Mods(ENetHost *Host)
{
    ENetPeer *currentPeer;
    std::vector<ENetPeer *> mods;
    for (currentPeer = Host->peers; currentPeer < &Host->peers[Host->peerCount]; ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (pinfo(currentPeer)->adminLevel >= 666)
        {
            mods.push_back(currentPeer);
        }
    }
    return mods;
}

ENetPeer *player(ENetHost *Host, std::string username)
{
    ENetPeer *currentPeer;
    for (currentPeer = Host->peers; currentPeer < &Host->peers[Host->peerCount]; ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (pinfo(currentPeer)->username == username)
        {
            return currentPeer;
        }
    }
    return NULL;
}

std::vector<std::string> explode(const std::string &delimiter, const std::string &str)
{
    std::vector<std::string> arr;

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

void onPeerConnect(ENetHost *Host, ENetPeer *peer)
{
    Packets p;
    std::vector<ENetPeer *> here = isHere(Host, peer, false);
    for (ENetPeer *currentPeer : here)
    {
        std::string netIdS = std::to_string(pinfo(currentPeer)->netID);
        std::string userids = std::to_string(pinfo(currentPeer)->userID);
        std::string netIdS2 = std::to_string(pinfo(peer)->netID);
        std::string userid = std::to_string(pinfo(peer)->userID);

        p.onSpawn(peer, "spawn|avatar\nnetID|" + netIdS + "\nuserID|" + userids + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(pinfo(currentPeer)->x) + "|" + std::to_string(pinfo(currentPeer)->y) + "\nname|``" + pinfo(currentPeer)->name + "``\ncountry|" + pinfo(currentPeer)->country + "\ninvis|0\nmstate|0\nsmstate|0\n");

        p.onSpawn(currentPeer, "spawn|avatar\nnetID|" + netIdS2 + "\nuserID|" + userid + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(pinfo(peer)->x) + "|" + std::to_string(pinfo(peer)->y) + "\nname|``" + pinfo(peer)->name + "``\ncountry|" + pinfo(peer)->country + "\ninvis|0\nmstate|0\nsmstate|0\n");
    }
}

bool is_digits(const std::string &str)
{
    if (str == "")
    {
        return false;
    }
    return str.find_first_not_of("0123456789") == std::string::npos;
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
                std::cout << "Packet too small for extended packet to be valid"
                          << "\n";
                std::cout << "Sizeof float is 4.  TankUpdatePacket size: 56"
                          << "\n";
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

void sendPData(ENetHost *Host, ENetPeer *peer, PlayerMoving *data)
{
    std::vector<ENetPeer *> ishere = isHere(Host, peer, false);
    for (ENetPeer *currentPeer : ishere)
    {
        data->netID = pinfo(peer)->netID;
        SendPacketRaw(4, packPlayerMoving(data), 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
    }
}

void updateAllClothes(ENetHost *Host, ENetPeer *peer)
{
    std::vector<ENetPeer *> ishere = isHere(Host, peer);
    for (ENetPeer *currentPeer : ishere)
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

int getState(Players *info)
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

void sendState(ENetHost *Host, ENetPeer *peer)
{
    std::vector<ENetPeer *> ishere = isHere(Host, peer);
    for (ENetPeer *currentPeer : ishere)
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
        int var = pinfo(peer)->effect;
        memcpy(&raw[1], &var, 3);
        char str[(sizeof raw) + 1];
        memcpy(str, &raw[0], sizeof raw);
        str[sizeof raw] = 0;
        SendPacketRaw(4, raw, 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);

        data.packetType = 0x14;
        data.characterState = 0; // animation
        data.x = 1000;
        data.y = 100;
        data.punchX = 0;
        data.punchY = 0;
        data.XSpeed = 300;
        data.YSpeed = 600;
        data.netID = pinfo(currentPeer)->netID;
        data.plantingTree = getState(pinfo(currentPeer));
        std::vector<BYTE> raw1 = packPlayerMoving(&data);
        int var1 = 8421376;
        memcpy(&raw1[1], &var1, 3);
        char str1[(sizeof raw1) + 1];
        memcpy(str1, &raw1[0], sizeof raw1);
        str1[sizeof raw1] = 0;
        SendPacketRaw(4, raw1, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
    }
}

void Add_ITEM(ENetPeer *peer, int id, int count, bool send)
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

void sendDrop(ENetHost *Host, ENetPeer *peer, int netID, int x, int y, int item, int count, BYTE specialEffect)
{
    if (item > static_cast<int>(itemDefs.size()) || item < 0)
    {
        return;
    }
    std::vector<ENetPeer *> ishere = isHere(Host, peer);
    for (ENetPeer *currentPeer : ishere)
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

void sendTake(ENetHost *Host, ENetPeer *peer, int netID, float x, float y, int item)
{
    std::vector<ENetPeer *> ishere = isHere(Host, peer);
    for (ENetPeer *currentPeer : ishere)
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

int get_count(ENetPeer *peer, int id, int add)
{
    int count = 0;
    for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
    {
        if (pinfo(peer)->inv[i].itemID == id)
        {
            count = pinfo(peer)->inv[i].itemCount + add;
        }
    }
    return count;
}

void sendmusic(ENetPeer *peer, std::string music)
{
    std::string text = "action|play_sfx\nfile|audio/" + music + ".wav\ndelayMS|0\n";
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
void updatetree(ENetHost *Host, ENetPeer *peer, int foreground, int x, int y, int background, int fruitCount, int timeIntoGrowth, int blockStateFlags)
{
    int hmm = 8;
    int zero = 0;
    int packetType = 5;
    int yeh = hmm + 3 + 1;
    int idk = 15 + 4;
    int blockState = blockStateFlags;
    int bubble_type = 4;
    int ok = 52 + idk;
    int packetSize = ok + 4;
    int yup = ok - 8 - idk;
    int four = 4;
    int magic = 56;
    int wew = ok + 5 + 4;
    int wow = magic + 4 + 5;

    short fc = (short)fruitCount;
    int time = (int)timeIntoGrowth;
    int text_len = 4;
    int treedata = 0x00020000;
    blockState |= 0x100000;

    std::vector<BYTE> data(packetSize);
    ENetPacket *p = enet_packet_create(0, wew, ENET_PACKET_FLAG_RELIABLE);
    memset(&data[0], 0, packetSize);
    memcpy(&data[0], &packetType, sizeof(int));
    memcpy(&data[yeh], &hmm, sizeof(int));
    memcpy(&data[yup], &x, sizeof(int));
    memcpy(&data[yup + 4], &y, sizeof(int));
    memcpy(&data[4 + yup + 4], &idk, sizeof(int));
    memcpy(&data[magic], &foreground, sizeof(short));
    memcpy(&data[magic + 2], &background, sizeof(short));
    memcpy(&data[magic + 4], &blockState, sizeof(int));
    memcpy(&data[magic + 8], &bubble_type, sizeof(BYTE));
    memcpy(&data[magic + 9 + 0], &time, 4);
    memcpy(&data[magic + 9 + 4], &fc, 1);
    memcpy(&data[ok], &blockState, sizeof(int));
    memcpy(p->data, &four, four);
    memcpy((char *)p->data + four, &data[0], packetSize);

    std::vector<ENetPeer *> ishere = isHere(Host, peer);
    enet_peer_send(peer, 0, p);
}

void sendtrade(ENetHost *Host, ENetPeer *peer, int id, int netIDsrc, int netIDdst, int timeMs)
{
    PlayerMoving data;
    data.packetType = 0x13;
    data.punchX = id;
    data.punchY = id;

    std::vector<BYTE> raw = packPlayerMoving(&data);
    int netIdSrc = netIDsrc;
    int netIdDst = netIDdst;
    int three = 3;
    int n1 = timeMs;
    memcpy(&raw[3], &three, 1);
    memcpy(&raw[4], &netIdDst, 4);
    memcpy(&raw[8], &netIdSrc, 4);
    memcpy(&raw[20], &n1, 4);

    std::vector<ENetPeer *> ishere = isHere(Host, peer);
    for (ENetPeer *currentPeer : ishere)
    {
        std::vector<BYTE> raw2(56);
        memcpy(&raw2[0], &raw[0], 56);
        SendPacketRaw(4, raw2, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
    }
}

void Rem(ENetPeer *peer, int id, int amount, bool send)
{
    for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
    {
        if (pinfo(peer)->inv[i].itemID == id)
        {
            pinfo(peer)->inv[i].itemCount -= amount;
            if (pinfo(peer)->inv[i].itemCount == 0 || pinfo(peer)->inv[i].itemCount < 0)
            {
                pinfo(peer)->inv.erase(pinfo(peer)->inv.begin() + i);
            }
        }
    }
    if (send)
    {
        sendInventory(peer);
    }
}

void Add(ENetPeer *peer, int id, int amount, bool send)
{
    for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
    {
        if (pinfo(peer)->inv[i].itemID == id)
        {
            pinfo(peer)->inv[i].itemCount += amount;
        }
    }
    if (send)
    {
        sendInventory(peer);
    }
}

bool MoreThan200(ENetPeer *peer, int id, int amount)
{
    int idx = -1;
    for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
    {
        if (pinfo(peer)->inv[i].itemID == id)
        {
            idx = i;
        }
    }
    if (idx != -1)
    {
        if (pinfo(peer)->inv[idx].itemCount + amount > 200)
        {
            return true;
        }
    }
    return false;
}

void sendPlayerLeave(ENetHost *Host, ENetPeer *peer)
{
    if (pinfo(peer)->currentWorld != "EXIT")
    {
        Packets s;
        std::vector<ENetPeer *> ishere = isHere(Host, peer);
        for (ENetPeer *currentPeer : ishere)
        {
            GamePacket p = GamePacketBuilder()
                               .appendString("OnRemove")
                               .appendString("netID|" + std::to_string(pinfo(peer)->netID) + "\n")
                               .build();
            p.send(peer);
            p.send(currentPeer);
            if (currentPeer != peer)
            {
                s.consoleMessage(currentPeer, "`5<`w" + pinfo(peer)->name + "`` left, `w" + std::to_string(getPlayersCountInWorld(Host, pinfo(peer)->currentWorld) - 1) + "`` others here>``");
            }
        }
    }
}

void Respawn(ENetPeer *peer, bool die)
{
    Packets p;
    int netid = pinfo(peer)->netID;
    if (die == false)
    {
        p.onkill(peer);
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
    p.onfreezestate(peer, 2);
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

void sendclothes(ENetHost *Host, ENetPeer *peer)
{
    std::vector<ENetPeer *> ishere = isHere(Host, peer);
    for (ENetPeer *currentPeer : ishere)
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
    }
}

void pushsign(Server *server, ENetPeer *peer, std::string text)
{
    Worlds *world = getPlyersWorld(server, peer);
    Signs sin;
    sin.text = text;
    sin.x = pinfo(peer)->wrenchx;
    sin.y = pinfo(peer)->wrenchy;
    world->sign.push_back(sin);
}

void pushmag(Server *server, ENetPeer *peer, int id)
{
    Worlds *world = getPlyersWorld(server, peer);
    Magplant mg;
    mg.x = pinfo(peer)->wrenchx;
    mg.y = pinfo(peer)->wrenchy;
    mg.id = id;
    mg.amount = 0;
    mg.active = 0;
    mg.all_can_use = 0;
    mg.state = 0;
    world->magplant.push_back(mg);
}

void pushdoor(Server *server, ENetPeer *peer, std::string dest, std::string label, std::string id, std::string iop)
{
    Worlds *world = getPlyersWorld(server, peer);
    Doors dr;
    dr.dest = dest;
    dr.label = label;
    dr.id = id;
    dr.is_open = iop;
    dr.x = pinfo(peer)->wrenchx;
    dr.y = pinfo(peer)->wrenchy;
    world->door.push_back(dr);
}

void pushsafe(Server *server, ENetPeer *peer, int count, int id, std::string pass)
{
    Worlds *world = getPlyersWorld(server, peer);
    safevault sf;
    sf.count = count;
    sf.id = id;
    sf.pass = pass;
    sf.x = pinfo(peer)->wrenchx;
    sf.y = pinfo(peer)->wrenchy;
    world->safe.push_back(sf);
}

void pushdblock(Server *server, ENetPeer *peer, int id)
{
    Worlds *world = getPlyersWorld(server, peer);
    Dblocks db;
    db.x = pinfo(peer)->wrenchx;
    db.y = pinfo(peer)->wrenchy;
    db.id = id;
    world->dblock.push_back(db);
}

void pushman(Server *server, ENetPeer *peer)
{
    Worlds *world = getPlyersWorld(server, peer);
    Mannequine man;
    man.x = pinfo(peer)->wrenchx;
    man.y = pinfo(peer)->wrenchy;
    man.clothBack = 0;
    man.clothFeet = 0;
    man.clothHair = 0;
    man.clothHand = 0;
    man.clothHead = 0;
    man.clothMask = 0;
    man.clothNeck = 0;
    man.clothPants = 0;
    man.clothShirt = 0;
    man.text = "";
    world->man.push_back(man);
}

void pushentrance(Server *server, ENetPeer *peer)
{
    Worlds *world = getPlyersWorld(server, peer);
    Entrances en;
    en.x = pinfo(peer)->wrenchx;
    en.y = pinfo(peer)->wrenchy;
    en.is_open = "1";
}

void InitializePacketWithDisplayBlock(BYTE *raw)
{
    int i = 0;
    raw[i] = 0x05;
    i++; // 0
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++; // 4
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++; // 8
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x08;
    i++; // 12
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++; // 16
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++; // 20
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++; // 24
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++; // 28
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++; // 32
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++; // 36
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++; // 40
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0xff;
    i++; // 44
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0xff;
    i++; // 48
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++; // 52
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x82;
    i++;
    raw[i] = 0x0b;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x01;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x17;
    i++;
    raw[i] = 0x82;
    i++;
    raw[i] = 0x04;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x6c;
    i++;
    raw[i] = 0xfd;
    i++;
    raw[i] = 0xfd;
    i++;
    raw[i] = 0xfd;
    i++;
}

void InitializePacketWithMannequin(BYTE *raw)
{
    int i = 0;
    raw[i] = 0x05;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x08;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x09;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x17;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x22;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x8c;
    i++;
    raw[i] = 0x05;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x01;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x0e;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
    raw[i] = 0x00;
    i++;
}

void updatedblock(ENetPeer *peer, int foreground, int x, int y, int background, int itemid)
{

    int plength = 74;
    BYTE *raw = new BYTE[plength];
    memset(raw, 0, plength);
    InitializePacketWithDisplayBlock(raw);

    memcpy(raw + 44, &x, sizeof(int));
    memcpy(raw + 48, &y, sizeof(int));
    memcpy(raw + 56, &foreground, sizeof(short));
    memcpy(raw + 58, &background, sizeof(short));
    memcpy(raw + 65, &itemid, sizeof(int));

    ENetPacket *p = enet_packet_create(0, plength + 4, ENET_PACKET_FLAG_RELIABLE);
    int four = 4;
    memcpy(p->data, &four, sizeof(int));
    memcpy((char *)p->data + 4, raw, plength);

    enet_peer_send(peer, 0, p);
    delete[] raw;
}

void updatesign(ENetPeer *peer, int fg, int bg, int x, int y, std::string text)
{
    int hmm = 8, wot = text.length(), lol = 0, wut = 5;
    int yeh = hmm + 3 + 1, idk = 15 + wot, lmao = -1, yey = 2; //idk = text_len + 15, wut = type(?), wot = text_len, yey = len of text_len
    int ok = 52 + idk;
    int kek = ok + 4, yup = ok - 8 - idk;
    int thonk = 4, magic = 56, wew = ok + 5 + 4;
    int wow = magic + 4 + 5;
    std::vector<BYTE> data(kek);
    ENetPacket *p = enet_packet_create(0, wew, ENET_PACKET_FLAG_RELIABLE);
    memcpy(&data[0], &wut, thonk);
    memcpy(&data[yeh], &hmm, thonk); //read discord
    memcpy(&data[yup], &x, 4);
    memcpy(&data[yup + 4], &y, 4);
    memcpy(&data[4 + yup + 4], &idk, thonk);
    memcpy(&data[magic], &fg, yey);
    memcpy(&data[magic + 2], &bg, yey); //p100 fix by the one and only lapada
    memcpy(&data[thonk + magic], &lol, thonk);
    memcpy(&data[magic + 4 + thonk], &yey, 1);
    memcpy(&data[wow], &wot, yey);               //data + wow = text_len
    memcpy(&data[yey + wow], text.c_str(), wot); //data + text_len_len + text_len_offs = text
    memcpy(&data[ok], &lmao, thonk);             //end ?
    memcpy(p->data, &thonk, thonk);
    memcpy((char *)p->data + thonk, &data[0], kek); //kek = data_len

    enet_peer_send(peer, 0, p);
}

void updateEntrance(ENetPeer *peer, int foreground, int x, int y, bool open, int bg)
{
    BYTE *data = new BYTE[69]; // memset(data, 0, 69);
    for (int i = 0; i < 69; i++)
        data[i] = 0;
    int four = 4;
    int five = 5;
    int eight = 8;
    int huhed = (65536 * bg) + foreground;
    int loled = 128;

    memcpy(data, &four, 4);
    memcpy(data + 4, &five, 4);
    memcpy(data + 16, &eight, 4);
    memcpy(data + 48, &x, 4);
    memcpy(data + 52, &y, 4);
    memcpy(data + 56, &eight, 4);
    memcpy(data + 60, &foreground, 4);
    memcpy(data + 62, &bg, 4);
    if (open)
    {
        int state = 0;
        memcpy(data + 66, &loled, 4);
        memcpy(data + 68, &state, 4);
    }
    else
    {
        int state = 100;
        int yeetus = 25600;
        memcpy(data + 67, &yeetus, 5);
        memcpy(data + 68, &state, 4);
    }
    ENetPacket *p = enet_packet_create(data, 69, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, p);
    delete[] data;
}

void updatedoor(ENetPeer *peer, int foreground, int background, int x, int y, std::string text, bool islocked)
{
    int hmm = 8;
    int text_len = text.length();
    int lol = 0;
    int wut = 5;
    int yeh = hmm + 3 + 1;
    int idk = 15 + text_len;
    int is_locked = 0;
    if (islocked)
        is_locked = -1;
    int bubble_type = 1;
    int ok = 52 + idk;
    int kek = ok + 4;
    int yup = ok - 8 - idk;
    int four = 4;
    int magic = 56;
    int wew = ok + 5 + 4;
    int wow = magic + 4 + 5;

    std::vector<BYTE> data(kek);
    ENetPacket *p = enet_packet_create(0, wew, ENET_PACKET_FLAG_RELIABLE);
    for (int i = 0; i < kek; i++)
        data[i] = 0;
    memcpy(&data[0], &wut, four);           //4
    memcpy(&data[yeh], &hmm, four);         //8
    memcpy(&data[yup], &x, 4);              //12
    memcpy(&data[yup + 4], &y, 4);          //16
    memcpy(&data[4 + yup + 4], &idk, four); //20
    memcpy(&data[magic], &foreground, 2);   //22
    memcpy(&data[magic + 2], &background, 2);
    memcpy(&data[four + magic], &lol, four);          //26
    memcpy(&data[magic + 4 + four], &bubble_type, 1); //27
    memcpy(&data[wow], &text_len, 2);                 //data + wow = text_len, pos 29
    memcpy(&data[2 + wow], text.c_str(), text_len);   //data + text_len_len + text_len_offs = text, pos 94
    memcpy(&data[ok], &is_locked, four);              //98
    memcpy(p->data, &four, four);                     //4
    memcpy((char *)p->data + four, &data[0], kek);    //kek = data_len
    enet_peer_send(peer, 0, p);
}

void updateMannequin(ENetPeer *peer, int foreground, int x, int y, int background, std::string text, int clothHair, int clothHead, int clothMask, int clothHand, int clothNeck, int clothShirt, int clothPants, int clothFeet, int clothBack)
{
    PlayerMoving sign;
    sign.packetType = 0x3;
    sign.characterState = 0x0;
    sign.x = x;
    sign.y = y;
    sign.punchX = x;
    sign.punchY = y;
    sign.XSpeed = 0;
    sign.YSpeed = 0;
    sign.netID = -1;
    sign.plantingTree = foreground;
    SendPacketRaw(4, packPlayerMoving(&sign), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);

    short textLen = text.size();
    int plength = 95 + textLen;
    BYTE *raw = new BYTE[plength];
    memset(raw, 0, plength);
    InitializePacketWithMannequin(raw);
    int negativeOne = -1;
    int adjhasdjk = 0xfdfdfdfd;
    int visor = 138;
    int blockState = 0;

    memcpy(raw + 44, &x, sizeof(int));
    memcpy(raw + 48, &y, sizeof(int));
    memcpy(raw + 56, &foreground, sizeof(short));
    memcpy(raw + 58, &background, sizeof(short));
    memcpy(raw + 60, &blockState, sizeof(short));
    memcpy(raw + 65, &textLen, sizeof(short));
    memcpy(raw + 67, text.c_str(), textLen);
    memcpy(raw + 68 + textLen, &negativeOne, sizeof(int));
    memcpy(raw + 72 + textLen, &clothHead, sizeof(short));
    memcpy(raw + 74 + textLen, &clothShirt, sizeof(short));
    memcpy(raw + 76 + textLen, &clothPants, sizeof(short));
    memcpy(raw + 78 + textLen, &clothFeet, sizeof(short));
    memcpy(raw + 80 + textLen, &clothMask, sizeof(short));
    memcpy(raw + 82 + textLen, &clothHand, sizeof(short));
    memcpy(raw + 84 + textLen, &clothBack, sizeof(short));
    memcpy(raw + 86 + textLen, &clothHair, sizeof(short));
    memcpy(raw + 88 + textLen, &clothNeck, sizeof(short));
    memcpy(raw + 91 + textLen, &adjhasdjk, sizeof(short));

    ENetPacket *p = enet_packet_create(0, plength + 4, ENET_PACKET_FLAG_RELIABLE);
    int four = 4;
    memcpy(p->data, &four, sizeof(int));
    memcpy((char *)p->data + 4, raw, plength);
    enet_peer_send(peer, 0, p);
    delete[] raw;
}

uint8_t *SuckerTileData(const uint16_t itemid, const uint16_t itemamount, const uint8_t magnet_on, const uint8_t remote_on)
{
    const auto data = new uint8_t[15];
    memset(data, 0, 15);
    data[0] = 0x3E;
    *reinterpret_cast<uint16_t *>(data + 1) = itemid;
    *reinterpret_cast<uint16_t *>(data + 5) = itemamount;
    *static_cast<uint8_t *>(data + 9) = magnet_on;
    *static_cast<uint8_t *>(data + 10) = remote_on;
    *static_cast<uint8_t *>(data + 12) = 1;
    return data;
}

void SendItemSucker(ENetPeer *peer, const int blockid, const int x, const int y, const uint16_t itemid, const uint16_t itemamount, const uint8_t magneton, const uint8_t remoteon)
{
    PlayerMoving pmov;
    pmov.packetType = 5;
    pmov.characterState = 8;
    pmov.punchX = x;
    pmov.punchY = y;
    pmov.netID = -1;
    std::vector<BYTE> pmovpacked = packPlayerMoving(&pmov);
    *reinterpret_cast<uint32_t *>(&pmovpacked[52]) = 15 + 8;
    const auto packet = new uint8_t[4 + 56 + 15 + 8];
    memset(packet, 0, 4 + 56 + 15 + 8);
    packet[0] = 4;
    memcpy(packet + 4, &pmovpacked[0], 56);
    *reinterpret_cast<uint16_t *>(packet + 4 + 56) = blockid;
    *reinterpret_cast<uint16_t *>(packet + 4 + 56 + 6) = 1;
    const auto tiledata = SuckerTileData(itemid, itemamount, magneton, remoteon);
    memcpy(packet + 4 + 56 + 8, tiledata, 15);
    const auto epacket = enet_packet_create(packet, 4 + 56 + 8 + 15, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, epacket);
    delete[] packet;
    delete[] tiledata;
}

std::string packPlayerMoving2(PlayerMoving *dataStruct)
{
    std::string data;
    data.resize(56);
    STRINT(data, 0) = dataStruct->packetType;
    STRINT(data, 4) = dataStruct->netID;
    STRINT(data, 12) = dataStruct->characterState;
    STRINT(data, 20) = dataStruct->plantingTree;
    STRINT(data, 24) = *(int *)&dataStruct->x;
    STRINT(data, 28) = *(int *)&dataStruct->y;
    STRINT(data, 32) = *(int *)&dataStruct->XSpeed;
    STRINT(data, 36) = *(int *)&dataStruct->YSpeed;
    STRINT(data, 44) = dataStruct->punchX;
    STRINT(data, 48) = dataStruct->punchY;
    return data;
}

std::string lockTileDatas(int visual, uint32_t owner, uint32_t adminLength, uint32_t *admins, bool isPublic, uint8_t bpm)
{
    std::string data;
    data.resize(4 + 2 + 4 + 4 + adminLength * 4 + 8);
    if (bpm)
        data.resize(data.length() + 4);
    data[2] = 0x01;
    if (isPublic)
        data[2] |= 0x80;
    data[4] = 3;
    data[5] = visual; // or 0x02
    STRINT(data, 6) = owner;
    //data[14] = 1;
    STRINT(data, 10) = adminLength;
    for (uint32_t i = 0; i < adminLength; i++)
    {
        STRINT(data, 14 + i * 4) = admins[i];
    }

    if (bpm)
    {
        STRINT(data, 10)
        ++;
        STRINT(data, 14 + adminLength * 4) = -bpm;
    }
    return data;
}

void sendTileData(ENetPeer *peer, int x, int y, int visual, uint16_t fgblock, uint16_t bgblock, std::string tiledata)
{
    PlayerMoving pmov;
    pmov.packetType = 5;
    pmov.characterState = 0;
    pmov.x = 0;
    pmov.y = 0;
    pmov.XSpeed = 0;
    pmov.YSpeed = 0;
    pmov.plantingTree = 0;
    pmov.punchX = x;
    pmov.punchY = y;
    pmov.netID = 0;

    std::string packetstr;
    packetstr.resize(4);
    packetstr[0] = 4;
    packetstr += packPlayerMoving2(&pmov);
    packetstr[16] = 8;
    packetstr.resize(packetstr.size() + 4);
    STRINT(packetstr, 52 + 4) = tiledata.size() + 4;
    STR16(packetstr, 56 + 4) = fgblock;
    STR16(packetstr, 58 + 4) = bgblock;
    packetstr += tiledata;

    ENetPacket *packet = enet_packet_create(&packetstr[0],
                                            packetstr.length(),
                                            ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(peer, 0, packet);
}

void remove_clothes_if_wearing(ENetPeer *peer, ENetHost *Host, int id)
{
    switch (itemDefs[id].clothingType)
    {
    case ClothTypes::BACK:
        if (pinfo(peer)->back == id)
        {
            pinfo(peer)->back = 0;
            pinfo(peer)->canDoubleJump = false;
        }
        sendState(Host, peer);
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

void sendWrench(ENetHost *Host, ENetPeer *peer, bool owner, bool mod, bool dev, bool mySelf)
{
    Packets p;
    std::string rank = "";
    if (dev)
    {
        if (mySelf)
        {
            std::string worlds = "";
            for (int i = 0; i < static_cast<int>(pinfo(peer)->worlds.size()); i++)
            {
                worlds += pinfo(peer)->worlds[i] + ", ";
            }
            p.dialog(peer, "set_default_color|`0\n\nadd_player_info|" + pinfo(peer)->name + "|" + std::to_string(pinfo(peer)->level) + "|" + std::to_string(pinfo(peer)->xp) + "|" + std::to_string(pinfo(peer)->to_break) + "|\nadd_spacer|small|\nadd_spacer|small|\nadd_label_with_icon|small|`wGems = `2" + std::to_string(pinfo(peer)->gem) + "|left|112|\nadd_spacer|small|\nadd_label|small|`oCurrent world : `0" + pinfo(peer)->currentWorld + "|left|4|\nadd_label|small|`oMy Worlds: " + worlds + "|left|4|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit");
        }
        else
        {
            ENetPeer *currentPeer = player(Host, pinfo(peer)->lastplayerwrench);
            if (currentPeer != NULL)
            {
                if (pinfo(currentPeer)->adminLevel == 0)
                    rank = "`w(no rank)";
                if (pinfo(currentPeer)->adminLevel == 333)
                    rank = "`r(VIP)";
                if (pinfo(currentPeer)->adminLevel == 666)
                    rank = "`#(Moderator)";
                if (pinfo(currentPeer)->adminLevel == 999)
                    rank = "`6(Server Creator)";
                std::string name = pinfo(currentPeer)->username;
                std::string gems = std::to_string(pinfo(currentPeer)->gem);
                int idx = -1;
                for (int i = 0; i < static_cast<int>(pinfo(peer)->friends.size()); i++)
                {
                    if (pinfo(peer)->friends[i] == pinfo(peer)->lastplayerwrench)
                    {
                        idx = i;
                    }
                }
                if (idx == -1) //add friend button
                {
                    p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + name + "`w(`2" + std::to_string(pinfo(currentPeer)->level) + "`w)``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player gems: " + gems + "``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player rank: " + rank + "``|left|32|\nadd_spacer|small|\nadd_button|trade|`wTrade|0|0|\nadd_button|punishorview|`1Punish/View|0|0|\nadd_button|addfriend|`wAdd as friend|0|0|\nadd_button|close|`oClose|0|0|\nadd_quick_exit");
                }
                else
                {
                    p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + name + "`w(`2" + std::to_string(pinfo(currentPeer)->level) + "`w)``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player gems: " + gems + "``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player rank: " + rank + "``|left|32|\nadd_spacer|small|\nadd_button|trade|`wTrade|0|0|\nadd_button|punishorview|`1Punish/View|0|0|\nadd_button|close|`oClose|0|0|\nadd_quick_exit");
                }
            }
        }
    }
    else if (mod)
    {
        if (mySelf)
        {
            std::string worlds = "";
            for (int i = 0; i < static_cast<int>(pinfo(peer)->worlds.size()); i++)
            {
                worlds += pinfo(peer)->worlds[i] + ", ";
            }
            p.dialog(peer, "set_default_color|`0\n\nadd_player_info|" + pinfo(peer)->name + "|" + std::to_string(pinfo(peer)->level) + "|" + std::to_string(pinfo(peer)->xp) + "|" + std::to_string(pinfo(peer)->to_break) + "|\nadd_spacer|small|\nadd_spacer|small|\nadd_label_with_icon|small|`wGems = `2" + std::to_string(pinfo(peer)->gem) + "|left|112|\nadd_spacer|small|\nadd_label|small|`oCurrent world : `0" + pinfo(peer)->currentWorld + "|left|4|\nadd_label|small|`oMy Worlds: " + worlds + "|left|4|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit");
        }
        else
        {
            ENetPeer *currentPeer = player(Host, pinfo(peer)->lastplayerwrench);
            if (currentPeer != NULL)
            {
                if (pinfo(currentPeer)->adminLevel == 0)
                    rank = "`w(no rank)";
                if (pinfo(currentPeer)->adminLevel == 333)
                    rank = "`r(VIP)";
                if (pinfo(currentPeer)->adminLevel == 666)
                    rank = "`#(Moderator)";
                if (pinfo(currentPeer)->adminLevel == 999)
                    rank = "`6(Server Creator)";
                std::string name = pinfo(currentPeer)->username;
                std::string gems = std::to_string(pinfo(currentPeer)->gem);
                int idx = -1;
                for (int i = 0; i < static_cast<int>(pinfo(peer)->friends.size()); i++)
                {
                    if (pinfo(peer)->friends[i] == pinfo(peer)->lastplayerwrench)
                    {
                        idx = i;
                    }
                }
                if (idx == -1) //add friend button
                {
                    p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + name + "`w(`2" + std::to_string(pinfo(currentPeer)->level) + "`w)``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player gems: " + gems + "``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player rank: " + rank + "``|left|32|\nadd_spacer|small|\nadd_button|trade|`wTrade|0|0|\nadd_button|punishorview|`1Punish/View|0|0|\nadd_button|addfriend|`wAdd as friend|0|0|\nadd_button|close|`oClose|0|0|\nadd_quick_exit");
                }
                else
                {
                    p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + name + "`w(`2" + std::to_string(pinfo(currentPeer)->level) + "`w)``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player gems: " + gems + "``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player rank: " + rank + "``|left|32|\nadd_spacer|small|\nadd_button|trade|`wTrade|0|0|\nadd_button|punishorview|`1Punish/View|0|0|\nadd_button|close|`oClose|0|0|\nadd_quick_exit");
                }
            }
        }
    }
    else if (owner)
    {
        if (mySelf)
        {
            std::string worlds = "";
            for (int i = 0; i < static_cast<int>(pinfo(peer)->worlds.size()); i++)
            {
                worlds += pinfo(peer)->worlds[i] + ", ";
            }
            p.dialog(peer, "set_default_color|`0\n\nadd_player_info|" + pinfo(peer)->name + "|" + std::to_string(pinfo(peer)->level) + "|" + std::to_string(pinfo(peer)->xp) + "|" + std::to_string(pinfo(peer)->to_break) + "|\nadd_spacer|small|\nadd_spacer|small|\nadd_label_with_icon|small|`wGems = `2" + std::to_string(pinfo(peer)->gem) + "|left|112|\nadd_spacer|small|\nadd_label|small|`oCurrent world : `0" + pinfo(peer)->currentWorld + "|left|4|\nadd_label|small|`oMy Worlds: " + worlds + "|left|4|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit");
        }
        else
        {
            ENetPeer *currentPeer = player(Host, pinfo(peer)->lastplayerwrench);
            if (currentPeer != NULL)
            {
                if (pinfo(currentPeer)->adminLevel == 0)
                    rank = "`w(no rank)";
                if (pinfo(currentPeer)->adminLevel == 333)
                    rank = "`r(VIP)";
                if (pinfo(currentPeer)->adminLevel == 666)
                    rank = "`#(Moderator)";
                if (pinfo(currentPeer)->adminLevel == 999)
                    rank = "`6(Server Creator)";
                std::string name = pinfo(currentPeer)->username;
                std::string gems = std::to_string(pinfo(currentPeer)->gem);
                int idx = -1;
                for (int i = 0; i < static_cast<int>(pinfo(peer)->friends.size()); i++)
                {
                    if (pinfo(peer)->friends[i] == pinfo(peer)->lastplayerwrench)
                    {
                        idx = i;
                    }
                }
                if (idx == -1) //add friend button
                {
                    p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + name + "`w(`2" + std::to_string(pinfo(currentPeer)->level) + "`w)``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player gems: " + gems + "``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player rank: " + rank + "``|left|32|\nadd_spacer|small|\nadd_button|trade|`wTrade|0|0|\nadd_button|pull|`5pull|0|0|\nadd_button|kick|`4kick|0|0|\nadd_button|wban|`4World Ban|0|0|\nadd_button|addfriend|`wAdd as friend|0|0|\nadd_button|close|`oClose|0|0|\nadd_quick_exit");
                }
                else
                {
                    p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + name + "`w(`2" + std::to_string(pinfo(currentPeer)->level) + "`w)``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player gems: " + gems + "``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player rank: " + rank + "``|left|32|\nadd_spacer|small|\nadd_button|trade|`wTrade|0|0|\nadd_button|pull|`5pull|0|0|\nadd_button|kick|`4kick|0|0|\nadd_button|wban|`4World Ban|0|0|\nadd_button|close|`oClose|0|0|\nadd_quick_exit");
                }
            }
        }
    }
    else
    {
        if (mySelf)
        {
            std::string worlds = "";
            for (int i = 0; i < static_cast<int>(pinfo(peer)->worlds.size()); i++)
            {
                worlds += pinfo(peer)->worlds[i] + ", ";
            }
            p.dialog(peer, "set_default_color|`0\n\nadd_player_info|" + pinfo(peer)->name + "|" + std::to_string(pinfo(peer)->level) + "|" + std::to_string(pinfo(peer)->xp) + "|" + std::to_string(pinfo(peer)->to_break) + "|\nadd_spacer|small|\nadd_spacer|small|\nadd_label_with_icon|small|`wGems = `2" + std::to_string(pinfo(peer)->gem) + "|left|112|\nadd_spacer|small|\nadd_label|small|`oCurrent world : `0" + pinfo(peer)->currentWorld + "|left|4|\nadd_label|small|`oMy Worlds: " + worlds + "|left|4|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit");
        }
        else
        {
            ENetPeer *currentPeer = player(Host, pinfo(peer)->lastplayerwrench);
            if (currentPeer != NULL)
            {
                if (pinfo(currentPeer)->adminLevel == 0)
                    rank = "`w(no rank)";
                if (pinfo(currentPeer)->adminLevel == 333)
                    rank = "`r(VIP)";
                if (pinfo(currentPeer)->adminLevel == 666)
                    rank = "`#(Moderator)";
                if (pinfo(currentPeer)->adminLevel == 999)
                    rank = "`6(Server Creator)";
                std::string name = pinfo(currentPeer)->username;
                std::string gems = std::to_string(pinfo(currentPeer)->gem);
                int idx = -1;
                for (int i = 0; i < static_cast<int>(pinfo(peer)->friends.size()); i++)
                {
                    if (pinfo(peer)->friends[i] == pinfo(peer)->lastplayerwrench)
                    {
                        idx = i;
                    }
                }
                if (idx == -1) //add friend button
                {
                    p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + name + "`w(`2" + std::to_string(pinfo(currentPeer)->level) + "`w)``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player gems: " + gems + "``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player rank: " + rank + "``|left|32|\nadd_spacer|small|\nadd_button|trade|`wTrade|0|0|\nadd_button|addfriend|`wAdd as friend|0|0|\nadd_button|close|`oClose|0|0|\nadd_quick_exit");
                }
                else
                {
                    p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + name + "`w(`2" + std::to_string(pinfo(currentPeer)->level) + "`w)``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player gems: " + gems + "``|left|32|\nadd_spacer|small|\nadd_label|small|`9Player rank: " + rank + "``|left|32|\nadd_spacer|small|\nadd_button|trade|`wTrade|0|0|\nadd_button|close|`oClose|0|0|\nadd_quick_exit");
                }
            }
        }
    }
}

void sendArrow(ENetPeer *peer, const uint16_t itemid, const bool droppeditem)
{
    PlayerMoving pmov;
    memset(&pmov, 0, sizeof(PlayerMoving));
    *reinterpret_cast<uint32_t *>(&pmov) = 37 | (droppeditem << 8);
    pmov.netID = itemid;
    const auto pmovpacked = packPlayerMoving(&pmov);
    const auto packet = new uint8_t[4 + 56];
    memset(packet, 0, 4 + 56);
    packet[0] = 4;
    memcpy(packet + 4, &pmovpacked[0], sizeof(PlayerMoving));
    const auto epacket = enet_packet_create(packet, 4 + 56, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, epacket);
    delete[] packet;
}

void RemoveDroppedItem(ENetHost *Host, ENetPeer *peer, const int obj_id)
{
    std::vector<ENetPeer *> ishere = isHere(Host, peer);
    for (ENetPeer *currentPeer : ishere)
    {
        std::vector<BYTE> b(56);
        *reinterpret_cast<int *>(&b[0]) = 0xe;
        *reinterpret_cast<int *>(&b[4]) = -2;
        *reinterpret_cast<int *>(&b[8]) = -1;
        *reinterpret_cast<int *>(&b[20]) = obj_id + 1;
        SendPacketRaw(4, b, 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
    }
}

void DropItem(Server *server, ENetHost *Host, ENetPeer *peer, int netID, float x, float y, int item, int count, BYTE specialEffect)
{
    Worlds *world = getPlyersWorld(server, peer);
    if (item > itemDefs.size())
        return;
    DroppedItem itemDropped;
    itemDropped.id = item;
    itemDropped.count = count;
    itemDropped.x = x;
    itemDropped.y = y;
    itemDropped.uid = world->dropcount++;
    world->droppeditems.push_back(itemDropped);
    world->dropsize = world->droppeditems.size();
    sendDrop(Host, peer, netID, x, y, item, count, specialEffect);
}

void gem_system(Server *server, ENetHost *Host, ENetPeer *peer, int block, int x, int y, bool drop, int i)
{
    int loop = 0;
    int tot = 0;
    if (itemDefs[block].rarity < 20)
        loop = 2;
    if (itemDefs[block].rarity >= 20)
        loop = 3;
    if (itemDefs[block].rarity >= 80)
        loop = 4;
    if (gem_event != 0)
        tot = loop * gem_event;
    else
        tot = loop;
    for (int i = 0; i < tot; i++)
    {
        int f = 0;
        if (i == 1)
            f = 3;
        if (i == 2)
            f = 6;
        if (i == 3)
            f = 9;
        if (i == 4)
            f = 12;
        int rad = rand() % itemDefs[block].gem.size();
        int value = itemDefs[block].gem[rad];
        if (value == 1 || value == 5 || value == 10)
        {
            if (block == 10)
            {
                int ras = (rand() % 1) + 1;
                if (value != 0)
                {
                    if (drop)
                    {
                        DropItem(server, Host, peer, -1, x * 32, y * 32 + f, value, ras, 0);
                    }
                    else
                    {
                        Worlds *world = getPlyersWorld(server, peer);
                        world->magplant[i].amount += ras;
                    }
                }
            }
            else
            {
                if (value != 0)
                {
                    if (drop)
                    {
                        DropItem(server, Host, peer, -1, x * 32, y * 32 + f, 112, value, 0);
                    }
                    else
                    {
                        Worlds *world = getPlyersWorld(server, peer);
                        world->magplant[i].amount += value;
                    }
                }
            }
        }
        else
        {
            int rasz = (rand() % 1) + 1;
            if (value != 0)
            {
                if (drop)
                {
                    DropItem(server, Host, peer, -1, x * 32, y * 32 + f, value, rasz, 0);
                }
                else
                {
                    Worlds *world = getPlyersWorld(server, peer);
                    world->magplant[i].amount += rasz;
                }
            }
        }
    }
}

void reset_trade(ENetPeer *peer, bool end)
{
    Packets p;
    if (end)
    {
        p.onforcetradeend(peer);
    }
    pinfo(peer)->accepted = false;
    pinfo(peer)->trade.clear();
    pinfo(peer)->tradingme = "";
    pinfo(peer)->itemlist = "";
    pinfo(peer)->lasttradeid = 0;
    pinfo(peer)->dotrade = false;
    pinfo(peer)->trading = false;
    pinfo(peer)->canceled = false;
}

uint8_t *magplantTileData(uint16_t itemid, uint16_t itemamount, uint8_t magnet_on, uint8_t remote_on)
{
    uint8_t *data = new uint8_t[15];
    memset(data, 0, 15);
    data[0] = 0x3E;
    *(uint16_t *)(data + 1) = itemid;
    *(uint16_t *)(data + 5) = itemamount;
    *(uint8_t *)(data + 9) = magnet_on;
    *(uint8_t *)(data + 10) = remote_on;
    *(uint8_t *)(data + 12) = 1;
    return data;
}

void sendMag(ENetPeer *peer, int x, int y, uint16_t itemid, uint16_t itemamount, uint8_t magneton, uint8_t remoteon)
{
    PlayerMoving pmov;
    pmov.packetType = 5;
    pmov.characterState = 8;
    pmov.punchX = x;
    pmov.punchY = y;
    pmov.netID = -1;
    std::vector<BYTE> pmovpacked = packPlayerMoving(&pmov);
    *(uint32_t *)(&pmovpacked[0] + 52) = 15 + 8;
    std::vector<BYTE> packet(4 + 56 + 15 + 8);
    packet[0] = 4;
    memcpy(&packet[4], &pmovpacked[0], 56);
    *(uint16_t *)(&packet[4 + 56]) = 5638; // magplant id
    *(uint16_t *)(&packet[4 + 56 + 6]) = 1;
    uint8_t *tiledata = magplantTileData(itemid, itemamount, magneton, remoteon);
    memcpy(&packet[4 + 56 + 8], tiledata, 15);
    ENetPacket *epacket = enet_packet_create((void *)&packet[0], 4 + 56 + 8 + 15, ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(peer, 0, epacket);

    delete[] tiledata;
}