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
                    }
                    else if (CHECK_LOGIN(pinfo(peer)->username, pinfo(peer)->password)) //correct username and password
                    {
                        std::istream *blobdata = PLAYER_DATA(pinfo(peer)->username);
                        delete (player *)peer->data;
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
                        peer->data = ply;
                        pinfo(peer)->isCorrect = true;
                        pinfo(peer)->InLobby = true;
                        pinfo(peer)->userID = PLAYER_ID(pinfo(peer)->username);
                        if (pinfo(peer)->ban != 0)
                        {
                            int to = pinfo(peer)->bantime;
                            time_t now = time(NULL);
                            time_t banned = pinfo(peer)->ban;
                            if (now - banned > to)
                            {
                                pinfo(peer)->ban = 0;
                                pinfo(peer)->bantime = 0;
                                Packets::consoleMessage(peer, "`$Your ban have been removed please behave better this time");
                                SendWorldOffers(peer);
                            }
                            else
                            {
                                int left = now - banned;
                                int start = to - left;
                                time_left(peer, start, "banned");
                            }
                        }
                        else
                        {
                            SendWorldOffers(peer);
                        }
                    }
                    else
                    {
                        Packets::consoleMessage(peer, "`rWrong username or password");
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
            if (cch.find("action|respawn") == 0)
            {
                if (cch.find("action|respawn_spike") == 0)
                {
                    Respawn(peer, true);
                }
                else
                {
                    Respawn(peer, false);
                }
            }
            if (cch.find("action|wrench\n|netid|") == 0)
            {
                std::stringstream ss(cch);
                std::string to;
                while (std::getline(ss, to, '\n'))
                {
                    std::vector<string> ex = explode("|", to);
                    if (ex.size() == 3)
                    {
                        if (ex[1] == "netid")
                        {
                            if (is_digits(ex[2]))
                            {
                                int id = stoi(ex[2]);
                                if (id > 1 || id <= static_cast<int>(itemDefs.size()))
                                {
                                    Worlds *world = getPlyersWorld(peer);
                                    if (pinfo(peer)->username == world->owner && pinfo(peer)->adminLevel < 666)
                                    {
                                        sendWrench(peer, true, false, false, (id == pinfo(peer)->netID));
                                    }
                                    else if (pinfo(peer)->adminLevel == 666)
                                    {
                                        sendWrench(peer, false, true, false, (id == pinfo(peer)->netID));
                                    }
                                    else if (pinfo(peer)->adminLevel == 999)
                                    {
                                        sendWrench(peer, false, false, true, (id == pinfo(peer)->netID));
                                    }
                                    else
                                    {
                                        sendWrench(peer, false, false, false, (id == pinfo(peer)->netID));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (cch.find("action|drop\n|itemID|") == 0)
            {
                std::stringstream ss(cch);
                std::string to;
                while (std::getline(ss, to, '\n'))
                {
                    std::vector<string> ex = explode("|", to);
                    if (ex.size() == 3)
                    {
                        if (ex[1] == "itemID")
                        {
                            if (is_digits(ex[2]))
                            {
                                int id = stoi(ex[2]);
                                if (id <= static_cast<int>(itemDefs.size()) && id > 0)
                                {
                                    if (LEGAL_ITEM(peer, id))
                                    {
                                        if (id == 18 || id == 32)
                                        {
                                            Packets::onoverlay(peer, "You can't drop that.");
                                        }
                                        else
                                        {
                                            pinfo(peer)->dropid = id;
                                            Packets::dialog(peer, "add_label_with_icon|big|`wDrop " + itemDefs[id].name + "``|left|" + std::to_string(id) + "|\nadd_textbox|`oHow many to drop?|\nadd_text_input|dropitemcount|||5|\nadd_textbox|`4Warning: `oAny player who asks you to drop items is scamming you. We cannot restore scammed items.``|\nend_dialog|dropdialog|Cancel|Ok|\n");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (cch.find("refresh_item_data") != std::string::npos)
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
                    if (!pinfo(peer)->isCorrect)
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
                    if (ex[1] == "quit_to_exit")
                    {
                        sendPlayerLeave(server, peer, pinfo(peer));
                        pinfo(peer)->currentWorld = "EXIT";
                        SendWorldOffers(peer);
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
                        sendState(server, peer);
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
                    if (pMov->plantingTree <= static_cast<int>(itemDefs.size()))
                    {
                        if (LEGAL_ITEM(peer, pMov->plantingTree))
                        {
                            switch (itemDefs[pMov->plantingTree].clothingType)
                            {
                            case ClothTypes::BACK:
                                pinfo(peer)->back = (pinfo(peer)->back == pMov->plantingTree) ? 0 : pMov->plantingTree;
                                if (pinfo(peer)->back == 0)
                                {
                                    pinfo(peer)->canDoubleJump = false;
                                }
                                else
                                {
                                    pinfo(peer)->canDoubleJump = true;
                                }
                                sendState(server, peer);
                                break;
                            case ClothTypes::FACE:
                                pinfo(peer)->face = (pinfo(peer)->face == pMov->plantingTree) ? 0 : pMov->plantingTree;
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
                                if (itemDefs[pMov->plantingTree].blockType == BlockTypes::LOCK)
                                {
                                }
                                else
                                {
                                    pinfo(peer)->hair = (pinfo(peer)->hair == pMov->plantingTree) ? 0 : pMov->plantingTree;
                                }
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
                    }
                }
                if (pMov->packetType == 11) //take
                {
                    int UID = pMov->plantingTree;
                    if (UID > 0)
                    {
                        if (world != NULL)
                        {
                            int idx = -1;
                            for (int i = 0; i < static_cast<int>(world->droppeditems.size()); i++)
                            {
                                if (world->droppeditems[i].uid == UID)
                                {
                                    idx = i;
                                }
                                if (idx != -1)
                                {
                                    int id = world->droppeditems[idx].id;
                                    float xx = world->droppeditems[idx].x;
                                    float yy = world->droppeditems[idx].y;
                                    int am = world->droppeditems[idx].count;
                                    if (id == 112)
                                    {
                                        pinfo(peer)->gem += am;
                                        // packet::onsetbux(peer);
                                    }
                                    else
                                    {
                                        int index = -1;
                                        for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
                                        {
                                            if (pinfo(peer)->inv[i].itemID == id)
                                            {
                                                index = i;
                                            }
                                        }
                                        if (index != -1)
                                        {
                                            int total = pinfo(peer)->inv[index].itemCount + am;
                                            if (total > 200)
                                            {
                                                pinfo(peer)->inv[index].itemCount = 200;
                                                DropItem(server, peer, -1, (float)pinfo(peer)->x + (32 * (pinfo(peer)->isRotatedLeft ? -1 : 1)), (float)pinfo(peer)->y, id, total - 200, 0);
                                            }
                                            else
                                            {
                                                pinfo(peer)->inv[index].itemCount += am;
                                            }
                                        }
                                        else
                                        {
                                            Add_ITEM(peer, id, am);
                                        }
                                    }
                                    world->droppeditems.erase(world->droppeditems.begin() + idx);
                                    world->dropsize = world->droppeditems.size();
                                    sendTake(server, peer, pinfo(peer)->netID, xx, yy, UID);
                                }
                                else
                                {
                                    Packets::ontalkbubble(peer, pinfo(peer)->netID, "`wToo Far Away");
                                }
                            }
                        }
                    }
                }
                if (pMov->packetType == 18)
                {
                    sendPData(server, peer, pMov);
                }
                if (pMov->packetType == 23) //consumbles
                {
                }

                if (pMov->punchX >= 0 && pMov->punchY >= 0 && pMov->punchX < 100 && pMov->punchY < 100) //TODO PLACE BLOCKS
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
                                    if (itemDefs[world->items[pMov->punchX][pMov->punchY].foreground].blockType == BlockTypes::LOCK)
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
                                int block = world->items[pMov->punchX][pMov->punchY].foreground;
                                if (world->isPublic)
                                {
                                    if (pinfo(peer)->username != world->owner && pinfo(peer)->adminLevel < 666)
                                    {
                                        if (itemDefs[block].blockType != BlockTypes::LOCK)
                                        {
                                            onWrench(world, pMov->punchX, pMov->punchY, peer);
                                        }
                                    }
                                    else
                                    {
                                        onWrench(world, pMov->punchX, pMov->punchY, peer);
                                    }
                                }
                                else
                                {
                                    if (block == 6016) //something all can wrench like growscan
                                    {
                                        onWrench(world, pMov->punchX, pMov->punchY, peer);
                                    }
                                    else
                                    {
                                        if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
                                        {
                                            onWrench(world, pMov->punchX, pMov->punchY, peer);
                                        }
                                    }
                                }
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
        Reset_Values(peer);
        if (pinfo(peer)->inv.size() >= 2)
        {
            UPDATE(serialize_player((player *)peer->data), pinfo(peer)->username);
        }
        delete (player *)peer->data; //deleting the struct
        peer->data = NULL;
    }
}