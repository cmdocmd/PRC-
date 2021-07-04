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
#include "commands.h"
#include "worlds.h"

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
                p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGet a GrowID``|left|32|\n\nadd_spacer|small|\nadd_text_input|username|GrowID||30|\nadd_text_input|password|Password||100|\nadd_text_input|email|Email||30|\nend_dialog|register|Cancel|Get My GrowID!|\n");
            }
            else
            {
                if (pinfo(peer)->username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
                {
                    p.consoleMessage(peer, "`rPlease remove special characters before login");
                }
                else if (CHECK_LOGIN(pinfo(peer)->username, pinfo(peer)->password))
                {
                    ENetPeer *currentPeer = player(Host, pinfo(peer)->username);
                    if (currentPeer != NULL)
                    {
                        if (currentPeer != peer)
                        {
                            p.consoleMessage(peer, "Someone else was logged into this account! He was kicked out now.");
                            p.consoleMessage(currentPeer, "`rSomeone else logged into this account!");
                            enet_peer_disconnect_later(currentPeer, 0);
                        }
                    }
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
                            int day = start / (24 * 3600);
                            start = start % (24 * 3600);
                            int hour = start / 3600;
                            start %= 3600;
                            int minutes = start / 60;
                            start %= 60;
                            int seconds = start;
                            p.consoleMessage(peer, "`5Sorry this account is `4banned `o. `5and will be removed in `w" + std::to_string(day) + " days, " + std::to_string(hour) + " hours, " + std::to_string(minutes) + " mins, " + std::to_string(seconds) + " secs. `5contact server developer for more info.");
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
        if (cch.find("action|friends") == 0)
        {
            p.dialog(peer, "set_default_color|`w\n\nadd_label_with_icon|big|Social Portal``|left|1366|\n\nadd_spacer|small|\nadd_button|showfriends|Show Friends``|0|0|\nend_dialog||OK||\nadd_quick_exit|");
        }
        if (cch.find("action|input\n|text|") != std::string::npos)
        {
            commands(cch, peer, Host, server);
        }
        if (cch.find("refresh_item_data") != std::string::npos)
        {
            p.refresh_data(peer);
        }
        if (cch.find("action|store") == 0)
        {
            std::string items = "";
            for (int i = 0; i < static_cast<int>(cant_take.size()); i++)
            {
                items += "\nadd_button_with_icon|store" + std::to_string(cant_take[i]) + "|" + itemDefs[cant_take[i]].name + "|staticBlueFrame|" + std::to_string(cant_take[i]) + "|0|";
            }
            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`7Welcome to the PR store!|left|112|\nadd_spacer|small|" + items);
        }
        if (cch.find("action|mod_trade") == 0)
        {
            std::stringstream ss(cch);
            std::string to;
            int id = -1;
            while (std::getline(ss, to, '\n'))
            {
                std::vector<std::string> ex = explode("|", to);
                if (ex.size() == 2)
                {
                    if (ex[0] == "itemID")
                    {
                        if (is_digits(ex[1]))
                        {
                            id = atoi(ex[1].c_str());
                        }
                    }
                }
            }
            if (id != -1 && id > 0 && id <= itemDefs.size())
            {
                if (id == 18 || id == 32)
                {
                    p.onoverlay(peer, "`wYou'd be sorry sorry if you lost that!");
                }
                else
                {
                    if (LEGAL_ITEM(peer, id))
                    {
                        pinfo(peer)->lasttradeid = id;
                        p.dialog(peer, "add_label_with_icon|big|`2Trade`w " + itemDefs[id].name + "``|left|" + std::to_string(id) + "|\nadd_textbox|`2Trade how many?|\nadd_text_input|itemcount|||5|\nend_dialog|tradedialog|Cancel|Ok|\n");
                    }
                }
            }
        }
        if (cch.find("action|rem_trade") == 0)
        {
            std::stringstream ss(cch);
            std::string to;
            int id = -1;
            int idx = -1;
            while (std::getline(ss, to, '\n'))
            {
                std::vector<std::string> ex = explode("|", to);
                if (ex.size() == 2)
                {
                    if (ex[0] == "itemID")
                    {
                        if (is_digits(ex[1]))
                        {
                            id = atoi(ex[1].c_str());
                        }
                    }
                }
            }
            if (id != -1 && id > 0 && id <= itemDefs.size())
            {
                for (int i = 0; i < static_cast<int>(pinfo(peer)->trade.size()); i++)
                {
                    if (pinfo(peer)->trade[i].itemid == id)
                        idx = i;
                }
                if (idx != -1)
                {
                    pinfo(peer)->trade.erase(pinfo(peer)->trade.begin() + idx);
                    std::string list1 = "";

                    for (int i = 0; i < static_cast<int>(pinfo(peer)->trade.size()); i++)
                    {
                        if (i == 0)
                        {
                            list1 += "add_slot|" + std::to_string(pinfo(peer)->trade[i].itemid) + "|" + std::to_string(pinfo(peer)->trade[i].count);
                        }
                        else
                        {
                            list1 += "\nadd_slot|" + std::to_string(pinfo(peer)->trade[i].itemid) + "|" + std::to_string(pinfo(peer)->trade[i].count);
                        }
                    }
                    pinfo(peer)->itemlist = list1;
                    std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                    for (ENetPeer *currentPeer : ishere)
                    {
                        if (pinfo(currentPeer)->username == pinfo(peer)->tradingme)
                        {
                            if (pinfo(currentPeer)->accepted)
                                pinfo(currentPeer)->accepted = false;
                            p.ontradestatus(peer, pinfo(peer)->netID, "", pinfo(peer)->name, pinfo(peer)->itemlist + "locked|0\nreset_locks|1\naccepted|0", true);
                            p.ontradestatus(currentPeer, pinfo(peer)->netID, "", pinfo(peer)->name, pinfo(peer)->itemlist + "locked|0\nreset_locks|1\naccepted|0", true);
                        }
                        else
                        {
                            p.ontradestatus(peer, pinfo(peer)->netID, "", pinfo(peer)->name, pinfo(peer)->itemlist + "locked|0\nreset_locks|1\naccepted|0", false);
                        }
                    }
                }
            }
        }
        if (cch.find("action|trade_cancel") == 0)
        {
            ENetPeer *currentPeer = player(Host, pinfo(peer)->tradingme);
            if (currentPeer != NULL)
            {
                reset_trade(currentPeer, true);
                if (pinfo(currentPeer)->tradingme == pinfo(peer)->username)
                {
                    p.onoverlay(currentPeer, pinfo(peer)->name + " `wcanceled the trade.");
                }
            }
            reset_trade(peer, true);
            p.onoverlay(peer, "`wYou canceled the trade.");
        }
        if (cch.find("action|trade_accept\nstatus|") == 0)
        {
            std::stringstream ss(cch);
            std::string to;
            int status = -1;
            while (std::getline(ss, to, '\n'))
            {
                std::vector<std::string> ex = explode("|", to);
                if (ex[0] == "status")
                {
                    if (is_digits(ex[1]))
                    {
                        status = atoi(ex[1].c_str());
                    }
                }
            }
            if (status >= 0 || status <= 1)
            {
                ENetPeer *currentPeer = player(Host, pinfo(peer)->tradingme);
                if (currentPeer != NULL)
                {
                    if (status == 1)
                    {
                        if (pinfo(currentPeer)->accepted)
                        {
                            p.onforcetradeend(peer);
                            p.onforcetradeend(currentPeer);
                            std::string list1 = "", list2 = "";
                            for (int i = 0; i < static_cast<int>(pinfo(peer)->trade.size()); i++)
                            {
                                list1 += "\nadd_label_with_icon|small|`o(`w" + std::to_string(pinfo(peer)->trade[i].count) + "`o) " + itemDefs[pinfo(peer)->trade[i].itemid].name + "``|left|" + std::to_string(pinfo(peer)->trade[i].itemid) + "|";
                            }
                            for (int i = 0; i < static_cast<int>(pinfo(currentPeer)->trade.size()); i++)
                            {
                                list2 += "\nadd_label_with_icon|small|`o(`w" + std::to_string(pinfo(currentPeer)->trade[i].count) + "`o) " + itemDefs[pinfo(currentPeer)->trade[i].itemid].name + "``|left|" + std::to_string(pinfo(currentPeer)->trade[i].itemid) + "|";
                            }
                            p.dialog(peer, "add_label_with_icon|big|`wTrade Confirmation``|left|1366|\nadd_spacer|small|\n\nadd_label|small|`4You'll give:``|left|4|\nadd_spacer|small|" + list1 + "\n\nadd_label|small|`2You'll get:``|left|4|\nadd_spacer|small|" + list2 + "\nadd_spacer|small|\nadd_button|dothetrade|`oDo The Trade!``|0|0|\nadd_button|notrade|`oCancel``|0|0|");
                            p.dialog(currentPeer, "add_label_with_icon|big|`wTrade Confirmation``|left|1366|\nadd_spacer|small|\n\nadd_label|small|`4You'll give:``|left|4|\nadd_spacer|small|" + list2 + "\nadd_spacer|small|\n\nadd_label|small|`2You'll get:``|left|4|\nadd_spacer|small|" + list1 + "\nadd_spacer|small|\nadd_button|dothetrade|`oDo The Trade!``|0|0|\nadd_button|notrade|`oCancel``|0|0|");
                        }
                        else
                        {
                            p.ontradestatus(currentPeer, pinfo(peer)->netID, "", pinfo(peer)->name, pinfo(peer)->itemlist + "locked|0\nreset_locks|0\naccepted|1", true);
                            pinfo(peer)->accepted = true;
                        }
                    }
                    if (status == 0)
                    {
                        p.ontradestatus(currentPeer, pinfo(peer)->netID, "", pinfo(peer)->name, pinfo(peer)->itemlist + "locked|0\nreset_locks|0\naccepted|0", true);
                        pinfo(peer)->accepted = false;
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
                std::vector<std::string> ex = explode("|", to);
                if (ex.size() == 3)
                {
                    if (ex[1] == "itemID")
                    {
                        if (is_digits(ex[2]))
                        {
                            int id = std::stoi(ex[2]);
                            if (id <= static_cast<int>(itemDefs.size()) && id > 0)
                            {
                                if (LEGAL_ITEM(peer, id))
                                {
                                    if (id == 18 || id == 32)
                                    {
                                        p.onoverlay(peer, "You can't drop that.");
                                    }
                                    else
                                    {
                                        pinfo(peer)->dropid = id;
                                        p.dialog(peer, "add_label_with_icon|big|`wDrop " + itemDefs[id].name + "``|left|" + std::to_string(id) + "|\nadd_textbox|`oHow many to drop?|\nadd_text_input|dropitemcount|||5|\nadd_textbox|`4Warning: `oAny player who asks you to drop items is scamming you. We cannot restore scammed items.``|\nend_dialog|dropdialog|Cancel|Ok|\n");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (cch.find("action|wrench\n|netid|") == 0)
        {
            std::stringstream ss(cch);
            std::string to;
            while (std::getline(ss, to, '\n'))
            {
                std::vector<std::string> ex = explode("|", to);
                if (ex.size() == 3)
                {
                    if (ex[1] == "netid")
                    {
                        if (is_digits(ex[2]))
                        {
                            int id = std::stoi(ex[2]);

                            Worlds *world = getPlyersWorld(server, peer);
                            std::vector<ENetPeer *> all = All(Host);
                            for (ENetPeer *currentPeer : all)
                            {
                                if (pinfo(currentPeer)->netID == id)
                                {
                                    pinfo(peer)->lastplayerwrench = pinfo(currentPeer)->username;
                                }
                            }
                            if (pinfo(peer)->username == world->owner && pinfo(peer)->adminLevel < 666)
                            {
                                sendWrench(Host, peer, true, false, false, (id == pinfo(peer)->netID));
                            }
                            else if (pinfo(peer)->adminLevel == 666)
                            {
                                sendWrench(Host, peer, false, true, false, (id == pinfo(peer)->netID));
                            }
                            else if (pinfo(peer)->adminLevel == 999)
                            {
                                sendWrench(Host, peer, false, false, true, (id == pinfo(peer)->netID));
                            }
                            else
                            {
                                sendWrench(Host, peer, false, false, false, (id == pinfo(peer)->netID));
                            }
                        }
                    }
                }
            }
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
        break;
    }
    case 3:
    {
        std::cout << "from case3: \n"
                  << cch << "\n";

        std::vector<std::string> ex = explode("|", cch);

        if (ex.size() == 4 || ex.size() == 2)
        {
            if (ex[0] == "action")
            {
                if (ex[1] == "quit")
                {
                    sendPlayerLeave(Host, peer);
                    pinfo(peer)->currentWorld = "EXIT";
                    enet_peer_disconnect_later(peer, 0);
                }
                if (ex[1] == "quit_to_exit")
                {
                    for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
                    {
                        if (pinfo(peer)->inv[i].itemID == 5640)
                        {
                            pinfo(peer)->inv.erase(pinfo(peer)->inv.begin() + i);
                        }
                    }
                    sendPlayerLeave(Host, peer);
                    pinfo(peer)->currentWorld = "EXIT";
                    SendWorldOffers(server, Host, peer);
                }
                if (ex[1] == "join_request\nname")
                {
                    if (!pinfo(peer)->InLobby || !pinfo(peer)->isCorrect)
                    {
                        break;
                    }
                    std::vector<std::string> ee = explode("\n", ex[2].c_str());
                    joinWorld(server, Host, peer, ee[0], 0, 0);
                }
            }
        }
        break;
    }
    case 4:
    {
        std::cout << "from case4: " << cch << "\n";
        BYTE *tankUpdatePacket = GetStructPointerFromTankPacket(packet);
        Worlds *world = getPlyersWorld(server, peer);
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
                        sendInventory(peer);
                    }
                }
                else
                {
                    if (pinfo(peer)->adminLevel >= 666)
                    {
                    }
                    else
                    {
                        p.onsetpos(peer, pinfo(peer)->x, pinfo(peer)->y);
                    }
                }
            }
            if (pMov->punchX >= 0 && pMov->punchY >= 0 && pMov->punchX < 100 && pMov->punchY < 100) //TODO PLACE BLOCKS
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
                                    if (pinfo(peer)->hand == 5480)
                                    {
                                        
                                    }
                                    OnPunch(pMov->punchX, pMov->punchY, world, peer, Host, server);
                                }
                                else
                                {
                                    Nothing(peer, pMov->punchX, pMov->punchY);
                                }
                            }
                            else
                            {
                                OnPunch(pMov->punchX, pMov->punchY, world, peer, Host, server);
                            }
                        }
                        else
                        {
                            if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
                            {
                                OnPunch(pMov->punchX, pMov->punchY, world, peer, Host, server);
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
                                OnPlace(pMov->punchX, pMov->punchY, pMov->plantingTree, world, peer, Host, server);
                            }
                        }
                        else
                        {
                            if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666 || itemDefs[pMov->plantingTree].blockType == BlockTypes::CONSUMMABLE)
                            {
                                if (LEGAL_ITEM(peer, pMov->plantingTree))
                                {
                                    OnPlace(pMov->punchX, pMov->punchY, pMov->plantingTree, world, peer, Host, server);
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
                if (pMov->packetType == 7)
                {
                    int x = pMov->punchX;
                    int y = pMov->punchY;
                    int tile = world->items[x][y].foreground;
                    if (tile == 6)
                    {
                        sendPlayerLeave(Host, peer);
                        SendWorldOffers(server, Host, peer);
                    }
                    if (itemDefs[tile].blockType == BlockTypes::CHECKPOINT)
                    {
                        pinfo(peer)->cpx = x * 32;
                        pinfo(peer)->cpy = y * 32;
                        p.setrespawnpos(peer, x, y);
                    }
                    if (tile == 762)
                    {
                        pinfo(peer)->wrenchx = x;
                        pinfo(peer)->wrenchy = y;
                        std::string dest = "", iop = "", label = itemDefs[tile].name;
                        for (int i = 0; i < static_cast<int>(world->door.size()); i++)
                        {
                            if (world->door[i].x == x && world->door[i].y == y)
                            {
                                dest = world->door[i].dest;
                                iop = world->door[i].is_open;
                                if (world->door[i].label != "")
                                    label = world->door[i].label;
                            }
                        }
                        if (iop == "0")
                        {
                            if (pinfo(peer)->userID != world->id && pinfo(peer)->adminLevel < 666 && world->owner != "")
                            {
                                p.ontalkbubble(peer, pinfo(peer)->netID, "`wThe door is locked.", 1);
                                p.onzoomcamera(peer, 2);
                                p.onfreezestate(peer, 0);
                            }
                            else
                            {
                                p.onzoomcamera(peer, 2);
                                p.onfreezestate(peer, 0);
                                p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|" + label + "|left|" + std::to_string(itemDefs[tile].id) + "|\n\nadd_label|small|`oThe door requires password.``|left|4|\n\nadd_text_input|pass|`oPassword||24|\nend_dialog|pdoor|Cancel|OK|\n");
                            }
                        }
                        else
                        {
                            p.onzoomcamera(peer, 2);
                            p.onfreezestate(peer, 0);
                            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|" + label + "|left|" + std::to_string(itemDefs[tile].id) + "|\n\nadd_label|small|`oThe door requires password.``|left|4|\n\nadd_text_input|pass|`oPassword||24|\nend_dialog|pdoor|Cancel|OK|\n");
                        }
                    }
                    if (itemDefs[tile].blockType == BlockTypes::DOOR && tile != 762)
                    {
                        std::string dest = "", iop = "";
                        for (int i = 0; i < static_cast<int>(world->door.size()); i++)
                        {
                            if (world->door[i].x == x && world->door[i].y == y)
                            {
                                dest = world->door[i].dest;
                                iop = world->door[i].is_open;
                            }
                        }
                        if (iop == "0")
                        {
                            if (pinfo(peer)->userID != world->id && pinfo(peer)->adminLevel < 666 && world->owner != "")
                            {
                                p.ontalkbubble(peer, pinfo(peer)->netID, "`wThe door is locked.", 1);
                                p.onzoomcamera(peer, 2);
                                p.onfreezestate(peer, 0);
                            }
                            else
                            {
                                if (dest == "exit" || dest == "EXIT")
                                {
                                    sendPlayerLeave(Host, peer);
                                    pinfo(peer)->currentWorld = "EXIT";
                                    SendWorldOffers(server, Host, peer);
                                }
                                if (dest.find(":") != std::string::npos)
                                {
                                    std::vector<std::string> ex = explode(":", dest);
                                    if (ex[0] == "")
                                    {
                                        int xo = 0;
                                        int yo = 0;
                                        for (int i = 0; i < static_cast<int>(world->door.size()); i++)
                                        {
                                            if (world->door[i].id == ex[1])
                                            {
                                                xo = world->door[i].x * 32;
                                                yo = world->door[i].y * 32;
                                            }
                                        }
                                        if (xo != 0 && yo != 0)
                                        {
                                            p.onsetpos(peer, xo, yo);
                                            p.onzoomcamera(peer, 2);
                                            p.onfreezestate(peer, 0);
                                        }
                                        else
                                        {
                                            for (int j = 0; j < world->Width * world->Height; j++)
                                            {
                                                if (world->items[j % world->Width][j / world->Width].foreground == 6)
                                                {
                                                    xo = (j % world->Width) * 32;
                                                    yo = (j / world->Width) * 32;
                                                }
                                            }
                                            p.onsetpos(peer, xo, yo);
                                            p.onzoomcamera(peer, 2);
                                            p.onfreezestate(peer, 0);
                                        }
                                    }
                                    else
                                    {
                                        int xo = 0;
                                        int yo = 0;
                                        Worlds *info = GET_WORLD(server, ex[0]).ptr;
                                        for (int i = 0; i < static_cast<int>(info->door.size()); i++)
                                        {
                                            if (info->door[i].id == ex[1])
                                            {
                                                xo = info->door[i].x * 32;
                                                yo = info->door[i].y * 32;
                                            }
                                        }
                                        if (xo != 0 && yo != 0)
                                        {
                                            sendPlayerLeave(Host, peer);
                                            pinfo(peer)->currentWorld = ex[0];
                                            joinWorld(server, Host, peer, ex[0], xo, yo);
                                        }
                                        else
                                        {
                                            sendPlayerLeave(Host, peer);
                                            pinfo(peer)->currentWorld = ex[0];
                                            joinWorld(server, Host, peer, ex[0], 0, 0);
                                        }
                                    }
                                }
                                else
                                {
                                    sendPlayerLeave(Host, peer);
                                    pinfo(peer)->currentWorld = dest;
                                    joinWorld(server, Host, peer, dest, 0, 0);
                                }
                            }
                        }
                        else
                        {
                            if (dest == "exit" || dest == "EXIT")
                            {
                                sendPlayerLeave(Host, peer);
                                pinfo(peer)->currentWorld = "EXIT";
                                SendWorldOffers(server, Host, peer);
                            }
                            if (dest.find(":") != std::string::npos)
                            {
                                std::vector<std::string> ex = explode(":", dest);
                                if (ex[0] == "")
                                {
                                    int xo = 0;
                                    int yo = 0;
                                    for (int i = 0; i < static_cast<int>(world->door.size()); i++)
                                    {
                                        if (world->door[i].id == ex[1])
                                        {
                                            xo = world->door[i].x * 32;
                                            yo = world->door[i].y * 32;
                                        }
                                    }
                                    if (xo != 0 && yo != 0)
                                    {
                                        p.onsetpos(peer, xo, yo);
                                        p.onzoomcamera(peer, 2);
                                        p.onfreezestate(peer, 0);
                                    }
                                    else
                                    {
                                        for (int j = 0; j < world->Width * world->Height; j++)
                                        {
                                            if (world->items[j % world->Width][j / world->Width].foreground == 6)
                                            {
                                                xo = (j % world->Width) * 32;
                                                yo = (j / world->Width) * 32;
                                            }
                                        }
                                        p.onsetpos(peer, xo, yo);
                                        p.onzoomcamera(peer, 2);
                                        p.onfreezestate(peer, 0);
                                    }
                                }
                                else
                                {
                                    int xo = 0;
                                    int yo = 0;
                                    Worlds *info = GET_WORLD(server, ex[0]).ptr;
                                    for (int i = 0; i < static_cast<int>(info->door.size()); i++)
                                    {
                                        if (info->door[i].id == ex[1])
                                        {
                                            xo = info->door[i].x * 32;
                                            yo = info->door[i].y * 32;
                                        }
                                    }
                                    if (xo != 0 && yo != 0)
                                    {
                                        sendPlayerLeave(Host, peer);
                                        pinfo(peer)->currentWorld = ex[0];
                                        joinWorld(server, Host, peer, ex[0], xo, yo);
                                    }
                                    else
                                    {
                                        sendPlayerLeave(Host, peer);
                                        pinfo(peer)->currentWorld = ex[0];
                                        joinWorld(server, Host, peer, ex[0], 0, 0);
                                    }
                                }
                            }
                            else
                            {
                                sendPlayerLeave(Host, peer);
                                pinfo(peer)->currentWorld = dest;
                                joinWorld(server, Host, peer, dest, 0, 0);
                            }
                        }
                    }
                }
            }
            if (pMov->packetType == 10) //clothes
            {
                if (pMov->plantingTree <= static_cast<int>(itemDefs.size()))
                {
                    if (LEGAL_ITEM(peer, pMov->plantingTree))
                    {
                        bool send = true;
                        switch (itemDefs[pMov->plantingTree].clothingType)
                        {
                        case ClothTypes::BACK:
                            if (pMov->plantingTree == 7166 || pMov->plantingTree == 5078 || pMov->plantingTree == 5080 || pMov->plantingTree == 5082 || pMov->plantingTree == 5084 || pMov->plantingTree == 5126 || pMov->plantingTree == 5128 || pMov->plantingTree == 5130 || pMov->plantingTree == 5132 || pMov->plantingTree == 5144 || pMov->plantingTree == 5146 || pMov->plantingTree == 5148 || pMov->plantingTree == 5150 || pMov->plantingTree == 5162 || pMov->plantingTree == 5164 || pMov->plantingTree == 5166 || pMov->plantingTree == 5168 || pMov->plantingTree == 5180 || pMov->plantingTree == 5182 || pMov->plantingTree == 5184 || pMov->plantingTree == 5186 || pMov->plantingTree == 7168 || pMov->plantingTree == 7170 || pMov->plantingTree == 7172 || pMov->plantingTree == 7174)
                            {
                                pinfo(peer)->ances = (pinfo(peer)->ances == pMov->plantingTree) ? 0 : pMov->plantingTree;
                            }
                            else
                            {
                                pinfo(peer)->back = (pinfo(peer)->back == pMov->plantingTree) ? 0 : pMov->plantingTree;
                                if (pinfo(peer)->back == 0)
                                {
                                    pinfo(peer)->canDoubleJump = false;
                                }
                                else
                                {
                                    pinfo(peer)->canDoubleJump = true;
                                }
                            }
                            sendState(Host, peer);
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
                                if (pMov->plantingTree == 242)
                                {
                                    send = false;
                                    int idx = -1, ido = -1;
                                    for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
                                    {
                                        if (pinfo(peer)->inv[i].itemID == 242)
                                            idx = i;
                                        if (pinfo(peer)->inv[i].itemID == 1796)
                                            ido = i;
                                    }
                                    if (idx != -1)
                                    {
                                        int amount = pinfo(peer)->inv[idx].itemCount;
                                        if (!MoreThan200(peer, 1796, 1) && amount - 100 >= 0)
                                        {
                                            if (ido == -1)
                                            {
                                                Add_ITEM(peer, 1796, 1, true);
                                                Rem(peer, 242, 100, true);
                                            }
                                            else
                                            {
                                                Add(peer, 1796, 1, true);
                                                Rem(peer, 242, 100, true);
                                            }
                                            p.consoleMessage(peer, "`wYou compressed 100 `2World Lock `winto a `2Diamond Lock `w!");
                                            p.ontalkbubble(peer, pinfo(peer)->netID, "`wYou compressed 100 `2World Lock `winto a `2Diamond Lock `w!", 1);
                                        }
                                    }
                                }
                                if (pMov->plantingTree == 1796)
                                {
                                    send = false;
                                    int idx = -1, ido = -1;
                                    for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
                                    {
                                        if (pinfo(peer)->inv[i].itemID == 1796)
                                            idx = i;
                                        if (pinfo(peer)->inv[i].itemID == 242)
                                            ido = i;
                                    }
                                    if (idx != -1)
                                    {
                                        int amount = pinfo(peer)->inv[idx].itemCount;
                                        if (!MoreThan200(peer, 242, 100) && amount - 1 >= 0)
                                        {
                                            if (ido == -1)
                                            {
                                                Add_ITEM(peer, 242, 100, true);
                                                Rem(peer, 1796, 1, true);
                                            }
                                            else
                                            {
                                                Add(peer, 242, 100, true);
                                                Rem(peer, 1796, 1, true);
                                            }
                                            p.consoleMessage(peer, "`wYou compressed a `2Diamond Lock `winto 100 `2World Lock `w!");
                                            p.ontalkbubble(peer, pinfo(peer)->netID, "`wYou compressed a `2Diamond Lock `winto 100 `2World Lock `w!", 1);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                pinfo(peer)->hair = (pinfo(peer)->hair == pMov->plantingTree) ? 0 : pMov->plantingTree;
                            }
                            break;
                        case ClothTypes::NECKLACE:
                            pinfo(peer)->neck = (pinfo(peer)->neck == pMov->plantingTree) ? 0 : pMov->plantingTree;
                            break;
                        default:
                            break;
                        }
                        if (send)
                        {
                            sendclothes(Host, peer);
                            sendmusic(peer, "change_clothes");
                        }
                    }
                }
            }
            if (pMov->packetType == 11)
            {
                int UID = pMov->plantingTree;
                int idx = -1;
                for (int i = 0; i < static_cast<int>(world->droppeditems.size()); i++)
                {
                    if (world->droppeditems[i].uid == UID)
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
                        p.onsetbux(peer);
                    }
                    else
                    {
                        if (!MoreThan200(peer, id, am))
                        {
                            int o = -1;
                            for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
                            {
                                if (pinfo(peer)->inv[i].itemID == id)
                                {
                                    o = i;
                                    pinfo(peer)->inv[i].itemCount += am;
                                }
                            }
                            if (o == -1)
                            {
                                Add_ITEM(peer, id, am, false);
                            }
                        }
                        else
                        {
                            for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
                            {
                                if (pinfo(peer)->inv[i].itemID == id)
                                {
                                    int total = pinfo(peer)->inv[i].itemCount + am;
                                    DropItem(server, Host, peer, -1, (float)pinfo(peer)->x, (float)pinfo(peer)->y, id, total - 200, 0);
                                    pinfo(peer)->inv[i].itemCount = 200;
                                }
                            }
                        }
                    }
                    world->droppeditems.erase(world->droppeditems.begin() + idx);
                    world->dropsize = world->droppeditems.size();
                    sendTake(Host, peer, pinfo(peer)->netID, xx, yy, UID);
                    if (itemDefs[id].rarity == 999)
                    {
                        if (id != 112)
                        {
                            p.consoleMessage(peer, "`oCollected `w" + std::to_string(am) + " " + itemDefs[id].name + "`o.");
                        }
                    }
                    else
                    {
                        p.consoleMessage(peer, "`oCollected `w" + std::to_string(am) + " " + itemDefs[id].name + "`o. Rarity: `w" + std::to_string(itemDefs[id].rarity) + "`o.");
                    }
                }
                std::cout << UID << "\n";
            }
            std::cout << "deleting pMov\n";
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
    sendPlayerLeave(Host, peer);
    reset_trade(peer, false);
    if (pinfo(peer)->inv.size() >= 2)
    {
        for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
        {
            if (pinfo(peer)->inv[i].itemID == 5640)
            {
                pinfo(peer)->inv.erase(pinfo(peer)->inv.begin() + i);
            }
        }
        UPDATE(serialize_player((Players *)peer->data), pinfo(peer)->username);
    }
    delete (Players *)peer->data; //deleting the struct
    peer->data = NULL;
}