#include <iostream>
#include "enet/include/enet/enet.h"
#include "player.cpp"
#include <string.h>
#include "Itemsbuilder.cpp"
#include "GamePacketBuilder.cpp"
#include "utils.cpp"
#include <sstream>
#include "DialogHandler.cpp"
#include "commands.cpp"
#include "worlds.cpp"

void UPDATE(std::stringstream str, std::string growid);
bool CHECK_LOGIN(std::string growid, std::string pass);
int PLAYER_ID(std::string growid);
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
                Packets::onsupermain(peer, itemdathash, "ubistatic-a.akamaihd.net", "0098/12040/cache/", player_hash);
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
                        pinfo(peer)->userID = PLAYER_ID(pinfo(peer)->username);
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
                Dialog_Handler(cch, peer, server);
            }
            if (cch.find("action|input\n|text|") != std::string::npos)
            {
                Commands_Handler(cch, peer, server);
            }
            if (cch.find("action|friends") != std::string::npos)
            {
                Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wSocial Portal``|left|1366|\n\nadd_spacer|small|\nadd_button|friends|`wShow Friends``|left|||\nadd_button|create_guild|`wCreate Guild``|left|||\nend_dialog||OK||\n");
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
            BYTE *tankUpdatePacket = GetStructPointerFromTankPacket(packet);
            Worlds *world = getPlyersWorld(peer);
            if (tankUpdatePacket)
            {
                PlayerMoving *pMov = unpackPlayerMoving(tankUpdatePacket);
                switch (pMov->packetType)
                {
                case 0:
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
                    sendPData(server, peer, pMov);
                    if (!pinfo(peer)->joinClothesUpdated)
                    {
                        pinfo(peer)->joinClothesUpdated = true;
                        updateAllClothes(server, peer);
                    }
                    break;
                default:
                    break;
                }
                if (pMov->packetType == 0) //player moves
                {
                }
                if (pMov->packetType == 7) //doors and checkpoints
                {
                }
                if (pMov->packetType == 10) //clothes
                {
                    switch (itemDefs[pMov->plantingTree].clothingType)
                    {
                    case ClothTypes::BACK:
                        pinfo(peer)->back = (pinfo(peer)->back == pMov->plantingTree) ? 0 : pMov->plantingTree;
                        break;
                    case ClothTypes::FACE:
                        if (itemDefs[pMov->plantingTree].blockType == BlockTypes::LOCK)
                        {
                        }
                        else
                        {
                            pinfo(peer)->face = (pinfo(peer)->face == pMov->plantingTree) ? 0 : pMov->plantingTree;
                        }
                        break;
                    case ClothTypes::FEET:
                        pinfo(peer)->feet = (pinfo(peer)->feet == pMov->plantingTree) ? 0 : pMov->plantingTree;
                        break;
                    case ClothTypes::MASK:
                        pinfo(peer)->mask = (pinfo(peer)->mask == pMov->plantingTree) ? 0 : pMov->plantingTree;
                        break;
                    case ClothTypes::HAND:
                        pinfo(peer)->hand = (pinfo(peer)->hand == pMov->plantingTree) ? 0 : pMov->plantingTree;
                        break;
                    case ClothTypes::HAIR:
                        pinfo(peer)->hair = (pinfo(peer)->hair == pMov->plantingTree) ? 0 : pMov->plantingTree;
                        break;
                    case ClothTypes::NECKLACE:
                        pinfo(peer)->neck = (pinfo(peer)->neck == pMov->plantingTree) ? 0 : pMov->plantingTree;
                        break;
                    case ClothTypes::ANCES:
                        pinfo(peer)->ances = (pinfo(peer)->ances == pMov->plantingTree) ? 0 : pMov->plantingTree;
                        break;
                    default:
                        break;
                    }
                    Packets::sendclothes(peer, server);
                }
                if (pMov->packetType == 11) //take
                {
                }
                if (pMov->packetType == 18)
                {
                    sendPData(server, peer, pMov);
                }
                if (pMov->packetType == 23) //consumbles
                {
                }

                if (pMov->punchX != -1 && pMov->punchY != -1) //TODO PLACE BLOCKS
                {
                    if (world != NULL)
                    {
                        if (pMov->packetType == 3)
                        {
                            switch (pMov->plantingTree)
                            {
                            case 18:
                            {
                                if (world->isPublic)
                                {
                                    if (itemDefs[world->items[pMov->punchX + (pMov->punchY * world->width)].foreground].blockType == BlockTypes::LOCK)
                                    {
                                        if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
                                        {
                                            OnPunch(pMov->punchX, pMov->punchY, world, peer, server);
                                        }
                                        else
                                        {
                                            Nothing(peer, pMov->punchX, pMov->punchY);
                                        }
                                    }
                                    else
                                    {
                                        OnPunch(pMov->punchX, pMov->punchY, world, peer, server);
                                    }
                                }
                                else
                                {
                                    if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
                                    {
                                        OnPunch(pMov->punchX, pMov->punchY, world, peer, server);
                                    }
                                    else
                                    {
                                        Nothing(peer, pMov->punchX, pMov->punchY);
                                    }
                                }
                            }
                            break;

                            case 32:
                            {
                            }
                            break;

                            default:
                            {
                                if (world->isPublic)
                                {
                                    if (LEGAL_ITEM(peer, pMov->plantingTree))
                                    {
                                        OnPlace(pMov->punchX, pMov->punchY, pMov->plantingTree, world, peer, server);
                                    }
                                }
                                else
                                {
                                    if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
                                    {
                                        if (LEGAL_ITEM(peer, pMov->plantingTree))
                                        {
                                            OnPlace(pMov->punchX, pMov->punchY, pMov->plantingTree, world, peer, server);
                                        }
                                    }
                                    else
                                    {
                                        Nothing(peer, pMov->punchX, pMov->punchY);
                                    }
                                }
                            }
                            break;
                            }
                        }
                    }
                }

                std::cout << "deleted pMov" << std::endl;
                delete pMov;
            }
            break;
        }
        }
    }
    void Disconnect(ENetPeer *peer)
    {
        if (pinfo(peer)->inv.size() >= 2)
        {
            UPDATE(serialize_player((player *)peer->data), pinfo(peer)->username);
        }
        delete (player *)peer->data; //deleting the struct
        peer->data = NULL;
    }
}