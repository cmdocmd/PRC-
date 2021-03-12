#include <iostream>
#include "enet/include/enet/enet.h"
#include "player.cpp"
#include <string.h>
#include "Itemsbuilder.cpp"
#include "GamePacketBuilder.cpp"
#include "utils.cpp"
#include <sstream>
#include "DialogHandler.cpp"
#include "worlds.cpp"

void UPDATE(std::stringstream str, std::string growid);
bool CHECK_LOGIN(std::string growid, std::string pass);
std::istream *PLAYER_DATA(std::string growid);

namespace Events
{
    void Connect(ENetPeer *peer, ENetHost *server)
    {
        if (count_players(server, peer) > 3)
        {
            return; //nothing..
        }
        peer->data = new player; //Creating a New Player
        sendData(server, peer);
    }
    void Recieve(ENetHost *server, ENetPacket *packet, ENetPeer *peer, string cch)
    {
        int MessageType = GetMessageTypeFromPacket(packet);
        std::cout << "MessageType: " << MessageType << std::endl;
        switch (MessageType)
        {
        case 2:
        {
            std::cout << "from case2: " << cch << std::endl;
            if (!pinfo(peer)->InLobby)
            {
                Packets::onsupermain(peer, itemdathash, "ubistatic-a.akamaihd.net", "0098/56640/cache/", player_hash);
                std::stringstream ss(cch);
                std::string to;
                while (std::getline(ss, to, '\n'))
                {
                    std::vector<string> ex = explode("|", to);
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
            if (cch.find("enter_game") != std::string::npos && !pinfo(peer)->InLobby)
            {
                pinfo(peer)->InLobby = true;
                if (!pinfo(peer)->inAccount)
                {
                    Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGet a GrowID``|left|32|\n\nadd_spacer|small|\nadd_text_input|username|GrowID||30|\nadd_text_input|password|Password||100|\nend_dialog|register|Cancel|Get My GrowID!|\n");
                }
                else
                {
                    if (pinfo(peer)->username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
                    {
                        Packets::consoleMessage(peer, "`rPlease remove special characters before login");
                        return;
                    }
                    if (CHECK_LOGIN(pinfo(peer)->username, pinfo(peer)->password)) //correct username and password
                    {
                        std::istream *blobdata = PLAYER_DATA(pinfo(peer)->username);
                        delete (player *)peer->data;
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
                        peer->data = ply;

                        SendWorldOffers(peer);
                    }
                    else
                    {
                        std::cout << "INCorrect" << std::endl;
                    }
                }
            }
            if (cch.find("dialog_name") != std::string::npos && pinfo(peer)->usingDialog)
            {
                Dialog_Handler(cch, peer);
            }
            if (cch.find("action|refresh") != std::string::npos)
            {
                Packets::refresh_data(peer);
            }
            break;
        }
        case 3:
        {
            std::cout << "from case3: " << cch << std::endl;

            std::stringstream ss(cch);
            std::string to;
            bool joinReq = false;
            while (std::getline(ss, to, '\n'))
            {
                std::vector<string> ex = explode("|", to);
                if (ex[0] == "name" && joinReq)
                {
                    if (!pinfo(peer)->InLobby)
                    {
                        break;
                    }
                    joinWorld(server, peer, ex[1], 0, 0);
                }
                if (ex[0] == "action")
                {
                    if (ex[1] == "quit")
                    {
                        enet_peer_disconnect_later(peer, 0);
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
            std::cout << "from case4: " << cch << std::endl;
            break;
        }
        }
    }
    void Disconnect(ENetPeer *peer)
    {
        UPDATE(serialize_player((player *)peer->data), pinfo(peer)->username);
        delete (player *)peer->data; //deleting the struct
        peer->data = NULL;
    }
}