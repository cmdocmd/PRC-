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
        ar &username &password &mac &country &currentWorld &name &guild &skinColor &netID &userID &back &hand &feet
            &face &hair &shirt &pants &ances &neck &mask &cpx &cpy &x &y &level &xp &to_break &invSize &adminLevel &effect &dropid &gem
                &ban &bantime &mute &mutetime &curse &cursetime &inAccount &InLobby &usingDialog &isCorrect &isRotatedLeft &joinClothesUpdated &canWalkInBlocks
                    &canDoubleJump &isInvisible &noHands &noEyes &noBody &devilHorns &goldenHalo &isFrozen &isCursed &isDuctaped &haveCigar &isShining &isZombie
                        &isHitByLava &haveHauntedShadows &haveGeigerRadiation &haveReflector &isEgged &havePineappleFloag &haveFlyingPineapple &haveSuperSupporterName
                            &haveSupperPineapple &inv &friends &worlds &wrenchx &wrenchy;
    }

public:
    string username;
    string password;
    string mac;
    string country;
    string currentWorld;
    string name;
    string guild;
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
    int ances = 0;
    int neck = 0;
    int mask = 0;
    int cpx;
    int cpy;
    int x;
    int y;
    int level;
    int xp;
    int to_break;
    int invSize = 100;
    int adminLevel = 0;
    int effect = 0;
    int dropid = 0;
    int gem = 0;
    int ban = 0;
    int bantime = 0;
    int mute = 0;
    int mutetime = 0;
    int curse = 0;
    int cursetime = 0;
    bool inAccount = false;
    bool InLobby = false;
    bool usingDialog = false;
    bool isCorrect = false;
    bool isRotatedLeft = false;
    bool joinClothesUpdated = false;
    bool canWalkInBlocks = false;
    bool canDoubleJump = false;
    bool isInvisible = false;
    bool noHands = false;
    bool noEyes = false;
    bool noBody = false;
    bool devilHorns = false;
    bool goldenHalo = false;
    bool isFrozen = false;
    bool isCursed = false;
    bool isDuctaped = false;
    bool haveCigar = false;
    bool isShining = false;
    bool isZombie = false;
    bool isHitByLava = false;
    bool haveHauntedShadows = false;
    bool haveGeigerRadiation = false;
    bool haveReflector = false;
    bool isEgged = false;
    bool havePineappleFloag = false;
    bool haveFlyingPineapple = false;
    bool haveSuperSupporterName = true;
    bool haveSupperPineapple = false;
    std::vector<PlayerInventory> inv;
    std::vector<string> friends;
    std::vector<string> worlds;
    int wrenchx;
    int wrenchy;
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

void Reset_Values(ENetPeer *peer)
{
    pinfo(peer)->usingDialog = false;
    pinfo(peer)->inAccount = true;
    pinfo(peer)->InLobby = false;
    pinfo(peer)->isCorrect = false;
}