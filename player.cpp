#pragma once
#include <iostream>
#include "enet/include/enet/enet.h"
#include <string.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>

using std::string;

class PlayerInventory
{
private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &itemID &itemCount;
    }

public:
    int itemID;
    int itemCount;
};

class player
{
private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &username &password &mac &country &currentWorld &name &skinColor &netID &userID &back &hand &feet
            &face &hair &shirt &pants &ances &neck &mask &cpx &cpy &x &y &invSize &adminLevel &inAccount &InLobby &usingDialog
                &isRotatedLeft &joinClothesUpdated &inv &friends &worlds;
    }

public:
    string username;
    string password;
    string mac;
    string country;
    string currentWorld;
    string name;
    int skinColor = 0x8295C3FF;
    int netID;
    int userID;
    int back = 0;
    int hand = 0;
    int feet = 0;
    int face = 0;
    int hair = 0;
    int shirt = 0;
    int pants = 0;
    int ances= 0;
    int neck = 0;
    int mask= 0;
    int cpx;
    int cpy;
    int x;
    int y;
    int invSize = 100;
    int adminLevel = 0;
    bool inAccount = false;
    bool InLobby = false;
    bool usingDialog = false;
    bool isRotatedLeft = false;
    bool joinClothesUpdated = false;
    std::vector<PlayerInventory> inv;
    std::vector<string> friends;
    std::vector<string> worlds;
};

player *pinfo(ENetPeer *peer)
{
    return (player *)(peer->data);
}

std::stringstream serialize_player(player *ply)
{
    std::stringstream str;
    {
        boost::archive::binary_oarchive oa(str);
        oa << ply;
    }
    return str;
}
