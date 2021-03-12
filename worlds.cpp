#pragma once
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "enet/include/enet/enet.h"
#include "player.cpp"
#include "GamePacketBuilder.cpp"

using std::string;

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

class WorldItem
{
private:
    friend class boost::serialization::access;
    template <class Archieve>
    void serialize(Archieve &ar, const unsigned int version)
    {
        ar &foreground;
        ar &background;
        ar &breakLevel;
        ar &breakTime;
        ar &water;
        ar &fire;
        ar &glue;
        ar &red;
        ar &green;
        ar &blue;
    }

public:
    int foreground = 0;
    int background = 0;
    int breakLevel = 0;
    long long int breakTime = 0;
    bool water = false;
    bool fire = false;
    bool glue = false;
    bool red = false;
    bool green = false;
    bool blue = false;
};

class Worlds
{
private:
    friend class boost::serialization::access;
    template <class Archieve>
    void serialize(Archieve &ar, const unsigned int version)
    {
        ar &width;
        ar &height;
        ar &name;
        ar &items;
        ar &owner;
        ar &weather;
        ar &isPublic;
        ar &isNuked;
    }

public:
    int width;
    int height;
    string name;
    WorldItem *items;
    string owner = "";
    int weather = 0;
    bool isPublic = false;
    bool isNuked = false;
};

Worlds generateWorld(string name, int width, int height)
{
    Worlds world;
    world.name = name;
    world.width = width;
    world.height = height;
    world.items = new WorldItem[world.width * world.height];
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

std::stringstream serialize_world(Worlds world)
{
    std::stringstream str;
    {
        boost::archive::binary_oarchive oa(str);
        oa << world;
    }
    return str;
}

void FLUSH_WORLDS(Worlds world)
{
    std::cout << "am i reach ? " << std::endl;
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
        std::cout << worlds.at(i).name << std::endl;
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
               // canBeFree = false;
            }
        }
        if (canBeFree) //SAVE TO MYSQL AND DELETE THE ITEMS
        {
            FLUSH_WORLDS(worlds.at(i));
            delete[] worlds.at(i).items;
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
        Worlds wld;
        std::istream *blobdata = WORLD_DATA(world);
        {
            boost::archive::binary_iarchive ia(*blobdata); //crashes at this line
            ia >> wld;
        }
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
            //just sendworld
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

void sendWorld(ENetPeer *peer, Worlds *wld)
{
    //TODO
}