#pragma once
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include <boost/algorithm/string.hpp>
#include <array>
#include <iostream>
#include <sstream>
#include "player.cpp"
#include "utils.cpp"
#include "GamePacketBuilder.cpp"
#include <chrono>
#include "Itemsbuilder.cpp"
#include "visuals.cpp"

int cId = 1;

typedef unsigned char BYTE;

bool WORLD_EXIST(std::string world);
std::istream *WORLD_DATA(std::string world);
void INSERT_WORLD(std::string name, std::stringstream blob);
void UPDATE_WORLD(std::stringstream str, std::string name);

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
    string is_open;
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
    string text;
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
    string pass;
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
    string is_open;
    string dest;
    string label;
    string id;
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
    string name;
    int time;
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
        ar &width &height &name &items &owner &weather &id &dropsize &dropcount &isPublic &isNuked
            &entrance_size &sign_size &safe_size &door_size &droppeditems &entrance &sign &safe &door
                &bans &access;
    }

public:
    int width;
    int height;
    std::string name;
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
    std::vector<string> access;
};

Worlds generateWorld(std::string name, int width, int height)
{
    Worlds world;
    world.name = name;
    world.width = width;
    world.height = height;
    for (int i = 0; i < world.width * world.height; i++)
    {
        if (i >= 3800 && i < 5400 && !(rand() % 50))
        {
            world.items[i % width][i / width].foreground = 10;
        }
        else if (i >= 3700 && i < 5400)
        {
            if (i > 5000)
            {
                if (i % 7 == 0)
                {
                    world.items[i % width][i / width].foreground = 4;
                }
                else
                {
                    world.items[i % width][i / width].foreground = 2;
                }
            }
            else
            {
                world.items[i % width][i / width].foreground = 2;
            }
        }
        else if (i >= 5400)
        {
            world.items[i % width][i / width].foreground = 8;
        }
        if (i >= 3700)
            world.items[i % width][i / width].background = 14;
        if (i == 3650)
            world.items[i % width][i / width].foreground = 6;
        else if (i >= 3600 && i < 3700)
            world.items[i % width][i / width].foreground = 0;
        if (i == 3750)
            world.items[i % width][i / width].foreground = 8;
    }
    return world;
}

std::vector<Worlds> worlds;

std::stringstream serialize_world(Worlds const &world)
{
    std::stringstream str;
    {
        boost::archive::binary_oarchive oa(str);
        oa << world;
    }
    return str;
}

Worlds deserialize(std::string world)
{
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

void SAVE_WORLDS()
{
    for (int i = 0; i < static_cast<int>(worlds.size()); i++)
    {
        std::cout << worlds.at(i).name << std::endl;
        FLUSH_WORLDS(worlds.at(i));
        worlds.erase(worlds.begin() + i);
        i--;
    }
}

struct AWorld
{
    Worlds *ptr;
    Worlds info;
    int id;
};

AWorld GET_WORLD(std::string world)
{
    //WHEN A GUY JOIN A WORLD WE GOTTA FREE THE UNUSED WORLDS BY CALLING SAVE_WORLDS
    if (static_cast<int>(worlds.size()) > 20)
    {
        SAVE_WORLDS();
    }
    AWorld ret;
    boost::to_upper(world);
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

Worlds *getPlyersWorld(ENetPeer *peer)
{
    try
    {
        return GET_WORLD(pinfo(peer)->currentWorld).ptr;
    }
    catch (int e)
    {
        return NULL;
    }
}

void pushsign(ENetPeer *peer, string text)
{
    Worlds *world = getPlyersWorld(peer);
    if (world == NULL)
        return;
    Signs sin;
    sin.text = text;
    sin.x = pinfo(peer)->wrenchx;
    sin.y = pinfo(peer)->wrenchy;
    world->sign.push_back(sin);
    world->safe_size = world->sign.size();
}

void pushdoor(ENetPeer* peer, string dest, string label, string id, string iop) {
	Worlds* world = getPlyersWorld(peer);
	if (world == NULL) return;
	Doors dr;
	dr.dest = dest;
	dr.label = label;
	dr.id = id;
	dr.is_open = iop;
	dr.x = pinfo(peer)->wrenchx;
	dr.y = pinfo(peer)->wrenchy;
	world->door.push_back(dr);
	world->door_size = world->door.size();
}

void sendWorld(ENetPeer *peer, Worlds *world)
{

    pinfo(peer)->joinClothesUpdated = false;

    int xSize = world->width;
    int ySize = world->height;
    int square = xSize * ySize;
    int namelen = world->name.length();

    int alloc = (8 * square);
    int total = 78 + namelen + square + 24 + alloc + (world->dropsize * 16);

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

        memcpy(blc + 2, &world->items[i % xSize][i / xSize].background, 2);
        int type = 0x00000000;
        // type 1 = locked
        if (world->items[i % xSize][i / xSize].water)
            type |= 0x04000000;
        if (world->items[i % xSize][i / xSize].glue)
            type |= 0x08000000;
        if (world->items[i % xSize][i / xSize].fire)
            type |= 0x10000000;
        if (world->items[i % xSize][i / xSize].red)
            type |= 0x20000000;
        if (world->items[i % xSize][i / xSize].green)
            type |= 0x40000000;
        if (world->items[i % xSize][i / xSize].blue)
            type |= 0x80000000;

        memcpy(blc + 4, &type, 4);
        blc += 8;
    }

    int totalitemdrop = world->droppeditems.size();
    int xd = world->dropcount - 1;
    memcpy(blc, &totalitemdrop, 2);
    memcpy(blc + 4, &xd, 2);
    if (totalitemdrop != 0)
    {
        blc += 8;
        for (int i = 0; i < static_cast<int>(world->droppeditems.size()); i++)
        {
            memcpy(blc, &world->droppeditems[i].id, 2);
            memcpy(blc + 2, &world->droppeditems[i].x, 4);
            memcpy(blc + 6, &world->droppeditems[i].y, 4);
            memcpy(blc + 10, &world->droppeditems[i].count, 1);
            memcpy(blc + 12, &world->droppeditems[i].uid, 2);
            blc += 16;
        }
    }

    ENetPacket *packetw = enet_packet_create((void *)&data[0], total, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packetw);

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
        data.plantingTree = world->items[i % xSize][i / xSize].foreground;
        SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
    }
    for (int i = 0; i < static_cast<int>(world->sign.size()); i++)
    {
        int x = world->sign[i].x;
        int y = world->sign[i].y;
        int fg = world->items[x][y].foreground;
        int bg = world->items[x][y].background;
        updatesign(peer, fg, bg, x, y, world->sign[i].text);
    }
    pinfo(peer)->currentWorld = world->name;
    if (world->owner != "")
    {
        Packets::consoleMessage(peer, "`#[`0" + world->name + " `9World Locked by " + world->owner + "`#]");
    }
}

void joinWorld(ENetHost *server, ENetPeer *peer, string act, int x2, int y2)
{
    try
    {
        if (act.length() > 18)
        {

            return;
        }
        else
        {
            Worlds wld;
            wld = GET_WORLD(act).info;
            bool cursed = false;
            if (wld.isNuked)
            {
                if (pinfo(peer)->adminLevel >= 666)
                {
                    Packets::consoleMessage(peer, "This world is inaccessable `2(You have access to enter)");
                }
                else
                {
                    Packets::consoleMessage(peer, "This world is inaccessable");
                    Packets::onfailedtoenterworld(peer);
                    return;
                }
            }
            for (int i = 0; i < static_cast<int>(wld.bans.size()); i++)
            {
                if (wld.bans[i].name == pinfo(peer)->username)
                {
                    int now = time(NULL);
                    if (now - wld.bans[i].time < 3600)
                    {
                        Packets::consoleMessage(peer, "`4Oh no! `oYou've been banned from that world by its owner! Try again later after the world ban wears off.");
                        Packets::onfailedtoenterworld(peer);
                        return;
                    }
                }
            }
            if (pinfo(peer)->curse != 0)
            {
                int to = pinfo(peer)->cursetime;
                time_t now = time(NULL);
                time_t curse = pinfo(peer)->curse;
                if (now - curse > to)
                {
                    pinfo(peer)->curse = 0;
                    pinfo(peer)->cursetime = 0;
                    Packets::consoleMessage(peer, "`$Your curse have been removed please behave better this time");
                }
                else
                {
                    cursed = true;
                }
            }
            if (cursed)
            {
                wld = GET_WORLD("HELL").info;
            }
            sendWorld(peer, &wld);
            int x = 3040;
            int y = 736;

            for (int j = 0; j < wld.width * wld.height; j++)
            {
                if (wld.items[j % wld.width][j / wld.width].foreground == 6)
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
            sendInventory(peer);
        }
    }
    catch (int e)
    {
        if (e == 1)
        {
            pinfo(peer)->currentWorld == "EXIT";
            Packets::onfailedtoenterworld(peer);
            Packets::consoleMessage(peer, "You have exited the world.");
            return;
        }
        else if (e == 2)
        {
            pinfo(peer)->currentWorld == "EXIT";
            Packets::onfailedtoenterworld(peer);
            Packets::consoleMessage(peer, "You have entered bad characters in the world name!");
            return;
        }
        else if (e == 3)
        {
            pinfo(peer)->currentWorld == "EXIT";
            Packets::onfailedtoenterworld(peer);
            Packets::consoleMessage(peer, "Exit from what? Click back if you're done playing.");
            return;
        }
        else
        {
            pinfo(peer)->currentWorld == "EXIT";
            Packets::onfailedtoenterworld(peer);
            Packets::consoleMessage(peer, "I know this menu is magical and all, but it has its limitations! You can't visit this world!");
            return;
        }
    }
}

void SendWorldOffers(ENetPeer *peer)
{
    if (!pinfo(peer)->InLobby)
    {
        return;
    }
    Packets::requestWorldSelectMenu(peer, "");
}

void Nothing(ENetPeer *peer, int x, int y)
{
    PlayerMoving data;
    data.netID = pinfo(peer)->netID;
    data.packetType = 0x8;
    data.plantingTree = 0;
    data.netID = -1;
    data.x = x;
    data.y = y;
    data.punchX = x;
    data.punchY = y;
    SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
}

void OnPlace(int x, int y, int tile, Worlds *world, ENetPeer *peer, ENetHost *server)
{
    bool lock = false;
    if (tile > static_cast<int>(itemDefs.size()))
    {
        return;
    }
    if (itemDefs[tile].blockType == BlockTypes::LOCK)
    {
        if (tile == 202 || tile == 204 || tile == 206)
        {
            if (world->owner != "")
            {
                if (pinfo(peer)->username != world->owner)
                {
                    Nothing(peer, x, y);
                    return;
                }
            }
        }
        else
        {
            if (world->owner != "")
            {
                Nothing(peer, x, y);
                return;
            }
            else
            {
                world->owner = pinfo(peer)->username;
                world->isPublic = false;
                world->id = pinfo(peer)->userID;
                Packets::ontalkbubble(peer, pinfo(peer)->netID, "`5[`w" + world->name + " has been `&World Locked `wby " + pinfo(peer)->name);
                pinfo(peer)->worlds.push_back(world->name);
                lock = true;
            }
        }
    }
    if (itemDefs[tile].blockType == BlockTypes::CLOTHING)
    {
        Nothing(peer, x, y);
        return;
    }
    if (itemDefs[tile].blockType == BlockTypes::BACKGROUND)
    {
        world->items[x][y].background = tile;
    }
    else
    {
        if (world->items[x][y].foreground != 0)
        {
            return;
        }
        world->items[x][y].foreground = tile;
    }
    PlayerMoving data;
    data.packetType = 0x3;
    data.characterState = 0x0;
    data.x = x;
    data.y = y;
    data.punchX = x;
    data.punchY = y;
    data.XSpeed = 0;
    data.YSpeed = 0;
    data.netID = pinfo(peer)->netID;
    data.plantingTree = tile;
    ENetPeer *currentPeer;
    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (isHere(peer, currentPeer))
        {
            SendPacketRaw(4, packPlayerMoving(&data), 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
            if (lock)
            {
                sendmusic(currentPeer, "use_lock");
            }
        }
    }
}

void OnPunch(int x, int y, Worlds *world, ENetPeer *peer, ENetHost *server)
{
    int tile = (world->items[x][y].foreground == 0) ? world->items[x][y].background : world->items[x][y].foreground;
    if (tile == 0 || tile == 6864 || tile == 6 || tile == 8)
    {
        Nothing(peer, x, y);
        return;
    }
    std::cout << "Punch place: " << x + y * world->width << std::endl;
    std::cout << "x: " << x << " y: " << y << std::endl;
    int tool = pinfo(peer)->hand;
    PlayerMoving data;
    data.characterState = 0x0; // animation
    data.x = x;
    data.y = y;
    data.punchX = x;
    data.punchY = y;
    data.XSpeed = 0;
    data.YSpeed = 0;
    data.packetType = 0x8;
    data.plantingTree = (tool == 98 || tool == 1438 || tool == 4956) ? 8 : 6;
    data.netID = pinfo(peer)->netID;
    using namespace std::chrono;
    if ((duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world->items[x][y].breakTime >= 4000)
    {
        world->items[x][y].breakTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
        world->items[x][y].breakLevel = 0; // TODO
    }
    if (y < world->height)
    {
        world->items[x][y].breakTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
        world->items[x][y].breakLevel += (int)((tool == 98 || tool == 1438 || tool == 4956) ? 8 : 6); // TODO
    }
    int times = 0;
    if (itemDefs[tile].blockType == BlockTypes::LOCK)
    {
        times = 35;
    }
    if (tile != 0)
    {
        times = 7;
    }
    else
    {
        times = 6;
    }
    int breakhit = 0;

    if (itemDefs[tile].blockType == BlockTypes::LOCK && tile != 202 && tile != 202 && tile != 206)
    {
        breakhit = 30;
    }
    else if (tile == 5638)
    {
        breakhit = 35;
    }
    else
    {
        breakhit = itemDefs[tile].breakHits;
    }
    if (y < world->height && world->items[x][y].breakLevel >= breakhit * times)
    {
        data.packetType = 0x3;
        data.plantingTree = 18;
        world->items[x][y].breakLevel = 0;
        if (world->items[x][y].foreground != 0)
        {
            int block = world->items[x][y].foreground;
            if (itemDefs[world->items[x][y].foreground].blockType == BlockTypes::LOCK)
            {
                world->owner = "";
                world->id = 0;
                world->isPublic = true;
                int idx = -1;
                for (int i = 0; i < static_cast<int>(pinfo(peer)->worlds.size()); i++)
                {
                    if (pinfo(peer)->worlds[i] == world->name)
                    {
                        idx = i;
                    }
                }
                if (idx != -1)
                {
                    pinfo(peer)->worlds.erase(pinfo(peer)->worlds.begin() + idx);
                }
            }
            if (itemDefs[block].blockType == BlockTypes::DOOR)
            {
                int idx = -1;
                for (int i = 0; i < static_cast<int>(world->door.size()); i++)
                {
                    if (world->door[i].x == x && world->door[i].y == y)
                        idx = i;
                }
                if (idx != -1)
                {
                    world->door.erase(world->door.begin() + idx);
                    world->door_size = world->door.size();
                }
            }
            world->items[x][y].foreground = 0;
        }
        else
        {
            world->items[x][y].background = 6864;
        }
    }
    ENetPeer *currentPeer;
    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (isHere(peer, currentPeer))
        {
            SendPacketRaw(4, packPlayerMoving(&data), 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
        }
    }
}

void onWrench(Worlds *world, int x, int y, ENetPeer *peer)
{
    int block = world->items[x][y].foreground;
    if (itemDefs[block].blockType == BlockTypes::SIGN || block == 1420 || block == 6214)
    {
        pinfo(peer)->wrenchx = x;
        pinfo(peer)->wrenchy = y;
        string text = "";
        for (int i = 0; i < static_cast<int>(world->sign.size()); i++)
        {
            if (world->sign[i].x == x && world->sign[i].y == y)
            {
                text = world->sign[i].text;
            }
        }
        Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wEdit " + itemDefs.at(world->items[x][y].foreground).name + "``|left|" + std::to_string(world->items[x][y].foreground) + "|\n\nadd_textbox|`oWhat would you like to write on this sign?|\nadd_text_input|sign||" + text + "|100|\nend_dialog|signok|Cancel|OK|");
    }
    if (itemDefs[block].blockType == BlockTypes::DOOR)
    {
        string dest = "", label = "", id = "", iop = "0", text = "ID (optional)";
        pinfo(peer)->wrenchx = x;
        pinfo(peer)->wrenchy = y;
        for (int i = 0; i < static_cast<int>(world->door.size()); i++)
        {
            if (world->door[i].x == x && world->door[i].y == y)
            {
                dest = world->door[i].dest;
                label = world->door[i].label;
                id = world->door[i].id;
                iop = world->door[i].is_open;
            }
        }
        if (block == 762)
            text = "Password";
        Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wEdit " + itemDefs[block].name + "``|left|" + std::to_string(block) + "||\n\nadd_text_input|dest|`oTarget World|" + dest + "|100|\nadd_text_input|label|Display Label (optional)|" + label + "|100|\nadd_text_input|doorid|" + text + "|" + id + "|35|\nadd_checkbox|isopenpublic|is open to public|" + iop + "\nend_dialog|editdoor|Cancel|OK|");
    }
}

void DropItem(ENetHost *server, ENetPeer *peer, int netID, float x, float y, int item, int count, BYTE specialEffect)
{
    Worlds *world = getPlyersWorld(peer);
    if (item > static_cast<int>(itemDefs.size()) || world == NULL || item < 0)
    {
        return;
    }
    DroppedItem itemDropped;
    itemDropped.id = item;
    itemDropped.count = count;
    itemDropped.x = x;
    itemDropped.y = y;
    itemDropped.uid = world->dropcount++;
    world->droppeditems.push_back(itemDropped);
    world->dropsize = world->droppeditems.size();
    sendDrop(server, peer, netID, x, y, item, count, specialEffect);
}