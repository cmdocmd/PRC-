#pragma once

#include <iostream>

#include "enet/enet.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/algorithm/string.hpp>


class Server;

class Entrances
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &x &y &is_open;
    }

public:
    int x;
    int y;
    std::string is_open;
};

class Signs
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &x &y &text;
    }

public:
    int x;
    int y;
    std::string text;
};

class safevault
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &x &y &id &count &pass;
    }

public:
    int x;
    int y;
    int id;
    int count;
    std::string pass;
};

class Doors
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &x &y &is_open &dest &label &id;
    }

public:
    int x;
    int y;
    std::string is_open;
    std::string dest;
    std::string label;
    std::string id;
};

class DroppedItem
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &id &uid &count &x &y;
    }

public:
    int id;
    int uid;
    int count;
    float x;
    float y;
};

class WorldBans
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &name &time;
    }

public:
    std::string name;
    int time;
};

class Lock
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &x &y &LockX &LockY &LockID &OwnerID &Public &
            access;
    }

public:
    int x;
    int y;
    int LockX;
    int LockY;
    int LockID;
    int OwnerID;
    bool Public;
    std::vector<std::string> access;
};

class WorldItem
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &foreground &background &breakLevel &breakTime &water &fire &glue &
            red &green &blue;
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
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &Width &Height &Name &items &owner &weather &id &dropsize &dropcount &isPublic &isNuked
            &entrance_size &sign_size &safe_size &door_size &droppeditems &entrance &sign &safe &door
                &bans &access &locks;
    }

public:
    int Width;
    int Height;
    std::string Name;
    WorldItem items[100][60];
    std::string owner = "";
    int weather = 0;
    int id;
    int dropsize = 0;
    int dropcount = 1;
    bool isPublic = true;
    bool isNuked = false;
    int entrance_size = 0;
    int sign_size = 0;
    int safe_size = 0;
    int door_size = 0;
    std::vector<DroppedItem> droppeditems;
    std::vector<Entrances> entrance;
    std::vector<Signs> sign;
    std::vector<safevault> safe;
    std::vector<Doors> door;
    std::vector<WorldBans> bans;
    std::vector<std::string> access;
    std::vector<Lock> locks;
    Worlds GenerateWorld(std::string name);
};

Worlds GET_WORLD(Server *server, std::string name);
void sendWorld(ENetPeer *peer, Worlds world);
void joinWorld(Server *server, ENetHost *Host, ENetPeer *peer, std::string act, int x2, int y2);
std::vector<Worlds> GetRandomWorlds(Server *server);
int getPlayersCountInWorld(ENetHost *Host, std::string name);
void SendWorldOffers(Server *server, ENetHost * Host, ENetPeer *peer);
Worlds getPlyersWorld(Server *server, ENetPeer *peer);