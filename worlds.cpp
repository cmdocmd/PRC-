#include <iostream>
#include <string.h>
#include <vector>

#include "worlds.h"
#include "mysql.h"
#include "packets.h"
#include "server.h"
#include "utils.h"

#include "enet/enet.h"

#include <boost/algorithm/string.hpp>

int cId = 0;

typedef unsigned char BYTE;

Worlds Worlds::GenerateWorld(std::string name)
{
    Worlds world;
    world.Name = name;
    world.Width = 100;
    world.Height = 60;
    for (int i = 0; i < world.Width * world.Height; i++)
    {
        if (i >= 3800 && i < 5400 && !(rand() % 50))
        {
            world.items[i % world.Width][i / world.Width].foreground = 10;
        }
        else if (i >= 3700 && i < 5400)
        {
            if (i > 5000)
            {
                if (i % 7 == 0)
                {
                    world.items[i % world.Width][i / world.Width].foreground = 4;
                }
                else
                {
                    world.items[i % world.Width][i / world.Width].foreground = 2;
                }
            }
            else
            {
                world.items[i % world.Width][i / world.Width].foreground = 2;
            }
        }
        else if (i >= 5400)
        {
            world.items[i % world.Width][i / world.Width].foreground = 8;
        }
        if (i >= 3700)
            world.items[i % world.Width][i / world.Width].background = 14;
        if (i == 3650)
            world.items[i % world.Width][i / world.Width].foreground = 6;
        else if (i >= 3600 && i < 3700)
            world.items[i % world.Width][i / world.Width].foreground = 0;
        if (i == 3750)
            world.items[i % world.Width][i / world.Width].foreground = 8;
    }
    return world;
}

Worlds GET_WORLD(Server *server, std::string name)
{
    Worlds world;
    world.Name = "@";
    boost::to_upper(name);
    if (name.length() >= 1 && name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") == std::string::npos && name != "EXIT")
    {
        for (int i = 0; i < static_cast<int>(server->m_worlds.size()); i++)
        {
            if (server->m_worlds[i].Name == name)
            {
                world = server->m_worlds[i];
            }
        }
        if (WORLD_EXIST(name) && world.Name == "@")
        {
            world = server->deserialize(name);
            server->m_worlds.push_back(world);
        }
        if (!WORLD_EXIST(name) && world.Name == "@")
        {
            world = world.GenerateWorld(name);
            server->m_worlds.push_back(world);
        }
    }
    return world;
}

void sendWorld(ENetPeer *peer, Worlds world)
{
    Packets p;
    pinfo(peer)->joinClothesUpdated = false;

    int xSize = world.Width;
    int ySize = world.Height;
    int square = xSize * ySize;
    int namelen = world.Name.length();

    int alloc = (8 * square);
    int total = 78 + namelen + square + 24 + alloc + (world.dropsize * 16);

    //BYTE *data = new BYTE[total];
    std::vector<BYTE> data(total);
    int s1 = 4, s3 = 8, zero = 0;

    //memset(data, 0, total);
    memcpy(&data[0], &s1, 1);
    memcpy(&data[4], &s1, 1);
    memcpy(&data[16], &s3, 1);
    memcpy(&data[66], &namelen, 1);
    memcpy(&data[68], world.Name.c_str(), namelen);
    memcpy(&data[68] + namelen, &xSize, 1);
    memcpy(&data[72] + namelen, &ySize, 1);
    memcpy(&data[76] + namelen, &square, 2);
    BYTE *blc = &data[80 + namelen];
    for (int i = 0; i < square; i++)
    {
        //removed cus some of blocks require tile extra and it will crash the world without
        memcpy(blc, &zero, 2);

        memcpy(blc + 2, &world.items[i % xSize][i / xSize].background, 2);
        int type = 0x00000000;
        // type 1 = locked
        if (world.items[i % xSize][i / xSize].water)
            type |= 0x04000000;
        if (world.items[i % xSize][i / xSize].glue)
            type |= 0x08000000;
        if (world.items[i % xSize][i / xSize].fire)
            type |= 0x10000000;
        if (world.items[i % xSize][i / xSize].red)
            type |= 0x20000000;
        if (world.items[i % xSize][i / xSize].green)
            type |= 0x40000000;
        if (world.items[i % xSize][i / xSize].blue)
            type |= 0x80000000;

        memcpy(blc + 4, &type, 4);
        blc += 8;
    }

    int totalitemdrop = world.droppeditems.size();
    int xd = world.dropcount - 1;
    memcpy(blc, &totalitemdrop, 2);
    memcpy(blc + 4, &xd, 2);
    if (totalitemdrop != 0)
    {
        blc += 8;
        for (int i = 0; i < static_cast<int>(world.droppeditems.size()); i++)
        {
            memcpy(blc, &world.droppeditems[i].id, 2);
            memcpy(blc + 2, &world.droppeditems[i].x, 4);
            memcpy(blc + 6, &world.droppeditems[i].y, 4);
            memcpy(blc + 10, &world.droppeditems[i].count, 1);
            memcpy(blc + 12, &world.droppeditems[i].uid, 2);
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
        data.x = i % world.Width;
        data.y = i / world.Height;
        data.punchX = i % world.Width;
        data.punchY = i / world.Width;
        data.XSpeed = 0;
        data.YSpeed = 0;
        data.netID = -1;
        data.plantingTree = world.items[i % xSize][i / xSize].foreground;
        SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
    }
    /*for (int i = 0; i < static_cast<int>(world.sign.size()); i++)
    {
        int x = world.sign[i].x;
        int y = world.sign[i].y;
        int fg = world.items[x][y].foreground;
        int bg = world.items[x][y].background;
        updatesign(peer, fg, bg, x, y, world.sign[i].text);
    }*/
    pinfo(peer)->currentWorld = world.Name;
    if (world.owner != "")
    {
        p.consoleMessage(peer, "`#[`0" + world.Name + " `9World Locked by " + world.owner + "`#]");
    }
}

void joinWorld(Server *server, ENetHost *Host, ENetPeer *peer, std::string act, int x2, int y2)
{
    if (act.length() > 18)
    {
        return;
    }
    Packets p;
    Worlds world = GET_WORLD(server, act);
    std::cout << world.Name << "\n";
    bool cursed = false;
    if (world.Name != "@")
    {
        if (world.isNuked)
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                p.consoleMessage(peer, "This world is inaccessable `2(You have access to enter)");
            }
            else
            {
                p.consoleMessage(peer, "This world is inaccessable");
                p.onfailedtoenterworld(peer);
                return;
            }
        }
        for (int i = 0; i < static_cast<int>(world.bans.size()); i++)
        {
            if (world.bans[i].name == pinfo(peer)->username)
            {
                int now = time(NULL);
                if (now - world.bans[i].time < 3600)
                {
                    p.consoleMessage(peer, "`4Oh no! `oYou've been banned from that world by its owner! Try again later after the world ban wears off.");
                    p.onfailedtoenterworld(peer);
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
                p.consoleMessage(peer, "`$Your curse have been removed please behave better this time");
            }
            else
            {
                cursed = true;
            }
        }
        if (cursed)
        {
            world = GET_WORLD(server, "HELL");
        }
    }
    else
    {
        p.onfailedtoenterworld(peer);
        return;
    }
    sendWorld(peer, world);
    int x = 0;
    int y = 0;

    for (int j = 0; j < world.Width * world.Height; j++)
    {
        if (world.items[j % world.Width][j / world.Width].foreground == 6)
        {
            x = (j % world.Width) * 32;
            y = (j / world.Width) * 32;
        }
    }
    pinfo(peer)->x = x;
    pinfo(peer)->y = y;
    pinfo(peer)->cpx = x;
    pinfo(peer)->cpy = y;
    if (x2 != 0 && y2 != 0)
    {
        x = x2;
        y = y2;
    }
    int uid = pinfo(peer)->userID; //TODO DONT READ THE FLAG OF THE PACKET BUT FROM DATABASE
    p.onSpawn(peer, "spawn|avatar\nnetID|" + std::to_string(cId) + "\nuserID|" + std::to_string(uid) + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(x) + "|" + std::to_string(y) + "\nname|``" + pinfo(peer)->name + "``\ncountry|" + pinfo(peer)->country + "\ninvis|0\nmstate|0\nsmstate|0\ntype|local\n");
    pinfo(peer)->netID = cId;
    onPeerConnect(Host, peer);
    cId++;
    sendInventory(peer);
}

std::vector<Worlds> GetRandomWorlds(Server *server)
{
    std::vector<Worlds> world;
    int num = static_cast<int>(server->m_worlds.size()) < 14 ? static_cast<int>(server->m_worlds.size()) : 14;
    for (int j = 0; j < num; j++)
    {
        bool isPossible = true;
        Worlds wld = server->m_worlds[rand() % num];
        for (int i = 0; i < static_cast<int>(world.size()); i++)
        {
            if (wld.Name == world[i].Name || wld.Name == "EXIT")
            {
                isPossible = false;
            }
        }
        if (isPossible)
            world.push_back(wld);
    }
    return world;
}

int getPlayersCountInWorld(ENetHost *Host, std::string name)
{
    int count = 0;
    ENetPeer *currentPeer;
    for (currentPeer = Host->peers; currentPeer < &Host->peers[Host->peerCount]; ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (((Players *)(currentPeer->data))->currentWorld == name)
            count++;
    }
    return count;
}

void SendWorldOffers(Server *server, ENetHost *Host, ENetPeer *peer)
{
    if (!pinfo(peer)->InLobby)
    {
        return;
    }
    Packets p;
    std::vector<Worlds> worlds = GetRandomWorlds(server);
    std::string worldOffers = "default|";
    if (static_cast<int>(worlds.size() > 0))
    {
        worldOffers += worlds[0].Name;
    }

    worldOffers += "\nadd_button|Showing: `wWorlds``|_catselect_|0.6|3529161471|\n";
    for (int i = 0; i < static_cast<int>(worlds.size()); i++)
    {
        worldOffers += "add_floater|" + worlds[i].Name + "|" + std::to_string(getPlayersCountInWorld(Host, worlds[i].Name)) + "|0.55|3529161471\n";
    }
    p.requestWorldSelectMenu(peer, worldOffers);
}

Worlds getPlyersWorld(Server *server, ENetPeer *peer)
{
    return GET_WORLD(server, pinfo(peer)->currentWorld);
}