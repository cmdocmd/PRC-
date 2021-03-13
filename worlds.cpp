#pragma once
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include <array>
#include <iostream>
#include <sstream>
#include "player.cpp"
#include "utils.cpp"
#include "GamePacketBuilder.cpp"

int cId = 1;

typedef unsigned char BYTE;

bool WORLD_EXIST(std::string world);
std::istream *WORLD_DATA(std::string world);
void INSERT_WORLD(std::string name, std::stringstream blob);
void UPDATE_WORLD(std::stringstream str, std::string name);

string getStrUpper(string txt)
{
    string ret;
    for (char c : txt)
        ret += toupper(c);
    return ret;
}


class WorldItem {
  private:
    friend class boost::serialization::access;
    template <class Ar> void serialize(Ar& ar, unsigned) {
        ar& foreground& background& breakLevel& breakTime& water& fire& glue&
            red& green& blue;
    }

  public:
    int foreground          = 0;
    int background          = 0;
    int breakLevel          = 0;
    long long int breakTime = 0;
    bool water              = false;
    bool fire               = false;
    bool glue               = false;
    bool red                = false;
    bool green              = false;
    bool blue               = false;
};

class Worlds
{
  private:
    friend class boost::serialization::access;
    template <class Ar> void serialize(Ar& ar, unsigned) {
        ar& width& height& name& items& owner& weather& isPublic& isNuked;
    }

  public:
    int width;
    int height;
    std::string name;

    std::array<WorldItem, 100 * 60> items;
    std::string owner = "";
    int weather       = 0;
    bool isPublic     = false;
    bool isNuked      = false;
};

Worlds generateWorld(std::string name, int width, int height) {
    Worlds world;
    world.name   = name;
    world.width = width;
    world.height = height;
    for (int i = 0; i < world.width * world.height; i++)
    {
        if (i >= 3800 && i < 5400 && !(rand() % 50))
        {
            world.items[i].foreground = 10;
        }
        else if (i >= 3700 && i < 5400)
        {
            if (i > 5000)
            {
                if (i % 7 == 0)
                {
                    world.items[i].foreground = 4;
                }
                else
                {
                    world.items[i].foreground = 2;
                }
            }
            else
            {
                world.items[i].foreground = 2;
            }
        }
        else if (i >= 5400)
        {
            world.items[i].foreground = 8;
        }
        if (i >= 3700)
            world.items[i].background = 14;
        if (i == 3650)
            world.items[i].foreground = 6;
        else if (i >= 3600 && i < 3700)
            world.items[i].foreground = 0;
        if (i == 3750)
            world.items[i].foreground = 8;
    }
    return world;
}

std::vector<Worlds> worlds;

std::stringstream serialize_world(Worlds const& world) {
    std::stringstream str;
    {
        boost::archive::binary_oarchive oa(str);
        oa << world;
    }
    return str;
}

Worlds deserialize(std::string world) {
    Worlds wld;
    std::istream *blobdata = WORLD_DATA(world);
    {
        boost::archive::binary_iarchive ia(*blobdata);
        ia >> wld;
    }
    return wld;
}

void FLUSH_WORLDS(Worlds world)
{
    if (WORLD_EXIST(world.name)) //UPDATE
    {
        UPDATE_WORLD(serialize_world(world), world.name);
    }
    else //INSERT
    {
        INSERT_WORLD(world.name, serialize_world(world));
    }
}

void SAVE_WORLDS(ENetHost *server)
{
    for (int i = 4; i < static_cast<int>(worlds.size()); i++)
    {
        bool canBeFree = true;
        ENetPeer *currentPeer;

        for (currentPeer = server->peers;
             currentPeer < &server->peers[server->peerCount];
             ++currentPeer)
        {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                continue;
            if (pinfo(currentPeer)->currentWorld == worlds.at(i).name)
            {
                canBeFree = false;
            }
        }
        if (canBeFree) //SAVE TO MYSQL AND DELETE THE ITEMS
        {
            std::cout << worlds.at(i).name << std::endl;
            FLUSH_WORLDS(worlds.at(i));
            worlds.erase(worlds.begin() + i);
            i--;
        }
    }
}

struct AWorld
{
    Worlds *ptr;
    Worlds info;
    int id;
};

AWorld GET_WORLD(ENetHost *server, std::string world)
{
    //WHEN A GUY JOIN A WORLD WE GOTTA FREE THE UNUSED WORLDS BY CALLING SAVE_WORLDS
    SAVE_WORLDS(server);
    AWorld ret;
    world = getStrUpper(world);
    if (world.length() < 1)
    {
        throw 1;
    }
    if (world.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
    {
        throw 2;
    }
    if (world == "EXIT")
    {
        throw 3;
    }
    for (int i = 0; i < static_cast<int>(worlds.size()); i++)
    {
        if (worlds.at(i).name == world)
        {
            ret.id = i;
            ret.info = worlds.at(i);
            ret.ptr = &worlds.at(i);
            return ret;
        }
    }
    if (WORLD_EXIST(world)) //LOAD
    {
        Worlds wld = deserialize(world);
        std::cout << wld.name << std::endl;
        worlds.push_back(wld);
        ret.id = worlds.size() - 1;
        ret.info = wld;
        ret.ptr = &worlds.at(worlds.size() - 1);
        return ret;
    }
    else
    {
        Worlds wld = generateWorld(world, 100, 60);
        worlds.push_back(wld);
        std::cout << worlds.size() << std::endl;
        ret.id = worlds.size() - 1;
        ret.info = wld;
        ret.ptr = &worlds.at(worlds.size() - 1);
        return ret;
    }
}

void sendWorld(ENetPeer *peer, Worlds *world)
{

    //((PlayerInfo*)(peer->data))->joinClothesUpdated = false;

    int xSize = world->width;
    int ySize = world->height;
    int square = xSize * ySize;
    int namelen = world->name.length();

    int alloc = (8 * square);
    int total = 78 + namelen + square + 24 + alloc;

    //BYTE *data = new BYTE[total];
    std::vector<BYTE> data(total);
    int s1 = 4, s3 = 8, zero = 0;

    //memset(data, 0, total);
    memcpy(&data[0], &s1, 1);
    memcpy(&data[4], &s1, 1);
    memcpy(&data[16], &s3, 1);
    memcpy(&data[66], &namelen, 1);
    memcpy(&data[68], world->name.c_str(), namelen);
    memcpy(&data[68] + namelen, &xSize, 1);
    memcpy(&data[72] + namelen, &ySize, 1);
    memcpy(&data[76] + namelen, &square, 2);
    BYTE *blc = &data[80 + namelen];
    for (int i = 0; i < square; i++)
    {
        //removed cus some of blocks require tile extra and it will crash the world without
        memcpy(blc, &zero, 2);

        memcpy(blc + 2, &world->items[i].background, 2);
        int type = 0x00000000;
        // type 1 = locked
        if (world->items[i].water)
            type |= 0x04000000;
        if (world->items[i].glue)
            type |= 0x08000000;
        if (world->items[i].fire)
            type |= 0x10000000;
        if (world->items[i].red)
            type |= 0x20000000;
        if (world->items[i].green)
            type |= 0x40000000;
        if (world->items[i].blue)
            type |= 0x80000000;

        memcpy(blc + 4, &type, 4);
        blc += 8;
    }

    //int totalitemdrop = worldInfo->dropobject.size();
    //memcpy(blc, &totalitemdrop, 2);

    ENetPacket *packetw = enet_packet_create((void*)&data[0], total, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packetw);
   // delete[] data;

    for (int i = 0; i < square; i++)
    {
        PlayerMoving data;
        //data.packetType = 0x14;
        data.packetType = 0x3;

        //data.characterState = 0x924; // animation
        data.characterState = 0x0; // animation
        data.x = i % world->width;
        data.y = i / world->height;
        data.punchX = i % world->width;
        data.punchY = i / world->width;
        data.XSpeed = 0;
        data.YSpeed = 0;
        data.netID = -1;
        data.plantingTree = world->items[i].foreground;
        SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
    }
    pinfo(peer)->currentWorld = world->name;
    Packets::consoleMessage(peer, "`#[`0" + world->name + " `9World Locked by " + world->owner + "`#]");
}

void joinWorld(ENetHost *server, ENetPeer *peer, string act, int x2, int y2)
{
    try
    {
        if (act.length() > 30)
        {

            return;
        }
        else
        {
            Worlds wld;
            wld = GET_WORLD(server, act).info;
            sendWorld(peer, &wld);
            int x = 3040;
            int y = 736;

            for (int j = 0; j < wld.width * wld.height; j++)
            {
                if (wld.items[j].foreground == 6)
                {
                    x = (j % wld.width) * 32;
                    y = (j / wld.width) * 32;
                }
            }
            pinfo(peer)->cpx = x;
            pinfo(peer)->cpy = y;
            if (x2 != 0 && y2 != 0)
            {
                x = x2;
                y = y2;
            }
            int uid = pinfo(peer)->userID; //TODO DONT READ THE FLAG OF THE PACKET BUT FROM DATABASE
            Packets::onSpawn(peer, "spawn|avatar\nnetID|" + std::to_string(cId) + "\nuserID|" + std::to_string(uid) + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(x) + "|" + std::to_string(y) + "\nname|``" + pinfo(peer)->name + "``\ncountry|" + pinfo(peer)->country + "\ninvis|0\nmstate|0\nsmstate|0\ntype|local\n");
            pinfo(peer)->netID = cId;
            onPeerConnect(server, peer);
            cId++;
            //sendInventory(peer, pdata(peer)->inventory);
        }
    }
    catch (int e)
    {
        if (e == 1)
        {
            return;
        }
        else if (e == 2)
        {
            return;
        }
        else if (e == 3)
        {
            return;
        }
        else
        {
            return;
        }
    }
}