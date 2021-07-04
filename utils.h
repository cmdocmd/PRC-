#pragma once

#include <iostream>
#include <vector>
#include "enet/enet.h"
#include "players.h"

typedef unsigned char BYTE;

extern int gem_event;
extern std::vector<int> cant_take;

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

class Server;

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
void Add_ITEM(ENetPeer *peer, int id, int count, bool send = false);
bool LEGAL_ITEM(ENetPeer *peer, int id);
void sendDrop(ENetHost *Host, ENetPeer *peer, int netID, int x, int y, int item, int count, BYTE specialEffect);
void sendTake(ENetHost *Host, ENetPeer *peer, int netID, float x, float y, int item);
int get_count(ENetPeer *peer, int id, int add);
void sendmusic(ENetPeer *peer, std::string music);
void updatetree(ENetHost *Host, ENetPeer *peer, int foreground, int x, int y, int background, int fruitCount, int timeIntoGrowth, int blockStateFlags);
void Rem(ENetPeer *peer, int id, int amount, bool send);
void Add(ENetPeer *peer, int id, int amount, bool send);
bool MoreThan200(ENetPeer *peer, int id, int amount);
void sendPlayerLeave(ENetHost *Host, ENetPeer *peer);
void Respawn(ENetPeer *peer, bool die);
void sendclothes(ENetHost *Host, ENetPeer *peer);
void pushsign(Server *server, ENetPeer *peer, std::string text);
void pushdoor(Server *server, ENetPeer *peer, std::string dest, std::string label, std::string id, std::string iop);
void pushdblock(Server *server, ENetPeer *peer, int id);
void pushman(Server *server, ENetPeer *peer);
void pushentrance(Server *server, ENetPeer *peer);
void pushmag(Server *server, ENetPeer *peer, int id);
void sendtrade(ENetHost *Host, ENetPeer *peer, int id, int netIDsrc, int netIDdst, int timeMs);
void updatedblock(ENetPeer* peer, int foreground, int x, int y, int background, int itemid);
void updatesign(ENetPeer *peer, int fg, int bg, int x, int y, std::string text);
void updatedoor(ENetPeer *peer, int foreground, int background, int x, int y, std::string text, bool islocked);
void updateMannequin(ENetPeer *peer, int foreground, int x, int y, int background, std::string text, int clothHair, int clothHead, int clothMask, int clothHand, int clothNeck, int clothShirt, int clothPants, int clothFeet, int clothBack);
void updateEntrance(ENetPeer *peer, int foreground, int x, int y, bool open, int bg);
std::string lockTileDatas(int visual, uint32_t owner, uint32_t adminLength, uint32_t *admins, bool isPublic = false, uint8_t bpm = 0);
void sendTileData(ENetPeer *peer, int x, int y, int visual, uint16_t fgblock, uint16_t bgblock, std::string tiledata);
void remove_clothes_if_wearing(ENetPeer *peer, ENetHost *Host, int id);
void sendArrow(ENetPeer *peer, const uint16_t itemid, const bool droppeditem);
void RemoveDroppedItem(ENetHost *Host, ENetPeer *peer, const int obj_id);
void DropItem(Server * server, ENetHost * Host, ENetPeer* peer, int netID, float x, float y, int item, int count, BYTE specialEffect);
void sendWrench(ENetHost *Host, ENetPeer *peer, bool owner, bool mod, bool dev, bool mySelf);
void gem_system(Server *server, ENetHost *Host, ENetPeer *peer, int block, int x, int y, bool drop, int i);
void pushsafe(Server *server, ENetPeer *peer, int count, int id, std::string pass);
void reset_trade(ENetPeer* peer, bool end);
void sendMag(ENetPeer *peer, int x, int y, uint16_t itemid, uint16_t itemamount, uint8_t magneton, uint8_t remoteon);
void SendItemSucker(ENetPeer* peer, const int blockid, const int x, const int y, const uint16_t itemid, const uint16_t itemamount, const uint8_t magneton, const uint8_t remoteon);
