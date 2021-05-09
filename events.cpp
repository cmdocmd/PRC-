#include <iostream>
#include <sstream>
#include "enet/enet.h"
#include "utils.h"
#include "players.h"
#include "packets.h"
#include "server.h"
#include "mysql.h"
#include "items.h"
#include "dialog.h"
#include "pathfinder.h"

void Connect(ENetPeer *peer, ENetHost *Host)
{
    if (count_players(Host, peer) > 3)
    {
        return; //nothing..
    }
    peer->data = new Players; //Creating a New Player
    sendData(Host, peer);
}

void Recieve(Server *server, ENetHost *Host, ENetPacket *packet, ENetPeer *peer, std::string cch)
{
    Packets p;
    int MessageType = GetMessageTypeFromPacket(packet);
    std::cout << "MessageType: " << MessageType << "\n";
    switch (MessageType)
    {
    case 2:
    {
        std::cout << "from case2: \n"
                  << cch << "\n";
        if (!pinfo(peer)->InLobby)
        {
            pinfo(peer)->InLobby = true;
            p.onsupermain(peer, itemdathash, "ubistatic-a.akamaihd.net", "0098/12040/cache/", player_hash);
            std::stringstream ss(cch);
            std::string to;
            while (std::getline(ss, to, '\n'))
            {
                std::vector<std::string> ex = explode("|", to);
                if (ex[0] == "tankIDName")
                {
                    if (ex[1].length() > 35)
                        continue;
                    pinfo(peer)->username = ex[1];
                    pinfo(peer)->inAccount = true;
                }
                if (ex[0] == "tankIDPass")
                {
                    if (ex[1].length() > 35)
                        continue;
                    pinfo(peer)->password = ex[1];
                }
                if (ex[0] == "requestedName")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "f")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "lmode")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "cbits")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "player_age")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "GDPR")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "hash2")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "meta")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "fhash")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "rid")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "platformID")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "deviceVersion")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "country")
                {
                    if (ex[1].length() > 35)
                        continue;
                    pinfo(peer)->country = ex[1];
                }
                if (ex[0] == "hash")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
                if (ex[0] == "mac")
                {
                    if (ex[1].length() > 37)
                        continue;
                    pinfo(peer)->mac = ex[1];
                }
                if (ex[0] == "wk")
                {
                    if (ex[1].length() > 37)
                        continue;
                }
                if (ex[0] == "zf")
                {
                    if (ex[1].length() > 35)
                        continue;
                }
            }
        }
        if (cch.find("enter_game") != std::string::npos)
        {
            if (!pinfo(peer)->inAccount)
            {
                p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGet a GrowID``|left|32|\n\nadd_spacer|small|\nadd_text_input|username|GrowID||30|\nadd_text_input|password|Password||100|\nend_dialog|register|Cancel|Get My GrowID!|\n");
            }
            else
            {
                if (pinfo(peer)->username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
                {
                    p.consoleMessage(peer, "`rPlease remove special characters before login");
                }
                else if (CHECK_LOGIN(pinfo(peer)->username, pinfo(peer)->password))
                {
                    Players *ply = Desrialize(PLAYER_DATA(pinfo(peer)->username));
                    delete (Players *)peer->data;
                    peer->data = ply;
                    pinfo(peer)->isCorrect = true;
                    pinfo(peer)->InLobby = true;
                    pinfo(peer)->userID = PLAYER_ID(pinfo(peer)->username);
                    if (pinfo(peer)->name == "")
                    {
                        pinfo(peer)->name = pinfo(peer)->username;
                    }
                    if (pinfo(peer)->ban != 0)
                    {
                        int to = pinfo(peer)->bantime;
                        time_t now = time(NULL);
                        time_t banned = pinfo(peer)->ban;
                        if (now - banned > to)
                        {
                            pinfo(peer)->ban = 0;
                            pinfo(peer)->bantime = 0;
                            p.consoleMessage(peer, "`$Your ban have been removed please behave better this time");
                            SendWorldOffers(server, Host, peer);
                        }
                        else
                        {
                            int left = now - banned;
                            int start = to - left;
                            //  time_left(peer, start, "banned");
                        }
                    }
                    else
                    {
                        SendWorldOffers(server, Host, peer);
                    }
                }
                else
                {
                    p.consoleMessage(peer, "`rWrong username or password");
                }
            }
        }
        if (cch.find("dialog_name") != std::string::npos && pinfo(peer)->usingDialog)
        {
            dialog(server, cch, peer, Host);
        }
        if (cch.find("action|input\n|text|") != std::string::npos)
        {
            //Commands_Handler(cch, peer, server);
        }
        if (cch.find("refresh_item_data") != std::string::npos)
        {
            p.refresh_data(peer);
        }
        break;
    }
    case 3:
    {
        std::cout << "from case3: \n"
                  << cch << "\n";
        std::stringstream ss(cch);
        std::string to;
        bool joinReq = false;
        while (std::getline(ss, to, '\n'))
        {
            std::vector<std::string> ex = explode("|", to);
            if (ex[0] == "name" && joinReq)
            {
                if (!pinfo(peer)->InLobby)
                {
                    break;
                }
                if (!pinfo(peer)->isCorrect)
                {
                    break;
                }
                joinWorld(server, Host, peer, ex[1], 0, 0);
            }
            if (ex[0] == "action")
            {
                if (ex[1] == "quit")
                {
                    enet_peer_disconnect_later(peer, 0);
                }
                if (ex[1] == "quit_to_exit")
                {
                    /*sendPlayerLeave(server, peer, pinfo(peer));
                    pinfo(peer)->currentWorld = "EXIT";*/
                    SendWorldOffers(server, Host, peer);
                }
                if (ex[1] == "join_request")
                {
                    joinReq = true;
                }
            }
        }
        break;
    }
    case 4:
    {
        std::cout << "from case4: " << cch << "\n";
        BYTE *tankUpdatePacket = GetStructPointerFromTankPacket(packet);
        Worlds world = getPlyersWorld(server, peer);
        if (tankUpdatePacket)
        {
            PlayerMoving *pMov = unpackPlayerMoving(tankUpdatePacket);
            if (pMov->packetType == 0)
            {
                int x = pinfo(peer)->x / 32;
                int y = pinfo(peer)->y / 32;
                std::cout << "x: " << x << " y: " << y << " pMovx: " << (int)pMov->x / 32 << " pMovy " << (int)pMov->y / 32 << "\n";
                Pair dest = std::make_pair((int)pMov->x / 32, (int)pMov->y / 32); // place of area we need to check if there is a destination
                Pair src = std::make_pair(x, y);
                if (aStarSearch(world, src, dest) || pinfo(peer)->adminLevel >= 666)
                {
                    pinfo(peer)->usingDoor = false;
                    pinfo(peer)->x = pMov->x;
                    pinfo(peer)->y = pMov->y;
                    pinfo(peer)->isRotatedLeft = pMov->characterState & 0x10;
                    if ((pinfo(peer)->isRotatedLeft = pMov->characterState & 0x10))
                    {
                        pinfo(peer)->isRotatedLeft = true;
                    }
                    else
                    {
                        pinfo(peer)->isRotatedLeft = false;
                    }
                    sendPData(Host, peer, pMov);
                    if (!pinfo(peer)->joinClothesUpdated)
                    {
                        pinfo(peer)->joinClothesUpdated = true;
                        updateAllClothes(Host, peer);
                        sendState(Host, peer);
                    }
                }
                else
                {
                }
            }
            delete pMov;
        }
        break;
    }
    default:
        break;
    }
}

void Disconnect(ENetHost *Host, ENetPeer *peer)
{
    delete (Players *)peer->data; //deleting the struct
    peer->data = NULL;
}