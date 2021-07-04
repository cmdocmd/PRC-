#pragma once

#include <iostream>
#include "enet/enet.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/algorithm/string.hpp>

class Trade
{

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &itemid &count;
    }

public:
    int itemid;
    int count;
};

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

class Players
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &username &password &email &mac &country &currentWorld &name &guild &tradingme &itemlist &lastmsgworld &lastmsger &lastplayerwrench &lastFrnWorld &lastFrn &lastfriend
            &netID &userID &back &hand &feet &face &hair &shirt &pants &ances &neck &mask &cpx &cpy &x &y &level
            &xp &to_break &invSize &adminLevel &effect &dropid &gem &lasttradeid &ban &bantime &mute &mutetime &curse &cursetime
            &wrenchx &wrenchy &skinColor &warpme &geigerx &geigery &mgid &inAccount &InLobby &usingDialog &isCorrect &isRotatedLeft &joinClothesUpdated
            &canWalkInBlocks &canDoubleJump &isInvisible &noHands &noEyes &noBody &devilHorns &goldenHalo &isFrozen &isCursed
            &isDuctaped &haveCigar &isShining &isZombie &isHitByLava &haveHauntedShadows &haveGeigerRadiation &haveReflector &isEgged
            &havePineappleFloag &haveFlyingPineapple &haveSuperSupporterName &haveSupperPineapple &usingDoor &GeigerCooldown &trading &accepted &dotrade &canceled &inv &trade &friends &worlds;
    }

public:
    std::string username = "", password = "", email = "", mac = "", country = "", currentWorld = "EXIT", name = "", guild = "",
                tradingme = "", itemlist = "", lastmsgworld = "", lastmsger = "", lastplayerwrench = "", lastFrnWorld = "", lastFrn = "", lastfriend = "";

    int netID = 0, userID = 0, back = 0, hand = 0, feet = 0, face = 0, hair = 0, shirt = 0,
        pants = 0, ances = 0, neck = 0, mask = 0, cpx = 0, cpy = 0, x = 0, y = 0, level = 0,
        xp = 0, to_break = 500, invSize = 100, adminLevel = 0, effect = 0, dropid = 0, gem = 0, lasttradeid = 0, ban = 0,
        bantime = 0, mute = 0, mutetime = 0, curse = 0, cursetime = 0, wrenchx = 0, wrenchy = 0, skinColor = 0x8295C3FF, warpme = 0,
        geigerx = 0, geigery = 0, mgid = 0;

    bool inAccount = false, InLobby = false, usingDialog = false, isCorrect = false, isRotatedLeft = false, joinClothesUpdated = false,
         canWalkInBlocks = false, canDoubleJump = false, isInvisible = false, noHands = false, noEyes = false, noBody = false, devilHorns = false,
         goldenHalo = false, isFrozen = false, isCursed = false, isDuctaped = false, haveCigar = false, isShining = false, isZombie = false, isHitByLava = false,
         haveHauntedShadows = false, haveGeigerRadiation = false, haveReflector = false, isEgged = false, havePineappleFloag = false, haveFlyingPineapple = false,
         haveSuperSupporterName = false, haveSupperPineapple = false, usingDoor = false, GeigerCooldown = false, trading = false, accepted = false, dotrade = false, canceled = false;

    std::vector<PlayerInventory> inv;
    std::vector<Trade> trade;
    std::vector<std::string> friends;
    std::vector<std::string> worlds;
};

Players *pinfo(ENetPeer *peer);
Players *Desrialize(std::istream *blobdata);
std::stringstream serialize_player(Players *ply);