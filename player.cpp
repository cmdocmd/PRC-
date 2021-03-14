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
            &face &hair &shirt &pants &ances &neck &mask &cpx &cpy &x &y &invSize &inAccount &InLobby &usingDialog
                &isRotatedLeft &joinClothesUpdated &inv;
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
    int back;
    int hand;
    int feet;
    int face;
    int hair;
    int shirt;
    int pants;
    int ances;
    int neck;
    int mask;
    int cpx;
    int cpy;
    int x;
    int y;
    int invSize = 100;
    bool inAccount = false;
    bool InLobby = false;
    bool usingDialog = false;
    bool isRotatedLeft = false;
    bool joinClothesUpdated = false;
    std::vector<PlayerInventory> inv;
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
