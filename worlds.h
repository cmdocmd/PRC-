#pragma once

#include <iostream>

#include "enet/enet.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/serialization/array.hpp>

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

class Magplant
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &x &y &id &amount &state &active &all_can_use;
    }

public:
    int x;
    int y;
    int id;
    int amount;
    int state;
    int active;
    int all_can_use;
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

class Dblocks
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &x &y &id;
    }

public:
    int x;
    int y;
    int id;
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

class Mannequine
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &x &y &clothHair &clothHead &clothMask &clothHand &clothNeck &clothShirt &clothPants &clothFeet &clothBack
            &text;
    }

public:
    int x;
    int y;
    int clothHair;
    int clothHead;
    int clothMask;
    int clothHand;
    int clothNeck;
    int clothShirt;
    int clothPants;
    int clothFeet;
    int clothBack;
    std::string text;
};

class Worlds
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &Width &Height &Name &items &owner &weather &id &dropsize &dropcount &pbm &dmb &isPublic &isNuked
            &jammed &pjammer &droppeditems &entrance &sign &safe &door &bans &access &locks &dblock &man &magplant;
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
    int pbm = 0;
    int dmb = 0;
    bool isPublic = true;
    bool isNuked = false;
    bool jammed = false;
    bool pjammer = false;
    std::vector<DroppedItem> droppeditems;
    std::vector<Entrances> entrance;
    std::vector<Signs> sign;
    std::vector<safevault> safe;
    std::vector<Doors> door;
    std::vector<WorldBans> bans;
    std::vector<std::string> access;
    std::vector<Lock> locks;
    std::vector<Dblocks> dblock;
    std::vector<Mannequine> man;
    std::vector<Magplant> magplant;
    Worlds GenerateWorld(std::string name);
};

struct AWorld
{
    Worlds *ptr;
    Worlds info;
    int id;
};

std::vector<Worlds> GetRandomWorlds(Server *server);
int getPlayersCountInWorld(ENetHost *Host, std::string name);
void SendWorldOffers(Server *server, ENetHost *Host, ENetPeer *peer);
AWorld GET_WORLD(Server *server, std::string world);
Worlds *getPlyersWorld(Server *server, ENetPeer *peer);
void sendWorld(ENetPeer *peer, Worlds *world);
void joinWorld(Server *server, ENetHost *Host, ENetPeer *peer, std::string act, int x2, int y2);
void Nothing(ENetPeer *peer, int x, int y);
void OnPlace(int x, int y, int tile, Worlds *world, ENetPeer *peer, ENetHost *Host, Server *server);
void OnPunch(int x, int y, Worlds *world, ENetPeer *peer, ENetHost *Host, Server *server);
void onWrench(Worlds *world, int x, int y, ENetPeer *peer);
