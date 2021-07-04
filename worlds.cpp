#include <iostream>
#include <string.h>
#include <vector>
#include <chrono>

#include "worlds.h"
#include "mysql.h"
#include "packets.h"
#include "server.h"
#include "utils.h"
#include "items.h"

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

AWorld GET_WORLD(Server *server, std::string world)
{
    AWorld ret;
    ret.id = -1;
    boost::to_upper(world);
    if (world.length() >= 1 && world.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") == std::string::npos && world != "EXIT")
    {
        for (int i = 0; i < static_cast<int>(server->m_worlds.size()); i++)
        {
            if (server->m_worlds.at(i).Name == world)
            {
                ret.id = i;
                ret.info = server->m_worlds.at(i);
                ret.ptr = &server->m_worlds.at(i);
            }
        }
        if (WORLD_EXIST(world) && ret.id == -1) //LOAD
        {
            Worlds wld = server->deserialize(world);
            server->m_worlds.push_back(wld);
            ret.id = server->m_worlds.size() - 1;
            ret.info = wld;
            ret.ptr = &server->m_worlds.at(server->m_worlds.size() - 1);
        }

        if (!WORLD_EXIST(world) && ret.id == -1)
        {
            Worlds wld = wld.GenerateWorld(world);
            server->m_worlds.push_back(wld);
            ret.id = server->m_worlds.size() - 1;
            ret.info = wld;
            ret.ptr = &server->m_worlds.at(server->m_worlds.size() - 1);
        }
    }

    return ret;
}

Worlds *getPlyersWorld(Server *server, ENetPeer *peer)
{
    return GET_WORLD(server, pinfo(peer)->currentWorld).ptr;
}

void sendWorld(ENetPeer *peer, Worlds *world)
{
    Packets p;
    pinfo(peer)->joinClothesUpdated = false;

    int xSize = world->Width;
    int ySize = world->Height;
    int square = xSize * ySize;
    int namelen = world->Name.length();

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
    memcpy(&data[68], world->Name.c_str(), namelen);
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
        data.x = i % world->Width;
        data.y = i / world->Height;
        data.punchX = i % world->Width;
        data.punchY = i / world->Width;
        data.XSpeed = 0;
        data.YSpeed = 0;
        data.netID = -1;
        data.plantingTree = world->items[i % xSize][i / xSize].foreground;
        SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);

        if (itemDefs[world->items[i % world->Width][i / world->Width].foreground].blockType == BlockTypes::LOCK && world->items[i % world->Width][i / world->Width].foreground != 4994 && world->items[i % world->Width][i / world->Width].foreground != 202 && world->items[i % world->Width][i / world->Width].foreground != 204 && world->items[i % world->Width][i / world->Width].foreground != 206 && world->items[i % world->Width][i / world->Width].foreground != 5814)
        {
            int pbm = world->pbm;
            if (pbm == 0)
                pbm = 100;
            uint8_t lol = 0x00;
            if (world->dmb == 1)
                lol |= 0x10;
            sendTileData(peer, i % world->Width, i / world->Width, 0x10, world->items[i % world->Width][i / world->Width].foreground, world->items[i % world->Width][i / world->Width].background, lockTileDatas(lol, world->id, 0, 0, false, pbm));
        }
        if (world->items[i % world->Width][i / world->Width].foreground == 226)
        {
            if (world->jammed)
            {
                data.characterState = 0x0; // animation
                data.x = i % world->Width;
                data.y = i / world->Width;
                data.punchX = i % world->Width;
                data.punchY = i / world->Width;
                data.XSpeed = 0;
                data.YSpeed = 0;
                data.packetType = 0x8;
                SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
            }
        }
    }
    for (int i = 0; i < static_cast<int>(world->dblock.size()); i++)
    {
        int x = world->dblock[i].x;
        int y = world->dblock[i].y;
        int id = world->dblock[i].id;
        int fg = world->items[x][y].foreground;
        int bg = world->items[x][y].background;
        updatedblock(peer, fg, x, y, bg, id);
    }
    for (int i = 0; i < static_cast<int>(world->man.size()); i++)
    {
        int x = world->man[i].x;
        int y = world->man[i].y;
        int fg = world->items[x][y].foreground;
        int bg = world->items[x][y].background;
        updateMannequin(peer, fg, x, y, bg, world->man[i].text, world->man[i].clothHair, world->man[i].clothHead, world->man[i].clothMask, world->man[i].clothHand, world->man[i].clothNeck, world->man[i].clothShirt, world->man[i].clothPants, world->man[i].clothFeet, world->man[i].clothBack);
    }
    for (int i = 0; i < static_cast<int>(world->sign.size()); i++)
    {
        int x = world->sign[i].x;
        int y = world->sign[i].y;
        int fg = world->items[x][y].foreground;
        int bg = world->items[x][y].background;
        updatesign(peer, fg, bg, x, y, world->sign[i].text);
    }
    for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
    {
        int x = world->magplant[i].x;
        int y = world->magplant[i].y;
        int id = world->magplant[i].id;
        sendMag(peer, x, y, id, 1, false, false);
    }
    /*for (int i = 0; i < static_cast<int>(world->entrance.size()); i++)
    {
        int x = world->entrance[i].x;
        int y = world->entrance[i].y;
        int fg = world->items[x][y].foreground;
        int bg = world->items[x][y].background;
        if (pinfo(peer)->username != world->owner && pinfo(peer)->adminLevel < 666 && world->entrance[i].is_open == "0")
        {
            updateEntrance(peer, fg, x, y, false, bg);
        }
        else
        {
            updateEntrance(peer, fg, x, y, true, bg);
        }
    }*/
    pinfo(peer)->currentWorld = world->Name;
    if (world->owner != "")
    {
        p.consoleMessage(peer, "`#[`0" + world->Name + " `$World Locked by `o" + world->owner + "`#]");
    }
    p.onsetcurrentweather(peer, world->weather);
}

void joinWorld(Server *server, ENetHost *Host, ENetPeer *peer, std::string act, int x2, int y2)
{
    Packets p;
    int id = GET_WORLD(server, act).id;
    if (act.length() < 18 && id != -1)
    {
        pinfo(peer)->usingDoor = false;
        Worlds wld;
        wld = GET_WORLD(server, act).info;
        bool cursed = false;
        if (wld.isNuked)
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
        for (int i = 0; i < static_cast<int>(wld.bans.size()); i++)
        {
            if (wld.bans[i].name == pinfo(peer)->username)
            {
                int now = time(NULL);
                if (now - wld.bans[i].time < 3600)
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
            wld = GET_WORLD(server, "HELL").info;
        }
        sendWorld(peer, &wld);
        int x = 0;
        int y = 0;

        for (int j = 0; j < wld.Width * wld.Height; j++)
        {
            if (wld.items[j % wld.Width][j / wld.Width].foreground == 6)
            {
                x = (j % wld.Width) * 32;
                y = (j / wld.Width) * 32;
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
    else
    {
        pinfo(peer)->currentWorld == "EXIT";
        p.onfailedtoenterworld(peer);
        p.consoleMessage(peer, "You have entered bad characters in the world name!");
        return;
    }
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

void OnPlace(int x, int y, int tile, Worlds *world, ENetPeer *peer, ENetHost *Host, Server *server)
{
    Packets p;
    bool lock = false;
    if (tile > static_cast<int>(itemDefs.size()))
    {
        return;
    }
    if (tile == 1280)
    {
        p.dialog(peer, "add_label_with_icon|big|`wChange your GrowID|left|1280|\nadd_label|small|`oThis will change your GrowID `4permanently`o.``|left|4|\nadd_label|small|`oYou will pay `2100,000 $ `oif you click on `5Change it`o.``|left|4|\nadd_textbox|`oEnter your new name: |\nadd_text_input|newname|||20|\nend_dialog|cnamedialog|Cancel|Change it!|\n");
        return;
    }
    if (tile == 2580)
    {
        p.dialog(peer, "add_label_with_icon|big|`wSwap World Names|left|2580|\nadd_label|small|`oThis will swap the name of the world you are standing in with other world `4permanetly`o. You must own both worlds, with a World Lock in place. Your `0Change of Address `owill be consumed if you click `5Swap 'Em`o.|left|4|\nadd_textbox|`oEnter the other world's name: |\nadd_text_input|worldname|||20|\nend_dialog|cworlddialog|Cancel|Swap 'Em!|\n");
        return;
    }
    std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
    int cn = 1;
    for (ENetPeer *currentPeer : ishere)
    {
        if (pinfo(currentPeer)->x / 32 == x && pinfo(currentPeer)->y / 32 == y)
        {
            if (tile == 278)
            {
                if (cn == 1)
                {
                    server->sendGlobalMessage("`#** `$The Ancients `ohave used `bCurse `oon `w" + pinfo(currentPeer)->username + "`o! `#**", "beep");
                    pinfo(currentPeer)->curse = time(NULL);
                    int hours = 1 * 60 * 60;
                    pinfo(currentPeer)->cursetime = hours;
                    p.onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `bCURSED `0from Private Server for 1 hours.", "audio/hub_open.wav");
                    sendPlayerLeave(Host, currentPeer);
                    joinWorld(server, Host, currentPeer, "HELL", 0, 0);
                    cn++;
                    Rem(peer, 278, 1, true);
                }
            }
            return;
        }
        if (itemDefs[tile].blockType == BlockTypes::CONSUMMABLE)
        {
            return;
        }
    }
    if (world->items[x][y].foreground == 2946 && tile != 2946)
    {
        int idx = -1;
        for (int i = 0; i < static_cast<int>(world->dblock.size()); i++)
        {
            if (world->dblock[i].x == x && world->dblock[i].y == y)
            {
                idx = i;
            }
        }
        if (idx == -1)
        {
            pinfo(peer)->wrenchx = x;
            pinfo(peer)->wrenchy = y;
            std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
            for (ENetPeer *currentPeer : ishere)
            {
                updatedblock(currentPeer, world->items[x][y].foreground, x, y, world->items[x][y].background, tile);
            }
            Rem(peer, tile, 1, true);
            pushdblock(server, peer, tile);
        }
        else
        {
            p.ontalkbubble(peer, pinfo(peer)->netID, "`wPickup the item first", 1);
        }
        return;
    }
    if (world->items[x][y].foreground == 1420 || world->items[x][y].foreground == 6214)
    {
        int idx = -1;
        for (int i = 0; i < static_cast<int>(world->man.size()); i++)
        {
            if (world->man[i].x == x && world->man[i].y == y)
            {
                idx = i;
            }
        }
        if (idx != -1)
        {
            switch (itemDefs[tile].clothingType)
            {
            case ClothTypes::BACK:
                world->man[idx].clothBack = tile;
                break;
            case ClothTypes::FACE:
                world->man[idx].clothHead = tile;
                break;
            case ClothTypes::FEET:
                world->man[idx].clothFeet = tile;
                break;
            case ClothTypes::MASK:
                world->man[idx].clothMask = tile;
                break;
            case ClothTypes::HAND:
                world->man[idx].clothHand = tile;
                break;
            case ClothTypes::HAIR:
                world->man[idx].clothHair = tile;
                break;
            case ClothTypes::NECKLACE:
                world->man[idx].clothNeck = tile;
                break;
            case ClothTypes::PANTS:
                world->man[idx].clothPants = tile;
                break;
            case ClothTypes::SHIRT:
                world->man[idx].clothShirt = tile;
                break;
            default:
                break;
            }

            std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
            for (ENetPeer *currentPeer : ishere)
            {
                updateMannequin(currentPeer, world->items[x][y].foreground, x, y, world->items[x][y].background, world->man[idx].text, world->man[idx].clothHair, world->man[idx].clothHead, world->man[idx].clothMask, world->man[idx].clothHand, world->man[idx].clothNeck, world->man[idx].clothShirt, world->man[idx].clothPants, world->man[idx].clothFeet, world->man[idx].clothBack);
            }
            Rem(peer, tile, 1, true);
        }
        return;
    }
    if (tile == 1420 || tile == 6214)
    {
        pinfo(peer)->wrenchx = x;
        pinfo(peer)->wrenchy = y;
        pushman(server, peer);
    }
    if (itemDefs[tile].blockType == BlockTypes::GATEWAY)
    {
        pinfo(peer)->wrenchx = x;
        pinfo(peer)->wrenchy = y;
        pushentrance(server, peer);
        std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
        for (ENetPeer *currentPeer : ishere)
        {
            for (int i = 0; i < static_cast<int>(world->entrance.size()); i++)
            {
                int x = world->entrance[i].x;
                int y = world->entrance[i].y;
                if (pinfo(currentPeer)->username != world->owner && pinfo(currentPeer)->adminLevel < 666 && world->entrance[i].is_open == "0")
                {
                    updateEntrance(currentPeer, world->items[x][y].foreground, x, y, false, world->items[x][y].background);
                }
                else
                {
                    updateEntrance(currentPeer, world->items[x][y].foreground, x, y, true, world->items[x][y].background);
                }
            }
        }
    }
    if (itemDefs[tile].blockType == BlockTypes::LOCK)
    {
        if (tile == 202 || tile == 204 || tile == 206)
        {
            Nothing(peer, x, y);
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
                p.ontalkbubble(peer, pinfo(peer)->netID, "`5[`w" + world->Name + " has been `$World Locked `wby " + pinfo(peer)->name, 1);
                pinfo(peer)->worlds.push_back(world->Name);
                pinfo(peer)->name = "`2" + pinfo(peer)->name;
                GamePacket p3 = GamePacketBuilder()
                                    .appendString("OnNameChanged")
                                    .appendString("```0" + pinfo(peer)->name)
                                    .build();
                memcpy(p3.data + 8, &(pinfo(peer)->netID), 4);
                std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                for (ENetPeer *currentPeer : ishere)
                {
                    p3.send(currentPeer);
                }
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
        world->items[x][y].foreground = tile == 5640 ? pinfo(peer)->mgid : tile;
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
    data.plantingTree = tile == 5640 ? pinfo(peer)->mgid : tile;

    for (ENetPeer *currentPeer : ishere)
    {
        SendPacketRaw(4, packPlayerMoving(&data), 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
        if (lock)
        {
            sendmusic(currentPeer, "use_lock");
            int pbm = world->pbm;
            if (pbm == 0)
                pbm = 100;
            uint8_t lol = 0x00;
            if (world->dmb == 1)
                lol |= 0x10;
            sendTileData(currentPeer, x, y, 0x10, tile, world->items[x][y].background, lockTileDatas(lol, pinfo(peer)->userID, 0, 0, false, pbm));
        }
    }
    if (tile != 5640)
    {
        Rem(peer, tile, 1, false);
    }
}

void OnPunch(int x, int y, Worlds *world, ENetPeer *peer, ENetHost *Host, Server *server)
{
    Packets p;
    int tile = (world->items[x][y].foreground == 0) ? world->items[x][y].background : world->items[x][y].foreground;
    if (tile == 0 || tile == 6864 || tile == 6 || tile == 8)
    {
        Nothing(peer, x, y);
        return;
    }
    bool weather = false;
    int jid = 0;
    std::cout << "Punch place: " << x + y * world->Width << std::endl;
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
    if (y < world->Height)
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
    else if (tile == 226 || tile == 1276)
    {
        breakhit = 30;
        times = 5;
    }
    else if (tile == 5638)
    {
        breakhit = 35;
    }
    else
    {
        breakhit = itemDefs[tile].breakHits;
    }
    if (y < world->Height && world->items[x][y].breakLevel >= breakhit * times)
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
                std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                for (ENetPeer *currentPeer : ishere)
                {
                    p.consoleMessage(currentPeer, "`$World Lock `ohas been removed");
                }
                int idx = -1;
                for (int i = 0; i < static_cast<int>(pinfo(peer)->worlds.size()); i++)
                {
                    if (pinfo(peer)->worlds[i] == world->Name)
                    {
                        idx = i;
                    }
                }
                if (idx != -1)
                {
                    pinfo(peer)->worlds.erase(pinfo(peer)->worlds.begin() + idx);
                }
            }
            if (block == 5638)
            {
                for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
                {
                    if (world->magplant[i].x == x && world->magplant[i].y == y)
                    {
                        if (world->magplant[i].amount != 0)
                        {
                        }
                    }
                }
            }
            if (itemDefs[block].blockType == BlockTypes::GATEWAY)
            {
                int idx = -1;
                for (int i = 0; i < static_cast<int>(world->entrance.size()); i++)
                {
                    if (world->entrance[i].x == x && world->entrance[i].y == y)
                        idx = i;
                }
                if (idx != -1)
                {
                    world->entrance.erase(world->entrance.begin() + idx);
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
                }
            }
            if (itemDefs[block].blockType == BlockTypes::WEATHER_MACHINE)
            {
                weather = true;
            }
            if (block == 226 || block == 1276)
            {
                jid = block;
            }
            if (block == 1420 || block == 6214)
            {
                int idx = -1;
                for (int i = 0; i < static_cast<int>(world->man.size()); i++)
                {
                    if (world->man[i].x == x && world->man[i].y == y)
                        idx = i;
                }
                if (idx != -1)
                {
                    if (!MoreThan200(peer, world->man[idx].clothBack, 1) && !MoreThan200(peer, world->man[idx].clothFeet, 1) && !MoreThan200(peer, world->man[idx].clothHair, 1) && !MoreThan200(peer, world->man[idx].clothHand, 1) && !MoreThan200(peer, world->man[idx].clothHead, 1) && !MoreThan200(peer, world->man[idx].clothMask, 1) && !MoreThan200(peer, world->man[idx].clothNeck, 1) && !MoreThan200(peer, world->man[idx].clothPants, 1) && !MoreThan200(peer, world->man[idx].clothShirt, 1))
                    {
                        if (world->man[idx].clothBack != 0)
                        {
                            if (LEGAL_ITEM(peer, world->man[idx].clothBack))
                            {
                                Add(peer, world->man[idx].clothBack, 1, true);
                            }
                            else
                            {
                                Add_ITEM(peer, world->man[idx].clothBack, 1, true);
                            }
                        }
                        if (world->man[idx].clothFeet != 0)
                        {
                            if (LEGAL_ITEM(peer, world->man[idx].clothFeet))
                            {
                                Add(peer, world->man[idx].clothFeet, 1, true);
                            }
                            else
                            {
                                Add_ITEM(peer, world->man[idx].clothFeet, 1, true);
                            }
                        }
                        if (world->man[idx].clothHair != 0)
                        {
                            if (LEGAL_ITEM(peer, world->man[idx].clothHair))
                            {
                                Add(peer, world->man[idx].clothHair, 1, true);
                            }
                            else
                            {
                                Add_ITEM(peer, world->man[idx].clothHair, 1, true);
                            }
                        }
                        if (world->man[idx].clothHand != 0)
                        {
                            if (LEGAL_ITEM(peer, world->man[idx].clothHand))
                            {
                                Add(peer, world->man[idx].clothHand, 1, true);
                            }
                            else
                            {
                                Add_ITEM(peer, world->man[idx].clothHand, 1, true);
                            }
                        }
                        if (world->man[idx].clothHead != 0)
                        {
                            if (LEGAL_ITEM(peer, world->man[idx].clothHead))
                            {
                                Add(peer, world->man[idx].clothHead, 1, true);
                            }
                            else
                            {
                                Add_ITEM(peer, world->man[idx].clothHead, 1, true);
                            }
                        }
                        if (world->man[idx].clothMask != 0)
                        {
                            if (LEGAL_ITEM(peer, world->man[idx].clothMask))
                            {
                                Add(peer, world->man[idx].clothMask, 1, true);
                            }
                            else
                            {
                                Add_ITEM(peer, world->man[idx].clothMask, 1, true);
                            }
                        }
                        if (world->man[idx].clothNeck != 0)
                        {
                            if (LEGAL_ITEM(peer, world->man[idx].clothNeck))
                            {
                                Add(peer, world->man[idx].clothNeck, 1, true);
                            }
                            else
                            {
                                Add_ITEM(peer, world->man[idx].clothNeck, 1, true);
                            }
                        }
                        if (world->man[idx].clothPants != 0)
                        {
                            if (LEGAL_ITEM(peer, world->man[idx].clothPants))
                            {
                                Add(peer, world->man[idx].clothPants, 1, true);
                            }
                            else
                            {
                                Add_ITEM(peer, world->man[idx].clothPants, 1, true);
                            }
                        }
                        if (world->man[idx].clothShirt != 0)
                        {
                            if (LEGAL_ITEM(peer, world->man[idx].clothShirt))
                            {
                                Add(peer, world->man[idx].clothShirt, 1, true);
                            }
                            else
                            {
                                Add_ITEM(peer, world->man[idx].clothShirt, 1, true);
                            }
                        }
                        world->man.erase(world->man.begin() + idx);
                    }
                    else
                    {
                        p.ontalkbubble(peer, pinfo(peer)->netID, "`wYou will have more than 200!", 1);
                        return;
                    }
                }
            }
            if (block == 2946)
            {
                int idx = -1;
                for (int i = 0; i < static_cast<int>(world->dblock.size()); i++)
                {
                    if (world->dblock[i].x == x && world->dblock[i].y == y)
                        idx = i;
                }
                if (idx != -1)
                {
                    if (!MoreThan200(peer, world->dblock[idx].id, 1))
                    {
                        Add(peer, world->dblock[idx].id, 1, true);
                        world->dblock.erase(world->dblock.begin() + idx);
                    }
                    else
                    {
                        p.ontalkbubble(peer, pinfo(peer)->netID, "`wYou will have more than 200!", 1);
                        return;
                    }
                }
            }
            if (itemDefs[block].blockType != BlockTypes::LOCK && itemDefs[block].blockType != BlockTypes::WEATHER_MACHINE && itemDefs[block].blockType != BlockTypes::SEED && block != 1008 && block != 1240 && block != 8878) //gem system
            {
                int c = 0;
                bool drop = true;
                for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
                {
                    if (world->magplant[i].id == block && world->magplant[i].active == 1 && c == 0)
                    {
                        drop = false;
                        c++;
                        std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                        for (ENetPeer *currentPeer : ishere)
                        {
                            int x = world->magplant[i].x;
                            int y = world->magplant[i].y;
                            //add flying code
                        }
                        gem_system(server, Host, peer, block, 0, 0, false, i);
                    }
                }
                if (drop)
                {
                    gem_system(server, Host, peer, block, x, y, true, 0);
                }
            }
            if (pinfo(peer)->level < 125)
            {
                if (itemDefs[block].rarity >= 2)
                {
                    pinfo(peer)->xp += itemDefs[block].rarity / 2;
                    if (pinfo(peer)->xp >= pinfo(peer)->to_break)
                    {
                        pinfo(peer)->to_break += 600;
                        pinfo(peer)->xp = 0;
                        pinfo(peer)->level++;
                        std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                        for (ENetPeer *currentPeer : ishere)
                        {
                            p.consoleMessage(currentPeer, pinfo(peer)->name + " `wis now level " + std::to_string(pinfo(peer)->level) + "!");
                            sendmusic(currentPeer, "levelup2");
                            p.onparticleeffect(currentPeer, 46, pinfo(peer)->x, pinfo(peer)->y);
                        }
                    }
                }
            }
            world->items[x][y].foreground = 0;
        }
        else
        {
            int c = 0;
            bool drop = true;
            for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
            {
                if (world->magplant[i].id == world->items[x][y].background && world->magplant[i].active == 1 && c == 0)
                {
                    drop = false;
                    c++;
                    std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                    for (ENetPeer *currentPeer : ishere)
                    {
                        int x = world->magplant[i].x;
                        int y = world->magplant[i].y;
                        //add flying code
                    }
                    gem_system(server, Host, peer, world->items[x][y].background, 0, 0, false, i);
                }
            }
            if (drop)
            {
                gem_system(server, Host, peer, world->items[x][y].background, x, y, true, 0);
            }
            world->items[x][y].background = 6864;
        }
    }
    std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
    for (ENetPeer *currentPeer : ishere)
    {
        SendPacketRaw(4, packPlayerMoving(&data), 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
        if (tile == 226)
        {
            if (world->jammed)
            {
                world->jammed = false;
            }
            else
            {
                world->jammed = true;
            }
        }
        if (itemDefs[tile].blockType == BlockTypes::WEATHER_MACHINE)
        {
            if (tile == 934)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 2 ? 0 : 2);
                world->weather = world->weather == 2 ? 0 : 2;
            }
            if (tile == 946)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 3 ? 0 : 3);
                world->weather = world->weather == 3 ? 0 : 3;
            }
            if (tile == 984)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 5 ? 0 : 5);
                world->weather = world->weather == 5 ? 0 : 5;
            }
            if (tile == 1210)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 8 ? 0 : 8);
                world->weather = world->weather == 8 ? 0 : 8;
            }
            if (tile == 1490)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 10 ? 0 : 10);
                world->weather = world->weather == 10 ? 0 : 10;
            }
            if (tile == 1364)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 11 ? 0 : 11);
                world->weather = world->weather == 11 ? 0 : 11;
            }
            if (tile == 1750)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 15 ? 0 : 15);
                world->weather = world->weather == 15 ? 0 : 15;
            }
            if (tile == 2046)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 17 ? 0 : 17);
                world->weather = world->weather == 17 ? 0 : 17;
            }
            if (tile == 2284)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 18 ? 0 : 18);
                world->weather = world->weather == 18 ? 0 : 18;
            }
            if (tile == 2744)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 19 ? 0 : 19);
                world->weather = world->weather == 19 ? 0 : 19;
            }
            if (tile == 3252)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 20 ? 0 : 20);
                world->weather = world->weather == 20 ? 0 : 20;
            }
            if (tile == 3446)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 21 ? 0 : 21);
                world->weather = world->weather == 21 ? 0 : 21;
            }
            if (tile == 3534)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 22 ? 0 : 22);
                world->weather = world->weather == 22 ? 0 : 22;
            }
            if (tile == 4776)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 32 ? 0 : 32);
                world->weather = world->weather == 32 ? 0 : 32;
            }
            if (tile == 4892)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 33 ? 0 : 33);
                world->weather = world->weather == 33 ? 0 : 33;
            }
            if (tile == 5112)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 35 ? 0 : 35);
                world->weather = world->weather == 35 ? 0 : 35;
            }
            if (tile == 5654)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 36 ? 0 : 36);
                world->weather = world->weather == 36 ? 0 : 36;
            }
            if (tile == 5716)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 37 ? 0 : 37);
                world->weather = world->weather == 37 ? 0 : 37;
            }
            if (tile == 6854)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 42 ? 0 : 42);
                world->weather = world->weather == 42 ? 0 : 42;
            }
            if (tile == 7644)
            {
                p.onsetcurrentweather(currentPeer, world->weather == 44 ? 0 : 44);
                world->weather = world->weather == 44 ? 0 : 44;
            }
        }
        if (weather)
        {
            p.onsetcurrentweather(currentPeer, 0);
            world->weather = 0;
        }
        if (jid != 0)
        {
            if (jid == 226)
            {
                world->jammed = false;
            }
            if (jid == 1276)
            {
            }
        }
    }
}

void onWrench(Worlds *world, int x, int y, ENetPeer *peer)
{
    Packets p;
    int block = world->items[x][y].foreground;
    if (block == 6016)
    {
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wWorld Stats|left|6016|\nadd_label|small|`wThis amazing block can show the stats for the whole world!|left|4|\n\nadd_spacer|small|\nadd_label|small|`wWhich stats would you like to view?|left|4|\nadd_button|floatitems|`oFloating Items``|0|0|\nend_dialog|noty|Cancel||gazette||");
    }
    if (block == 8878)
    { //safe valut
        int count = 0;
        int idx = -1;
        pinfo(peer)->wrenchx = x;
        pinfo(peer)->wrenchy = y;
        for (int i = 0; i < static_cast<int>(world->safe.size()); i++)
        {
            if (world->safe[i].x == x && world->safe[i].y == y)
            {
                if (world->safe[i].pass != "")
                    idx = i; //to check if it have a password or no
            }
        }
        if (idx != -1)
        {
            p.dialog(peer, "add_label_with_icon|big|`wSafe Vault|left|8878|\nadd_label|small|`oPlease enter your password to access the Safe Vault.|left|8874|\nadd_text_input|safepss|||12|\nend_dialog|safe|`wExit|`wEnter Password|\nadd_quick_exit|\n");
        }
        else
        {
            p.dialog(peer, "add_label_with_icon|big|`wSafe Vault|left|8878|\nadd_label|small|`oThis Safe Vault is not `4password protected`o!|left|8874|\nadd_spacer|small|\nadd_label|small|`oSet a password.|left|8874|\nadd_text_input|addpss|||12|\nend_dialog|spss|`wExit|`wEnter Password|\nadd_quick_exit|\n");
        }
    }
    if (block == 2946)
    {
        pinfo(peer)->wrenchx = x;
        pinfo(peer)->wrenchy = y;
        int idx = -1;
        for (int i = 0; i < world->dblock.size(); i++)
        {
            if (world->dblock[i].x == x && world->dblock[i].y == y)
                idx = i;
        }
        if (idx != -1)
        {
            p.dialog(peer, "add_label_with_icon|big|`wEdit " + itemDefs[world->items[x][y].foreground].name + "``|left|" + std::to_string(world->items[x][y].foreground) + "|\nadd_label|small|`oDo you want to pickup a " + itemDefs[world->dblock[idx].id].name + ".|left|8874|\nend_dialog|displayblock|Cancel|`wPick up|\nadd_quick_exit|\n");
        }
    }
    if (block == 5638)
    {
        pinfo(peer)->wrenchx = x;
        pinfo(peer)->wrenchy = y;
        int idx = -1;
        for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
        {
            if (world->magplant[i].x == x && world->magplant[i].y == y)
                idx = i;
        }
        if (idx != -1)
        {
            std::string de = "DISABLED";
            std::string aa = "Punch to activate building mode.";
            std::string ll = "";
            if (!LEGAL_ITEM(peer, 5640))
                ll = "\nadd_button|getremote|`oGet Remote|0|0|";
            if (world->magplant[idx].state == 1)
            {
                de = "ACTIVE";
                aa = "Punch to disable building mode.";
            }
            if (world->magplant[idx].amount != 0)
            {
                p.dialog(peer, "add_label_with_icon|big|`w" + itemDefs[world->items[x][y].foreground].name + "``|left|" + std::to_string(world->items[x][y].foreground) + "|\nadd_label_with_icon|small|`2" + itemDefs[world->magplant[idx].id].name + "|left|" + std::to_string(world->magplant[idx].id) + "|\nadd_label|small|`oThe machine contains " + std::to_string(world->magplant[idx].amount) + " `2" + itemDefs[world->magplant[idx].id].name + "|left|8874|\nadd_button|addmac|`oAdd items to the machine|0|0|\nadd_button|reitems|`oRetrieve items|0|0|\nadd_label|small|`oBuilding mode: `9" + de + "|left|8874|\nadd_label|small|`oUse the MAGPLANT 5000 Remote to build `2" + itemDefs[world->magplant[idx].id].name + "`o directly from the MAGPLANT 5000's storage.|left|8874|\nadd_checkbox|ismagactive|`oEnable Collection.|" + std::to_string(world->magplant[idx].active) + "|" + ll + "\nend_dialog|updatemag|Close|`wUpdate|");
            }
            else
            {
                p.dialog(peer, "add_label_with_icon|big|`w" + itemDefs[world->items[x][y].foreground].name + "``|left|" + std::to_string(world->items[x][y].foreground) + "|\nadd_label_with_icon|small|`2" + itemDefs[world->magplant[idx].id].name + "|left|" + std::to_string(world->magplant[idx].id) + "|\nadd_label|small|`9The machine is currently empty!|left|8874|\nadd_button|addmac|`oAdd items to the machine|0|0|\nadd_item_picker|chnitem|`oChange Item|Choose an item to put in the MAGPLANT 5000!|\nadd_label|small|`oBuilding mode: `9" + de + "|left|8874|\nadd_label|small|`o" + aa + "|left|8874|\nadd_checkbox|ismagactive|`oEnable Collection.|" + std::to_string(world->magplant[idx].active) + "|\nend_dialog|updatemag|Close|`wUpdate|");
            }
        }
        else
        {
            p.dialog(peer, "add_label_with_icon|big|`w" + itemDefs.at(world->items[x][y].foreground).name + "``|left|" + std::to_string(world->items[x][y].foreground) + "|\nadd_label|small|`9The machine is empty.|left|8874|\nadd_item_picker|mgitem|`wChoose Item|Choose an item to put in the MAGPLANT 5000!|\nend_dialog|mgplant|`wCancel||\nadd_quick_exit|\n");
        }
    }
    //|| block == 1420 || block == 6214
    if (itemDefs[block].blockType == BlockTypes::SIGN)
    {
        pinfo(peer)->wrenchx = x;
        pinfo(peer)->wrenchy = y;
        std::string text = "";
        for (int i = 0; i < static_cast<int>(world->sign.size()); i++)
        {
            if (world->sign[i].x == x && world->sign[i].y == y)
            {
                text = world->sign[i].text;
            }
        }
        std::string what = itemDefs[block].blockType == BlockTypes::SIGN ? "sign" : "mannequine";
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wEdit " + itemDefs.at(world->items[x][y].foreground).name + "``|left|" + std::to_string(world->items[x][y].foreground) + "|\n\nadd_textbox|`oWhat would you like to write on this " + what + "?|\nadd_text_input|sign||" + text + "|100|\nend_dialog|signok|Cancel|OK|");
    }
    if (itemDefs[block].blockType == BlockTypes::DOOR)
    {
        std::string dest = "", label = "", id = "", iop = "0", text = "ID (optional)";
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
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wEdit " + itemDefs[block].name + "``|left|" + std::to_string(block) + "||\n\nadd_text_input|dest|`oTarget World|" + dest + "|100|\nadd_text_input|label|Display Label (optional)|" + label + "|100|\nadd_text_input|doorid|" + text + "|" + id + "|35|\nadd_checkbox|isopenpublic|is open to public|" + iop + "\nend_dialog|editdoor|Cancel|OK|");
    }
    if (itemDefs[block].blockType == BlockTypes ::LOCK && block != 4994 && block != 202 && block != 204 && block != 206 && block != 5814)
    {
        pinfo(peer)->wrenchx = x;
        pinfo(peer)->wrenchy = y;
        std::string accesslist = "";
        std::string offname = "";
        for (std::vector<std::string>::const_iterator i = world->access.begin(); i != world->access.end(); ++i)
        {
            offname = *i;
            accesslist += "\nadd_checkbox|isAccessed" + offname + "|" + offname + "|1|\n";
        }
        int ispublic = 0;
        int pbm = world->pbm;
        if (pbm == 0)
            pbm = 100;
        int dmb = world->dmb;
        if (world->isPublic)
            ispublic = 1;
        if (world->access.size() == 0)
        {
            p.dialog(peer, "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + itemDefs[block].name + "``|left|" + std::to_string(itemDefs[block].id) + "|\nadd_label|small|`wAccess list:``|left\nadd_spacer|small|\nadd_label|small|Currently, you're the only one with access.``|left\nadd_spacer|small|\nadd_player_picker|netid|`wAdd``|\nadd_checkbox|isworldpublic|Allow anyone to Build and Break|" + std::to_string(ispublic) + "\nadd_checkbox|checkbox_disable_music|Disable Custom Music Blocks|" + std::to_string(dmb) + "\nadd_text_input|tempo|Music BPM|" + std::to_string(pbm) + "|3|\nend_dialog|editlock|Cancel|OK|");
        }
        else
        {
            p.dialog(peer, "set_default_color|`o\nadd_label_with_icon|big|`wEdit " + itemDefs[block].name + "``|left|" + std::to_string(itemDefs[block].id) + "|\nadd_label|small|`wAccess list:``|left\nadd_spacer|small|\nadd_label|small|" + accesslist + "add_spacer|small|\nadd_player_picker|netid|`wAdd``|\nadd_checkbox|isworldpublic|Allow anyone to Build and Break|" + std::to_string(ispublic) + "\nadd_checkbox|checkbox_disable_music|Disable Custom Music Blocks|" + std::to_string(dmb) + "\nadd_text_input|tempo|Music BPM|" + std::to_string(pbm) + "|3|\nend_dialog|editlock|Cancel|OK|");
        }
    }
    if (itemDefs[block].blockType == BlockTypes::GATEWAY)
    {
        pinfo(peer)->wrenchx = x;
        pinfo(peer)->wrenchy = y;
        std::string iop = "0";
        for (int i = 0; i < world->entrance.size(); i++)
        {
            if (world->entrance[i].x == x && world->entrance[i].y == y)
                iop = world->entrance[i].is_open;
        }
        p.dialog(peer, "add_label_with_icon|big|`wEdit " + itemDefs[world->items[x][y].foreground].name + "``|left|" + std::to_string(world->items[x][y].foreground) + "|\nadd_checkbox|ishpublic|`oPublic?|" + iop + "|\nend_dialog|editentranceo|Cancel|`wOK|\nadd_quick_exit|\n");
    }
}