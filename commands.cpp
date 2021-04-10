#include <iostream>
#include "enet/include/enet/enet.h"
#include "player.cpp"
#include "GamePacketBuilder.cpp"
#include "worlds.cpp"
#include "pathfinder.cpp"

using std::string;
std::vector<string> explode(const string &delimiter, const string &str);
void UPDATE(std::stringstream str, std::string growid);
std::istream *PLAYER_DATA(std::string growid);
/*void ApplyLockPacketGTOS(Worlds *world, ENetPeer *peer, int x, int y, int id, int lockowner)
{
    int locksize = 0;
    if (id == 202)
        locksize = 11;
    PlayerMoving pmov;
    pmov.packetType = 0xf;
    pmov.characterState = 0;
    pmov.x = 0;
    pmov.y = 0;
    pmov.XSpeed = 0;
    pmov.YSpeed = 0;
    pmov.plantingTree = id;
    pmov.punchX = x;
    pmov.punchY = y;
    pmov.netID = lockowner;
    std::vector<BYTE> pmovp = packPlayerMoving(&pmov);
    std::vector<BYTE> packet(64 + locksize * 2);
    packet[0] = 4;
    memcpy(&packet[4], &pmovp[0], 56);
    packet[12] = locksize;
    packet[16] = 8;
    int locksz = locksize * 2;
    memcpy(&packet[56], &locksz, 4);
    int lock_place = x + y * world->width;
    std::cout << "place of sl: " << lock_place << std::endl;
    int fml = 0;
    if (world->items[lock_place + 100].foreground == 6)
    {
        for (int i = 0; i < locksize; i++)
        {
            if (i == 0)
            {
                fml = lock_place - 200;
            }
            if (i == 1)
            {
                fml = lock_place - 100 - i;
            }
            if (i == 2 || i == 3 || i == 5 || i == 7)
            {
                fml += 1;
            }
            if (i == 4)
            {
                fml = lock_place - 2;
            }
            if (i == 6 || i == 9)
            {
                fml += 2;
            }
            if (i == 8)
            {
                fml = fml + 97;
            }
            memcpy(&packet[world->height + i * 2], &fml, 2);
            std::cout << i << " " << fml << std::endl;
        }
    }
    else
    {
        for (int i = 0; i < locksize; i++)
        {
            if (i == 0)
            {
                fml = lock_place - 200;
            }
            if (i == 1)
            {
                fml = lock_place - 100 - i;
            }
            if (i == 2 || i == 3 || i == 5 || i == 8 || i == 9)
            {
                fml += 1;
            }
            if (i == 4)
            {
                fml = lock_place - 2;
            }
            if (i == 6)
            {
                fml += 2;
            }
            if (i == 7)
            {
                fml = fml + 98;
            }
            memcpy(&packet[world->height + i * 2], &fml, 2);
            std::cout << i << " " << fml << std::endl;
        }
    }
    ENetPacket *packetenet = enet_packet_create((void *)&packet[0], 64 + locksize * 2, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packetenet);
}*/
void Commands_Handler(std::string cch, ENetPeer *peer, ENetHost *server)
{
    std::stringstream ss(cch);
    std::string to;
    string str = "";
    while (std::getline(ss, to, '\n'))
    {
        std::vector<string> ex = explode("|", to);
        if (ex.size() == 3)
        {
            if (ex[1] == "text" && ex[2].c_str() != NULL)
            {
                str = ex[2];
            }
        }
    }
    if (str.length() > 120 || 1 > (str.size() - countSpaces(str)))
    {
        return;
    }
    if (str == "/find")
    {
        Packets::dialog(peer, "add_label_with_icon|big|`wFind item``|left|6016|\nadd_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\nadd_quick_exit|\n");
    }
    if (str == "/test")
    {
        int x = pinfo(peer)->x / 32;
        int y = pinfo(peer)->y / 32;
        Worlds * world = getPlyersWorld(peer);
        Pair src = std::make_pair(53, 34);// place of area we need to check if there is a destination
        Pair dest = std::make_pair(x, y);
        aStarSearch(world, src, dest);
    }
   /* if (str == "/test")
    {
        Worlds *world = getPlyersWorld(peer);
        int x;
        int y;
        for (int i = 0; i < world->width * world->height; i++)
        {
            if (world->items[i].foreground == 202)
            {

                x = (i % world->width);
                y = (i / world->width);
                //ApplyLockPacket(peer, x, y, 202, 5, pdata(peer)->netID);
                ApplyLockPacketGTOS(world, peer, x, y, 202, pinfo(peer)->netID);
            }
        }
    }*/
    if (str.substr(0, 5) == "/ban ")
    {
        if (pinfo(peer)->adminLevel >= 666)
        {
            Packets::dialog(peer, "add_label_with_icon|big|`4Ban Panel``|left|32|\nadd_textbox|`1User: " + str.substr(5, str.length() - 5) + "|\nadd_textbox|`rKeep in mind! Maximum ban you can apply is 730 days.|\nadd_textbox|`1Choose the ban amount|\nadd_text_input|b-days|`oDays:|0|4|\nadd_text_input|b-hours|`oHours:|0|4|\nadd_text_input|b-mint|`oMinutes:|0|4|\nend_dialog|punish-ban-" + str.substr(5, str.length() - 5) + "|Cancel|`4BAN|");
        }
    }
    if (str.substr(0, 6) == "/mute ")
    {
        if (pinfo(peer)->adminLevel >= 666)
        {
            Packets::dialog(peer, "add_label_with_icon|big|`#Mute Panel``|left|32|\nadd_textbox|`1User: " + str.substr(6, str.length() - 6) + "|\nadd_textbox|`rKeep in mind! Maximum mute you can apply is 60 days.|\nadd_textbox|`1Choose the mute amount|\nadd_text_input|m-days|`oDays:|0|4|\nadd_text_input|m-hours|`oHours:|0|4|\nadd_text_input|m-mint|`oMinutes:|0|4|\nend_dialog|punish-mute-" + str.substr(6, str.length() - 6) + "|Cancel|`#MUTE|");
        }
    }
    if (str.substr(0, 7) == "/curse ")
    {
        if (pinfo(peer)->adminLevel >= 666)
        {
            Packets::dialog(peer, "add_label_with_icon|big|`bCurse Panel``|left|32|\nadd_textbox|`1User: " + str.substr(7, str.length() - 7) + "|\nadd_textbox|`rKeep in mind! Maximum curse you can apply is 60 days.|\nadd_textbox|`1Choose the curse amount|\nadd_text_input|c-days|`oDays:|0|4|\nadd_text_input|c-hours|`oHours:|0|4|\nadd_text_input|c-mint|`oMinutes:|0|4|\nend_dialog|punish-curse-" + str.substr(7, str.length() - 7) + "|Cancel|`bCURSE|");
        }
    }
    if (str.substr(0, 6) == "/wban ")
    {
        Worlds *world = getPlyersWorld(peer);
        if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
        {
            bool found = false;
            string name = str.substr(6, str.length() - 6);
            if (name == pinfo(peer)->username)
            {
                return;
            }
            for (int i = 0; i < static_cast<int>(world->bans.size()); i++)
            {
                if (world->bans[i].name == name)
                {
                    found = true;
                }
            }
            if (!found)
            {
                bool banned = false;
                string nm = "";
                ENetPeer *currentPeer;
                for (currentPeer = server->peers;
                     currentPeer < &server->peers[server->peerCount];
                     ++currentPeer)
                {
                    if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                        continue;
                    if (pinfo(currentPeer)->currentWorld == pinfo(peer)->currentWorld)
                    {
                        if (pinfo(currentPeer)->username == name)
                        {
                            WorldBans ban;
                            ban.name = name;
                            ban.time = time(NULL);
                            world->bans.push_back(ban);
                            banned = true;
                            nm = pinfo(currentPeer)->name;
                            sendPlayerLeave(server, currentPeer, pinfo(currentPeer));
                            SendWorldOffers(currentPeer);
                        }
                        else
                        {
                            Packets::consoleMessage(peer, "`4Oops: `oThere is nobody currently in this world with a name `w" + name);
                        }
                        if (banned)
                        {
                            Packets::consoleMessage(currentPeer, "`o" + pinfo(peer)->name + " `4world bans `o" + nm + " from `w" + pinfo(peer)->currentWorld);
                            Packets::consoleMessage(peer, "`oYou've banned " + nm + " `ofrom `w" + pinfo(peer)->currentWorld + " `ofor one hour. You can also type `#/uba `oto unban him/her early.");
                            sendmusic(currentPeer, "repair");
                        }
                    }
                }
            }
        }
    }
    if (str == "/uba")
    {
        Worlds *world = getPlyersWorld(peer);
        if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
        {
            Packets::consoleMessage(peer, "`oYou've unbanned everybody from this world.");
            world->bans.clear();
        }
    }
    if (str == "/test")
    {
        std::cout << pinfo(peer)->x / 32 << " " << pinfo(peer)->y / 32 << std::endl;
    }
    if (str.substr(0, 5) == "/nuke")
    {
    }
    if (str == "/nuke1")
    {
        if (pinfo(peer)->adminLevel >= 666)
        {
            Worlds *world = getPlyersWorld(peer);
            world->isNuked = true;
            if (static_cast<int>(world->access.size()) == 0)
            {
                ENetPeer *currentPeer;
                for (currentPeer = server->peers;
                     currentPeer < &server->peers[server->peerCount];
                     ++currentPeer)
                {
                    if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                        continue;
                    Packets::consoleMessage(currentPeer, "`#** `$The Ancients `ohave used `4Ban `oon `w" + world->owner + "`o! `#**");
                    sendmusic(currentPeer, "beep");
                    sendmusic(currentPeer, "bigboom");
                    Packets::consoleMessage(currentPeer, "`o>>`4" + world->name + " `4was nuked from orbit`o. It's the only way to be sure. Play nice, everybody!");
                    if (pinfo(currentPeer)->currentWorld == pinfo(peer)->currentWorld)
                    {
                        sendPlayerLeave(server, currentPeer, pinfo(currentPeer));
                        pinfo(currentPeer)->currentWorld = "EXIT";
                        SendWorldOffers(currentPeer);
                    }
                    if (pinfo(currentPeer)->username == world->owner)
                    {
                        pinfo(currentPeer)->ban = time(NULL);
                        int days = 3 * 24 * 60 * 60;
                        pinfo(currentPeer)->bantime = days;
                        Packets::onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `4BANNED `0from Private Server for 3 days.");
                        enet_peer_disconnect_later(currentPeer, 0);
                    }
                    else
                    {
                        std::istream *blobdata = PLAYER_DATA(world->owner);
                        if (blobdata != NULL)
                        {
                            player *ply;
                            {
                                boost::archive::binary_iarchive ia(*blobdata);
                                ia >> ply;
                            }
                            ply->ban = time(NULL);
                            int days = 3 * 24 * 60 * 60;
                            ply->bantime = days;
                            UPDATE(serialize_player(ply), world->owner);
                        }
                    }
                }
                return;
            }
            for (int i = 0; i < static_cast<int>(world->access.size()); i++)
            {
                ENetPeer *currentPeer;
                for (currentPeer = server->peers;
                     currentPeer < &server->peers[server->peerCount];
                     ++currentPeer)
                {
                    if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                        continue;
                    if (i == 1)
                    {
                        Packets::consoleMessage(currentPeer, "`#** `$The Ancients `ohave used `4Ban `oon `w" + world->owner + "`o! `#**");
                        sendmusic(currentPeer, "beep");
                        sendmusic(currentPeer, "bigboom");
                        Packets::consoleMessage(currentPeer, "`o>>`4" + world->name + " `4was nuked from orbit`o. It's the only way to be sure. Play nice, everybody!");
                        if (pinfo(currentPeer)->currentWorld == pinfo(peer)->currentWorld)
                        {
                            sendPlayerLeave(server, currentPeer, pinfo(currentPeer));
                            pinfo(currentPeer)->currentWorld = "EXIT";
                            SendWorldOffers(currentPeer);
                        }
                        if (pinfo(currentPeer)->username == world->owner)
                        {
                            pinfo(currentPeer)->ban = time(NULL);
                            int days = 3 * 24 * 60 * 60;
                            pinfo(currentPeer)->bantime = days;
                            Packets::onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `4BANNED `0from Private Server for 3 days.");
                            enet_peer_disconnect_later(currentPeer, 0);
                        }
                        else
                        {
                            std::istream *blobdata = PLAYER_DATA(world->owner);
                            if (blobdata != NULL)
                            {
                                player *ply;
                                {
                                    boost::archive::binary_iarchive ia(*blobdata);
                                    ia >> ply;
                                }
                                ply->ban = time(NULL);
                                int days = 3 * 24 * 60 * 60;
                                ply->bantime = days;
                                UPDATE(serialize_player(ply), world->owner);
                            }
                        }
                    }
                    Packets::consoleMessage(currentPeer, "`#** `$The Ancients `ohave used `bCurse `oon `w" + world->access[i] + "`o! `#**");
                    sendmusic(currentPeer, "beep");
                    if (pinfo(currentPeer)->username == world->access[i])
                    {
                        pinfo(currentPeer)->curse = time(NULL);
                        int hours = 5 * 60 * 60;
                        pinfo(currentPeer)->cursetime = hours;
                        Packets::onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `bCURSED `0from Private Server for 5 Hours.");
                        sendPlayerLeave(server, currentPeer, pinfo(currentPeer));
                        joinWorld(server, currentPeer, "HELL", 0, 0);
                    }
                    else
                    {
                        std::istream *blobdata = PLAYER_DATA(world->access[i]);
                        if (blobdata != NULL)
                        {
                            player *ply;
                            {
                                boost::archive::binary_iarchive ia(*blobdata);
                                ia >> ply;
                            }
                            ply->curse = time(NULL);
                            int hours = 5 * 60 * 60;
                            ply->cursetime = hours;
                            UPDATE(serialize_player(ply), world->access[i]);
                        }
                    }
                }
            }
        }
    }
    if (str.length() && str[0] == '/')
    {
    }
    else
    {
        str = trimString(str);
        ENetPeer *currentPeer;
        for (currentPeer = server->peers;
             currentPeer < &server->peers[server->peerCount];
             ++currentPeer)
        {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                continue;
            if (pinfo(peer)->currentWorld == pinfo(currentPeer)->currentWorld)
            {
                Packets::consoleMessage(currentPeer, "CP:0_PL:4_OID:_CT:[W]_ `o<`w" + pinfo(peer)->name + "`o> " + str);
                Packets::ontalkbubble(currentPeer, pinfo(peer)->netID, str);
            }
        }
    }
}