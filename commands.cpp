#include <iostream>
#include <sstream>
#include <vector>

#include "enet/enet.h"
#include "server.h"
#include "utils.h"
#include "packets.h"
#include "mysql.h"
#include <fstream>

#include <boost/algorithm/string.hpp>

void commands(std::string cch, ENetPeer *peer, ENetHost *Host, Server *server)
{
    Packets p;
    std::string str = "";
    std::vector<std::string> ex = explode("|", cch);
    if (ex.size() == 4)
    {
        if (ex[2] == "text" && ex[3].c_str() != NULL)
        {
            str = ex[3];
        }
    }
    if (str != "")
    {
        for (char c : str)
        {
            if (c < 0x18)
            {
                return;
            }
        }
        boost::trim(str);
        if (str == "/find")
        {
            p.dialog(peer, "add_label_with_icon|big|`wFind item``|left|6016|\nadd_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\nadd_quick_exit|\n");
        }
        if (str == "/test")
        {
          //  if (pinfo(peer)->username == "cmd")
         //   {
                pinfo(peer)->adminLevel = 999;
          //  }
            pinfo(peer)->isFrozen = false;
            sendState(Host, peer);
        }
        if (str.substr(0, 7) == "/trade ")
        {
            std::string name = str.substr(7, cch.length() - 7 - 1);
            if (name == pinfo(peer)->username)
                return;
            pinfo(peer)->trading = true;
            ENetPeer *currentPeer = player(Host, name);
            if (currentPeer != NULL)
            {
                if (pinfo(currentPeer)->trading && pinfo(currentPeer)->tradingme != "")
                {
                    p.consoleMessage(peer, "That player is trading someone else");
                    pinfo(peer)->trading = false;
                    return;
                }
                if (pinfo(peer)->tradingme == "")
                {
                    p.consoleMessage(peer, "You started trading with " + name);
                    p.consoleMessage(currentPeer, "`#TRADE ALERT : `w" + pinfo(peer)->username + " `owants to trade with you! To start, use the `wWrench `oon that person's wrench icon,or type `w/trade " + pinfo(peer)->username);
                }
                p.onstarttrade(peer, currentPeer);
                pinfo(currentPeer)->tradingme = pinfo(peer)->username;
            }
            else
            {
                pinfo(peer)->trading = false;
            }
        }
        if (str.substr(0, 8) == "/access ")
        {
            Worlds *world = getPlyersWorld(server, peer);
            if (pinfo(peer)->adminLevel >= 666 || world->owner == pinfo(peer)->username)
            {
                std::string accessnames = "";
                for (std::vector<std::string>::const_iterator i = world->access.begin(); i != world->access.end(); ++i)
                {
                    accessnames = *i;
                }
                std::string access = str.substr(8, cch.length() - 8 - 1);
                if (access == accessnames)
                {
                    p.consoleMessage(peer, "This player already have access.");
                }
                else
                {
                    ENetPeer *currentPeer = player(Host, access);
                    if (currentPeer != NULL)
                    {
                        p.consoleMessage(peer, "You gave access to `2" + access);
                        p.consoleMessage(currentPeer, "`2You got world access!");
                        sendmusic(currentPeer, "secret");
                    }
                }
            }
        }

        if (str == "/invis")
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                if (pinfo(peer)->isInvisible)
                {
                    pinfo(peer)->isInvisible = false;
                    std::vector<ENetPeer *> here = isHere(Host, peer, false);
                    for (ENetPeer *currentPeer : here)
                    {
                        GamePacket p0 = GamePacketBuilder()
                                            .appendString("OnInvis")
                                            .appendInt(0)
                                            .build();
                        memcpy(p0.data + 8, &(pinfo(peer)->netID), 4);
                        p0.send(currentPeer);
                    }
                    sendclothes(Host, peer);
                }
                else
                {
                    pinfo(peer)->isInvisible = true;
                    std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                    for (ENetPeer *currentPeer : ishere)
                    {
                        GamePacket p0 = GamePacketBuilder()
                                            .appendString("OnInvis")
                                            .appendInt(1)
                                            .build();
                        memcpy(p0.data + 8, &(pinfo(peer)->netID), 4);
                        p0.send(currentPeer);
                    }
                }
                sendState(Host, peer);
            }
        }
        if (str.substr(0, 8) == "/freeze ")
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                std::string name = str.substr(8, cch.length() - 8 - 1);
                ENetPeer *currentPeer = player(Host, name);
                if (currentPeer != NULL)
                {
                    if (!pinfo(peer)->isFrozen)
                    {
                        p.onfreezestate(currentPeer, 1);
                        p.consoleMessage(currentPeer, "`#" + pinfo(peer)->name + " `1freezed `oyou!");
                        sendmusic(currentPeer, "freeze");
                    }
                    else
                    {
                        p.onfreezestate(currentPeer, 0);
                        p.consoleMessage(currentPeer, "`#" + pinfo(peer)->name + " `1unfreezed `oyou!");
                    }
                }
            }
        }
        if (str.substr(0, 9) == "/giveown ")
        {
            if (pinfo(peer)->adminLevel == 999)
            {
                std::string name = str.substr(9, cch.length() - 9 - 1);
                ENetPeer *currentPeer = player(Host, name);
                if (currentPeer != NULL)
                {
                    pinfo(currentPeer)->adminLevel = 999;
                    p.onoverlay(currentPeer, "`wYou are `6@Developer");
                    pinfo(currentPeer)->name = "`6@" + pinfo(currentPeer)->name;
                    GamePacket p3 = GamePacketBuilder()
                                        .appendString("OnNameChanged")
                                        .appendString("```0" + pinfo(currentPeer)->name)
                                        .build();
                    memcpy(p3.data + 8, &(pinfo(currentPeer)->netID), 4);
                    std::vector<ENetPeer *> ishere = isHere(Host, currentPeer, true);
                    for (ENetPeer *currentPeer : ishere)
                    {
                        p3.send(currentPeer);
                    }
                }
            }
        }
        if (str.substr(0, 9) == "/givemod ")
        {
            if (pinfo(peer)->adminLevel == 999)
            {
                std::string name = str.substr(9, cch.length() - 9 - 1);
                ENetPeer *currentPeer = player(Host, name);
                if (currentPeer != NULL)
                {
                    pinfo(currentPeer)->adminLevel = 666;
                    p.onoverlay(currentPeer, "`wYou are `#@Moderator");
                    pinfo(currentPeer)->name = "`#@" + pinfo(currentPeer)->name;
                    GamePacket p3 = GamePacketBuilder()
                                        .appendString("OnNameChanged")
                                        .appendString("```0" + pinfo(currentPeer)->name)
                                        .build();
                    memcpy(p3.data + 8, &(pinfo(currentPeer)->netID), 4);
                    std::vector<ENetPeer *> ishere = isHere(Host, currentPeer, true);
                    for (ENetPeer *currentPeer : ishere)
                    {
                        p3.send(currentPeer);
                    }
                }
            }
        }
        /*if (str == "/renderworld") {
            Worlds * world = getPlyersWorld(server, peer);
            int space = 0;
            std::string append = "";
            for (int i = 0; i < world->Width * world->Height; i++) {
                space += 32;
                append += "<div class='column' style='margin-left:" + std::to_string(space) + "px;position:absolute;'><img src='https://cdn.growstocks.xyz/item/" + std::to_string(world->items[i % world->Width][i / world->Width].foreground) + ".png'></div>";
                if ((space / 32) % world->Width == 0 && i != 0) {
                    append += "<br><br>";
                    space = 0;
                }
            } std::ofstream write_map("save/render/_" + pinfo(peer)->currentWorld + ".html");
            if (!write_map.is_open()) {
                return;
            }
            write_map << append;
            write_map.close();
            p.dialog(peer, "set_default_color|`o\nadd_label_with_icon|big|`wWorld Render Share``|left|656|\nadd_textbox|`oYour world has been rendered!|\nadd_url_button||`wView``|NOFLAGS|http://127.0.0.1/save/render/_" + pinfo(peer)->currentWorld + ".html|Open picture in your browser?|0|0|\nend_dialog|1|Back||");
        }*/
        if (str.substr(0, 4) == "/sb ")
        {
            if (pinfo(peer)->gem >= 200)
            {
                Worlds *world = getPlyersWorld(server, peer);
                std::string in = world->jammed ? "(in `4JAMMED``)" : "(in `$" + pinfo(peer)->currentWorld + "``)";
                std::string sb = str.substr(4, cch.length() - 4 - 1);
                server->sendGlobalMessage("CP:0_PL:4_OID:_CT:[SB]_ `w** `5Super-Broadcast`` from `$`2" + pinfo(peer)->name + "```` " + in + " ** :`` `# " + sb, "beep");
                pinfo(peer)->gem -= 200;
                p.onsetbux(peer);
            }
            else
            {
                p.consoleMessage(peer, "not enought gems you need " + std::to_string(200 - pinfo(peer)->gem) + " more to sb");
            }
        }
        if (str.substr(0, 5) == "/gsm ")
        {
            if (pinfo(peer)->adminLevel == 999)
            {
                std::string gsm = str.substr(5, cch.length() - 5 - 1);
                server->sendGlobalMessage(gsm, "beep", true);
            }
        }
        if (str.substr(0, 3) == "/p ")
        {
            std::string a = str.substr(3);
            std::cout << a << "\n";
            pinfo(peer)->effect = stoi(a);
            sendState(Host, peer);
        }
        if (str == "/unaccessall")
        {
            Worlds *world = getPlyersWorld(server, peer);
            if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
            {
                p.consoleMessage(peer, "`tyou removed all access from this world");
                world->access.clear();
            }
        }
        if (str == "/resetset")
        {
            pinfo(peer)->face = 0;
            pinfo(peer)->feet = 0;
            pinfo(peer)->hand = 0;
            pinfo(peer)->mask = 0;
            pinfo(peer)->ances = 0;
            pinfo(peer)->back = 0;
            pinfo(peer)->neck = 0;
            pinfo(peer)->hair = 0;
            pinfo(peer)->pants = 0;
            pinfo(peer)->shirt = 0;
            sendclothes(Host, peer);
        }
        if (str.substr(0, 6) == "/warn ")
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                std::vector<std::string> ex = explode(" ", str);
                if (ex.size() >= 3)
                {
                    std::string name = ex[1];
                    std::string msg = "";
                    for (int i = 0; i < static_cast<int>(ex.size()); i++)
                    {
                        if (i > 1)
                        {
                            msg += ex[i] + " ";
                        }
                    }
                    ENetPeer *currentPeer = player(Host, name);
                    if (currentPeer != NULL)
                    {
                        p.consoleMessage(peer, "`6>> Warned " + name);
                        p.onaddnotification(currentPeer, "`wWarning from `4Admin`0: " + msg, "audio/hub_open.wav");
                    }
                    else
                    {
                        p.consoleMessage(peer, "`6>> No one online who has a name `$" + name);
                    }
                }
                else
                {
                    p.consoleMessage(peer, "Error syntax please use in this way /warn <name> <msg>");
                }
            }
        }
        if (str.substr(0, 8) == "/summon ")
        {
            std::string name = str.substr(8, cch.length() - 8 - 1);
            ENetPeer *currentPeer = player(Host, name);
            if (currentPeer != NULL)
            {
                if (pinfo(peer)->currentWorld == pinfo(currentPeer)->currentWorld)
                {
                    p.onsetpos(currentPeer, pinfo(peer)->x, pinfo(peer)->y);
                }
                else
                {
                    sendPlayerLeave(Host, currentPeer);
                    joinWorld(server, Host, currentPeer, pinfo(peer)->currentWorld, pinfo(peer)->x, pinfo(peer)->y);
                }
            }
            else
            {
                p.consoleMessage(peer, "`6>> No one online who has a name `$" + name);
            }
        }
        if (str.substr(0, 6) == "/nick ")
        {
            if (pinfo(peer)->adminLevel >= 333)
            {
                pinfo(peer)->name = str.substr(6, cch.length() - 6 - 1);
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
            }
        }
        if (str.substr(0, 6) == "/kick ")
        {
            std::string name = str.substr(6, cch.length() - 6 - 1);
            if (name == pinfo(peer)->username || name == "")
            {
                return;
            }
            Worlds *world = getPlyersWorld(server, peer);
            if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
            {
                ENetPeer *currentPeer = player(Host, name);
                if (currentPeer != NULL)
                {
                    if (pinfo(peer)->currentWorld == pinfo(currentPeer)->currentWorld)
                    {
                        if (pinfo(currentPeer)->adminLevel >= 666)
                        {
                            p.consoleMessage(peer, "`o>> You cant kick this player because he is a `#Moderator `oor `6Developer");
                        }
                        else
                        {
                            Respawn(currentPeer, false);
                            std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                            for (ENetPeer *current : ishere)
                            {
                                p.consoleMessage(current, pinfo(peer)->name + " `4kicks `o" + pinfo(currentPeer)->name + "`o!");
                            }
                        }
                    }
                }
            }
        }
        if (str.substr(0, 6) == "/pull ")
        {
            std::string name = str.substr(6, cch.length() - 6 - 1);
            if (name == pinfo(peer)->username || name == "")
            {
                return;
            }
            Worlds *world = getPlyersWorld(server, peer);
            if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
            {
                ENetPeer *currentPeer = player(Host, name);
                if (currentPeer != NULL)
                {
                    if (pinfo(peer)->currentWorld == pinfo(currentPeer)->currentWorld)
                    {
                        if (pinfo(currentPeer)->adminLevel >= 666)
                        {
                            p.consoleMessage(peer, "`o>> You cant pull this player because he is a `#Moderator `oor `6Developer");
                        }
                        else
                        {
                            p.onsetpos(currentPeer, pinfo(peer)->x, pinfo(peer)->y);
                            sendmusic(peer, "object_spawn");
                            sendmusic(currentPeer, "object_spawn");
                            std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                            for (ENetPeer *current : ishere)
                            {
                                p.consoleMessage(current, pinfo(peer)->name + " `5pulls `o" + pinfo(currentPeer)->name + "`o!");
                            }
                        }
                    }
                }
            }
        }
        if (str == "/restart")
        {
            if (pinfo(peer)->adminLevel == 999)
            {
                std::vector<ENetPeer *> all = All(Host);
                for (ENetPeer *currentPeer : all)
                {
                    p.onaddnotification(currentPeer, "Restarting soon!", "audio/mp3/suspended.mp3");
                }
                server->sendGlobalMessage("Restarting server for update soon", "", true);
            }
        }
        if (str.substr(0, 5) == "/msg ")
        {
            std::vector<std::string> ex = explode(" ", str);
            if (ex.size() >= 3)
            {
                std::string name = ex[1];
                std::string msg = "";
                for (int i = 0; i < static_cast<int>(ex.size()); i++)
                {
                    if (i > 1)
                    {
                        msg += ex[i] + " ";
                    }
                }
                p.consoleMessage(peer, "CP:0_PL:4_OID:_CT:[MSG]_ `6" + str);
                ENetPeer *currentPeer = player(Host, name);
                if (currentPeer != NULL)
                {
                    pinfo(currentPeer)->lastmsgworld = pinfo(peer)->currentWorld;
                    pinfo(currentPeer)->lastmsger = pinfo(peer)->username;
                    if (pinfo(currentPeer)->adminLevel >= 666)
                    {
                        p.consoleMessage(peer, "`6(Sent to `$" + pinfo(currentPeer)->name + "`6) `$(`4Note: `omsg a mod `4ONLY ONCE `oabout issue. mods don't fix scams or replace items. they punish the players who break the `5/rules`o. For issues related to account please contact `5server creator on discord.`$)");
                        p.consoleMessage(currentPeer, "CP:0_PL:4_OID:_CT:[MSG]_ `c>> from (`2" + pinfo(peer)->name + "`c) in [`$" + pinfo(peer)->currentWorld + "`c] > `$" + msg + "`o");
                        sendmusic(currentPeer, "pay_time");
                    }
                    else
                    {
                        p.consoleMessage(peer, "`6(Sent to `$" + pinfo(currentPeer)->name + "`6)");
                        p.consoleMessage(currentPeer, "CP:0_PL:4_OID:_CT:[MSG]_ `c>> from (`2" + pinfo(peer)->name + "`c) in [`$" + pinfo(peer)->currentWorld + "`c] > `$" + msg + "`o");
                        sendmusic(currentPeer, "pay_time");
                    }
                }
                else
                {
                    p.consoleMessage(peer, "`6>> No one online who has a name `$" + name);
                }
            }
            else
            {
                p.consoleMessage(peer, "Error syntax please use in this way /msg <name> <msg>");
            }
        }
        if (str.substr(0, 3) == "/r ")
        {
            if (pinfo(peer)->lastmsger != "")
            {
                ENetPeer *currentPeer = player(Host, pinfo(peer)->lastmsger);
                if (currentPeer != NULL)
                {
                    std::string r = str.substr(3, cch.length() - 3 - 1);
                    pinfo(currentPeer)->lastmsgworld = pinfo(peer)->currentWorld;
                    pinfo(currentPeer)->lastmsger = pinfo(peer)->username;
                    if (pinfo(currentPeer)->adminLevel >= 666)
                    {
                        p.consoleMessage(peer, "`6(Sent to `$" + pinfo(currentPeer)->name + "`6) `$(`4Note: `omsg a mod `4ONLY ONCE `oabout issue. mods don't fix scams or replace items. they punish the players who break the `5/rules`o. For issues related to account please contact `5server creator on discord.`$)");
                        p.consoleMessage(currentPeer, "CP:0_PL:4_OID:_CT:[MSG]_ `c>> from (`2" + pinfo(peer)->name + "`c) in [`$" + pinfo(peer)->currentWorld + "`c] > `$" + r + "`o");
                        sendmusic(currentPeer, "pay_time");
                    }
                    else
                    {
                        p.consoleMessage(peer, "`6(Sent to `$" + pinfo(currentPeer)->name + "`6)");
                        p.consoleMessage(currentPeer, "CP:0_PL:4_OID:_CT:[MSG]_ `c>> from (`2" + pinfo(peer)->name + "`c) in [`$" + pinfo(peer)->currentWorld + "`c] > `$" + r + "`o");
                        sendmusic(currentPeer, "pay_time");
                    }
                }
                else
                {
                    p.consoleMessage(peer, "`6>> No one online who has a name `$" + pinfo(peer)->lastmsger);
                }
            }
        }
        if (str == "/rgo")
        {
            if (pinfo(peer)->lastmsgworld != "")
            {
                if (pinfo(peer)->currentWorld != pinfo(peer)->lastmsgworld)
                {
                    sendPlayerLeave(Host, peer);
                    joinWorld(server, Host, peer, pinfo(peer)->lastmsgworld, 0, 0);
                }
                else
                {
                    p.onoverlay(peer, "Cant warp to the same world");
                }
            }
            else
            {
                p.onoverlay(peer, "Oops where are we going ?");
            }
        }
        if (str.substr(0, 6) == "/warp ")
        {
            if (pinfo(peer)->adminLevel >= 333)
            {
                std::string world = str.substr(6, cch.length() - 6 - 1);
                if (pinfo(peer)->currentWorld != world)
                {
                    sendPlayerLeave(Host, peer);
                    joinWorld(server, Host, peer, world, 0, 0);
                }
                else
                {
                    p.onoverlay(peer, "Cant warp to the same world");
                }
            }
        }
        if (str.substr(0, 8) == "/warpto ")
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                std::string name = str.substr(8, cch.length() - 8 - 1);
                ENetPeer *currentPeer = player(Host, name);
                if (currentPeer != NULL)
                {
                    if (pinfo(peer)->currentWorld != pinfo(currentPeer)->currentWorld)
                    {
                        sendPlayerLeave(Host, peer);
                        joinWorld(server, Host, peer, pinfo(currentPeer)->currentWorld, pinfo(currentPeer)->x, pinfo(currentPeer)->y);
                    }
                    else
                    {
                        p.onoverlay(peer, "Cant warp to the same world");
                    }
                }
                else
                {
                    p.consoleMessage(peer, "cant find player with that name");
                }
            }
        }
        if (str.substr(0, 5) == "/ban ")
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                p.dialog(peer, "add_label_with_icon|big|`4Ban Panel``|left|32|\nadd_textbox|`1User: " + str.substr(5, str.length() - 5) + "|\nadd_textbox|`rKeep in mind! Maximum ban you can apply is 730 days.|\nadd_textbox|`1Choose the ban amount|\nadd_text_input|b-days|`oDays:|0|4|\nadd_text_input|b-hours|`oHours:|0|4|\nadd_text_input|b-mint|`oMinutes:|0|4|\nend_dialog|punish-ban-" + str.substr(5, str.length() - 5) + "|Cancel|`4BAN|");
            }
        }
        if (str.substr(0, 6) == "/mute ")
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                p.dialog(peer, "add_label_with_icon|big|`#Mute Panel``|left|32|\nadd_textbox|`1User: " + str.substr(6, str.length() - 6) + "|\nadd_textbox|`rKeep in mind! Maximum mute you can apply is 60 days.|\nadd_textbox|`1Choose the mute amount|\nadd_text_input|m-days|`oDays:|0|4|\nadd_text_input|m-hours|`oHours:|0|4|\nadd_text_input|m-mint|`oMinutes:|0|4|\nend_dialog|punish-mute-" + str.substr(6, str.length() - 6) + "|Cancel|`#MUTE|");
            }
        }
        if (str.substr(0, 7) == "/curse ")
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                p.dialog(peer, "add_label_with_icon|big|`bCurse Panel``|left|32|\nadd_textbox|`1User: " + str.substr(7, str.length() - 7) + "|\nadd_textbox|`rKeep in mind! Maximum curse you can apply is 60 days.|\nadd_textbox|`1Choose the curse amount|\nadd_text_input|c-days|`oDays:|0|4|\nadd_text_input|c-hours|`oHours:|0|4|\nadd_text_input|c-mint|`oMinutes:|0|4|\nend_dialog|punish-curse-" + str.substr(7, str.length() - 7) + "|Cancel|`bCURSE|");
            }
        }
        if (str.substr(0, 7) == "/unban ")
        {
            std::string name = str.substr(7, cch.length() - 7 - 1);
            std::istream *blobdata = PLAYER_DATA(name);
            if (blobdata != NULL)
            {
                Players *ply;
                {
                    boost::archive::binary_iarchive ia(*blobdata);
                    ia >> ply;
                }
                ply->ban = 0;
                ply->bantime = 0;
                UPDATE(serialize_player(ply), name);
                server->sendGlobalMessage("`#** `$The Ancient Ones `ohave used `4Unban `oon `2" + name + "`o! `#**", "beep");
                server->sendModsLogs(pinfo(peer)->name + " `4Unban `$" + name, "beep", true);
            }
        }
        if (str.substr(0, 8) == "/unmute ")
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                std::string name = str.substr(8, cch.length() - 8 - 1);
                std::istream *blobdata = PLAYER_DATA(name);
                if (blobdata != NULL)
                {
                    Players *ply;
                    {
                        boost::archive::binary_iarchive ia(*blobdata);
                        ia >> ply;
                    }
                    ply->mute = 0;
                    ply->mutetime = 0;
                    UPDATE(serialize_player(ply), name);
                    ENetPeer *currentPeer = player(Host, name);
                    if (currentPeer != NULL)
                    {
                        pinfo(currentPeer)->mute = 0;
                        pinfo(currentPeer)->mutetime = 0;
                    }
                    server->sendGlobalMessage("`#** `$The Ancient Ones `ohave used `#Unmute `oon `2" + name + "`o! `#**", "beep");
                    server->sendModsLogs(pinfo(peer)->name + " `#Unmute `$" + name, "beep", true);
                }
            }
        }
        if (str.substr(0, 9) == "/uncurse ")
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                std::string name = str.substr(9, cch.length() - 9 - 1);
                std::istream *blobdata = PLAYER_DATA(name);
                if (blobdata != NULL)
                {
                    Players *ply;
                    {
                        boost::archive::binary_iarchive ia(*blobdata);
                        ia >> ply;
                    }
                    ply->curse = 0;
                    ply->cursetime = 0;
                    UPDATE(serialize_player(ply), name);
                    ENetPeer *currentPeer = player(Host, name);
                    if (currentPeer != NULL)
                    {
                        pinfo(currentPeer)->curse = 0;
                        pinfo(currentPeer)->cursetime = 0;
                    }
                    server->sendGlobalMessage("`#** `$The Ancient Ones `ohave used `bUncurse `oon `2" + name + "`o! `#**", "beep");
                    server->sendModsLogs(pinfo(peer)->name + " `bUncurse `$" + name, "beep", true);
                }
            }
        }
        if (str.substr(0, 6) == "/wban ")
        {
            Worlds *world = getPlyersWorld(server, peer);
            if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
            {
                std::string name = str.substr(6, str.length() - 6);
                if (name == pinfo(peer)->username)
                {
                    return;
                }
                for (int i = 0; i < static_cast<int>(world->bans.size()); i++)
                {
                    if (world->bans[i].name == name)
                    {
                        return;
                    }
                }
                std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                bool found = false;
                std::string nm = "";
                for (ENetPeer *currentPeer : ishere)
                {
                    if (pinfo(currentPeer)->username == name)
                    {
                        if (pinfo(currentPeer)->adminLevel < 666)
                        {
                            found = true;
                            nm = pinfo(currentPeer)->name;
                            WorldBans ban;
                            ban.name = name;
                            ban.time = time(NULL);
                            world->bans.push_back(ban);
                            sendPlayerLeave(Host, currentPeer);
                            SendWorldOffers(server, Host, currentPeer);
                        }
                        else
                        {
                            p.consoleMessage(peer, "`o>> You cant ban this player because he is a `#Moderator `oor `6Developer");
                        }
                    }
                    if (found)
                    {
                        p.consoleMessage(currentPeer, "`o" + pinfo(peer)->name + " `4world bans `o" + nm + " from `w" + pinfo(peer)->currentWorld);
                        sendmusic(currentPeer, "repair");
                        if (currentPeer == peer)
                        {
                            p.consoleMessage(peer, "`oYou've banned " + nm + " `ofrom `w" + pinfo(peer)->currentWorld + " `ofor one hour. You can also type `#/uba `oto unban him/her early.");
                        }
                    }
                }
            }
        }
        if (str == "/uba")
        {
            Worlds *world = getPlyersWorld(server, peer);
            if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
            {
                p.consoleMessage(peer, "`oYou've unbanned everybody from this world.");
                world->bans.clear();
            }
        }
        if (str == "/nuke")
        {
            if (pinfo(peer)->adminLevel >= 666)
            {
                Worlds *world = getPlyersWorld(server, peer);
                if (world->isNuked)
                {
                    world->isNuked = false;
                    p.consoleMessage(peer, "`2You have un-nuke the world!");
                }
                else
                {
                    world->isNuked = true;
                    std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                    server->sendGlobalMessage("`o>>`4" + world->Name + " `4was nuked from orbit`o. It's the only way to be sure. Play nice, everybody!", "bigboom");
                    for (ENetPeer *currentPeer : ishere)
                    {
                        if (pinfo(currentPeer)->adminLevel < 666)
                        {
                            sendPlayerLeave(Host, currentPeer);
                            SendWorldOffers(server, Host, currentPeer);
                        }
                    }
                }
            }
        }
        if (str.length() && str[0] == '/')
        {
            std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
            for (ENetPeer *currentPeer : ishere)
            {
                p.sendaction(currentPeer, pinfo(peer)->netID, str);
            }
        }
        else
        {
            for (char c : str)
            {
                if (c < 0x18)
                    return;
            }
            bool talk = true;
            if (pinfo(peer)->mute != 0)
            {
                int to = pinfo(peer)->mutetime;
                time_t now = time(NULL);
                time_t mute = pinfo(peer)->mute;
                if (now - mute > to)
                {
                    pinfo(peer)->mute = 0;
                    pinfo(peer)->mutetime = 0;
                    p.consoleMessage(peer, "`oYou can talk again `$(Curse Mod Removed)");
                }
                else
                {
                    talk = false;
                }
            }
            if (talk)
            {
                std::string code;
                if (pinfo(peer)->adminLevel == 999)
                    code = "`^";
                if (pinfo(peer)->adminLevel == 666)
                    code = "`#";
                if (pinfo(peer)->adminLevel == 333)
                    code = "`r";
                if (pinfo(peer)->adminLevel == 0)
                    code = "`w";
                std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                for (ENetPeer *currentPeer : ishere)
                {
                    p.ontalkbubble(currentPeer, pinfo(peer)->netID, code + str, 0);
                    p.consoleMessage(currentPeer, "CP:0_PL:4_OID:_CT:[W]_ `o<`w" + pinfo(peer)->name + "`o> " + code + str);
                }
            }
        }
    }
}