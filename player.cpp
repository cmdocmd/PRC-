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

class player
{
private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &username;
        ar &password;
        ar &mac;
        ar &country;
        ar &currentWorld;
        ar &name;
        ar &netID;
        ar &userID;
        ar &back;
        ar &hand;
        ar &feet;
        ar &face;
        ar &hair;
        ar &shirt;
        ar &pants;
        ar &ances;
        ar &neck;
        ar &mask;
        ar &cpx;
        ar &cpy;
        ar &x;
        ar &y;
        ar &inAccount;
        ar &InLobby;
        ar &usingDialog;
    }

public:
    string username;
    string password;
    string mac;
    string country;
    string currentWorld;
    string name;
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
    bool inAccount = false;
    bool InLobby = false;
    bool usingDialog = false;
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
