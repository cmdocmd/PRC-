#include <iostream>
#include <sstream>
#include "enet/enet.h"
#include "server.h"
#include "utils.h"
#include "players.h"
#include "packets.h"
#include "mysql.h"
#include "items.h"

void dialog(Server *server, std::string cch, ENetPeer *peer, ENetHost *Host)
{
    Packets p;
    pinfo(peer)->usingDialog = false;
    std::stringstream ss(cch);
    std::string to;
    std::string username = "", password = "", email = "", itemFind = "", btn = "", g_name = "", g_state = "", d_count = "", s_text = "", b_days = "", b_hours = "", b_mint = "", m_days = "", m_hours = "", m_mint = "", c_days = "", c_hours = "", c_mint = "", p_name = "", dest = "", label = "", doorid = "", iop = "", itemcount = "", pub = "", disable_music = "", tempo = "", netid = "", newname = "", newworld = "", safepass = "", chnpass = "", ppass = "", mgitem = "", ismagactive = "";
    ;
    bool isRegister = false, isFindDialog = false, isGuildDialog = false, isDropDialog = false, isSign = false, isBan = false, isMute = false, isCurse = false, isDoor = false, istradedialog = false, isEditLock = false, Accesspicker = false, iscNameDialog = false, iscWorldDialog = false, isaddpss = false, ispdoor = false, ismgplant = false, isupdatemag = false;

    while (std::getline(ss, to, '\n'))
    {
        std::vector<std::string> ex = explode("|", to);
        if (ex.size() == 2)
        {

            if (ex[0] == "showlocation" && ex[1] == "0")
                pinfo(peer)->warpme = 0;
            if (ex[0] == "showlocation" && ex[1] == "1")
                pinfo(peer)->warpme = 1;

            if (ex[0] == "isworldpublic" && ex[1] == "0")
                getPlyersWorld(server, peer)->isPublic = false;
            if (ex[0] == "isworldpublic" && ex[1] == "1")
                getPlyersWorld(server, peer)->isPublic = true;

            if (ex[0].substr(0, 10) == "isAccessed" && ex[1] == "0")
            {
                Worlds *world = getPlyersWorld(server, peer);
                std::string unaccessname = ex[0].substr(10, ex[0].length() - 10);
                p.consoleMessage(peer, "`oYou removed " + unaccessname + " from accesslist.");
                ENetPeer *currentPeer = player(Host, unaccessname);
                if (currentPeer != NULL)
                {
                    p.consoleMessage(currentPeer, "`oYou lost your access in (`4" + getPlyersWorld(server, peer)->Name + "`o)");
                }
                world->access.erase(std::remove(world->access.begin(), world->access.end(), unaccessname), world->access.end());
            }

            if (ex[0] == "ishpublic")
            {
                Worlds *world = getPlyersWorld(server, peer);
                int x = pinfo(peer)->wrenchx;
                int y = pinfo(peer)->wrenchy;
                int fg = world->items[x][y].foreground;
                int bg = world->items[x][y].background;
                int idx = -1;
                for (int i = 0; i < static_cast<int>(world->entrance.size()); i++)
                {
                    if (world->entrance[i].x == x && world->entrance[i].y == y)
                        idx = i;
                }
                if (idx != -1)
                {
                    if (ex[1] == "1")
                    {
                        world->entrance[idx].is_open = "1";
                        std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                        for (ENetPeer *currentPeer : ishere)
                        {
                            updateEntrance(currentPeer, fg, x, y, true, bg);
                        }
                    }
                    if (ex[1] == "0")
                    {
                        world->entrance[idx].is_open = "0";
                        std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                        for (ENetPeer *currentPeer : ishere)
                        {
                            if (pinfo(currentPeer)->userID == world->id || pinfo(currentPeer)->adminLevel >= 666)
                            {
                                updateEntrance(currentPeer, fg, x, y, true, bg);
                            }
                            else
                            {
                                updateEntrance(currentPeer, fg, x, y, false, bg);
                            }
                        }
                    }
                }
            }

            if (ex[0] == "buttonClicked")
            {
                btn = ex[1];
            }
            if (ex[0] == "dialog_name")
            {
                std::string dialog = ex[1];
                if (dialog == "register")
                {
                    isRegister = true;
                }
                if (dialog == "tradedialog")
                {
                    istradedialog = true;
                }
                if (dialog == "tradedialog")
                {
                    istradedialog = true;
                }
                if (dialog == "findid")
                {
                    isFindDialog = true;
                }
                if (dialog == "guild")
                {
                    isGuildDialog = true;
                }
                if (dialog == "dropdialog")
                {
                    isDropDialog = true;
                }
                if (dialog == "signok")
                {
                    isSign = true;
                }
                if (dialog == "editdoor")
                {
                    isDoor = true;
                }
                if (dialog == "spss")
                {
                    isaddpss = true;
                }
                if (dialog == "editlock")
                {
                    isEditLock = true;
                }
                if (dialog == "cnamedialog")
                {
                    iscNameDialog = true;
                }
                if (dialog == "cworlddialog")
                {
                    iscWorldDialog = true;
                }
                if (dialog == "pdoor")
                {
                    ispdoor = true;
                }
                if (dialog == "mgplant")
                {
                    ismgplant = true;
                }
                if (dialog == "updatemag")
                {
                    isupdatemag = true;
                }
                if (dialog == "displayblock")
                {
                    Worlds *world = getPlyersWorld(server, peer);
                    int x = pinfo(peer)->wrenchx;
                    int y = pinfo(peer)->wrenchy;
                    int fg = world->items[x][y].foreground;
                    int bg = world->items[x][y].background;
                    int idx = -1;
                    for (int i = 0; i < world->dblock.size(); i++)
                    {
                        if (world->dblock[i].x == x && world->dblock[i].y == y)
                            idx = i;
                    }
                    if (idx != -1)
                    {
                        int id = world->dblock[idx].id;
                        if (!MoreThan200(peer, id, 1))
                        {
                            Add(peer, id, 1, true);
                            sendclothes(Host, peer);
                            world->dblock.erase(world->dblock.begin() + idx);
                            updatedblock(peer, fg, bg, x, y, 0);
                        }
                        else
                        {
                            p.ontalkbubble(peer, pinfo(peer)->netID, "`wYou will have more than 200!", 1);
                        }
                    }
                }
                if (dialog.substr(0, 7) == "punish-")
                {
                    std::vector<std::string> ex = explode("-", dialog);
                    if (ex[1] == "ban")
                    {
                        isBan = true;
                    }
                    if (ex[1] == "mute")
                    {
                        isMute = true;
                    }
                    if (ex[1] == "curse")
                    {
                        isCurse = true;
                    }
                    p_name = ex[2];
                }
            }
            if (isBan)
            {
                if (ex[0] == "b-days")
                {
                    b_days = ex[1];
                }
                if (ex[0] == "b-hours")
                {
                    b_hours = ex[1];
                }
                if (ex[0] == "b-mint")
                {
                    b_mint = ex[1];
                }
            }
            if (isMute)
            {
                if (ex[0] == "m-days")
                {
                    m_days = ex[1];
                }
                if (ex[0] == "m-hours")
                {
                    m_hours = ex[1];
                }
                if (ex[0] == "m-mint")
                {
                    m_mint = ex[1];
                }
            }
            if (isCurse)
            {
                if (ex[0] == "c-days")
                {
                    c_days = ex[1];
                }
                if (ex[0] == "c-hours")
                {
                    c_hours = ex[1];
                }
                if (ex[0] == "c-mint")
                {
                    c_mint = ex[1];
                }
            }
            if (ispdoor)
            {
                if (ex[0] == "pass")
                    ppass = ex[1];
            }
            if (isSign)
            {
                if (ex[0] == "sign")
                {
                    s_text = ex[1];
                }
            }
            if (isRegister)
            {
                if (ex[0] == "username")
                {
                    username = ex[1];
                }
                if (ex[0] == "password")
                {
                    password = ex[1];
                }
                if (ex[0] == "email")
                {
                    email = ex[1];
                }
            }
            if (isFindDialog)
            {
                if (ex[0] == "item")
                {
                    itemFind = ex[1];
                }
            }
            if (iscNameDialog)
            {
                if (ex[0] == "newname")
                {
                    newname = ex[1];
                }
            }
            if (istradedialog)
            {
                if (ex[0] == "itemcount")
                {
                    itemcount = ex[1];
                }
            }
            if (ismgplant)
            {
                if (ex[0] == "mgitem")
                {
                    mgitem = ex[1];
                }
            }
            if (isupdatemag)
            {
                if (ex[0] == "chnitem")
                {
                    mgitem = ex[1];
                }
                if (ex[0] == "ismagactive")
                {
                    ismagactive = ex[1];
                }
            }
            if (iscWorldDialog)
            {
                if (ex[0] == "worldname")
                {
                    newworld = ex[1];
                }
            }
            if (isaddpss)
            {
                if (ex[0] == "addpss")
                    safepass = ex[1];
                if (ex[0] == "chnpss")
                    chnpass = ex[1];
            }
            if (istradedialog)
            {
                if (ex[0] == "itemcount")
                {
                    itemcount = ex[1];
                }
            }
            if (isGuildDialog)
            {
                if (ex[0] == "g-name")
                {
                    g_name = ex[1];
                }
                if (ex[0] == "g-state")
                {
                    g_state = ex[1];
                }
            }
            if (isDropDialog)
            {
                if (ex[0] == "dropitemcount")
                {
                    d_count = ex[1];
                }
            }
            if (isEditLock)
            {
                if (ex[0] == "checkbox_public")
                    pub = ex[1];
                if (ex[0] == "checkbox_disable_music")
                    disable_music = ex[1];
                if (ex[0] == "tempo")
                    tempo = ex[1];
                if (ex[0] == "netid")
                {
                    netid = ex[1];
                    Accesspicker = true;
                }
            }
            if (isDoor)
            {
                if (ex[0] == "dest")
                {
                    dest = ex[1];
                }
                if (ex[0] == "label")
                {
                    label = ex[1];
                }
                if (ex[0] == "doorid")
                {
                    doorid = ex[1];
                }
                if (ex[0] == "isopenpublic")
                {
                    iop = ex[1];
                }
            }
        }
    }
    if (btn == "addmac")
    {
        Worlds *world = getPlyersWorld(server, peer);
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        int idx = -1;
        for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
        {
            if (world->magplant[i].x == x && world->magplant[i].y == y)
                idx = i;
        }
        if (idx != -1)
        {
            int id = world->magplant[idx].id;
            int am = -1;
            for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
            {
                if (pinfo(peer)->inv[i].itemID == id)
                {
                    am = pinfo(peer)->inv[i].itemCount;
                }
            }
            if (am != -1)
            {
                Rem(peer, id, am, true);
                world->magplant[idx].amount += am;
                p.ontalkbubble(peer, pinfo(peer)->netID, "`wAdded " + std::to_string(am) + " " + itemDefs[id].name + " to the magplant", 1);
                remove_clothes_if_wearing(peer, Host, id);
            }
        }
    }
    if (btn == "reitems")
    {
        Worlds *world = getPlyersWorld(server, peer);
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        int idx = -1;
        for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
        {
            if (world->magplant[i].x == x && world->magplant[i].y == y)
                idx = i;
        }
        if (idx != -1)
        {
            int id = world->magplant[idx].id;
            int amount = world->magplant[idx].amount;
            int count = 0;
            for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
            {
                if (pinfo(peer)->inv[i].itemID == id)
                    count = pinfo(peer)->inv[i].itemCount;
            }
            if (amount >= 200 && count == 0)
            {
                world->magplant[idx].amount -= 200;
                Add_ITEM(peer, id, 200, true);
            }
            else if (count != 0 && amount >= 200)
            {
                int left = 200 - count;
                world->magplant[idx].amount -= left;
                Add(peer, id, left, true);
            }
            else if (amount < 200 && count == 0)
            {
                world->magplant[idx].amount = 0;
                Add_ITEM(peer, id, amount, true);
            }
            else if (amount < 200 && count != 0)
            {
                int left = 200 - count;
                world->magplant[idx].amount -= left;
                Add(peer, id, left, true);
            }
        }
    }
    if (btn == "getremote")
    {
        Worlds *world = getPlyersWorld(server, peer);
        int count = 0;
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        int id = 0;
        for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
        {
            if (world->magplant[i].x == x && world->magplant[i].y == y)
                id = world->magplant[i].id;
        }
        for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
        {
            if (world->magplant[i].id == id)
                count++;
        }
        if (count > 1)
        {
            p.ontalkbubble(peer, pinfo(peer)->netID, "`wThere is other magplant have the same item please change one of them in order to get the remote", 1);
            return;
        }
        if (id != 0)
        {
            pinfo(peer)->mgid = id;
            if (!LEGAL_ITEM(peer, 5640))
            {
                Add_ITEM(peer, 5640, 1, true);
            }
        }
    }
    if (btn == "trade")
    {
        std::string name = pinfo(peer)->lastplayerwrench;
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
    if (btn == "punishorview")
    {
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`1Punish/View player``|left|1432|\nadd_button|punish|`1Punish `w(`4Ban`1/`4IP Ban`1/`4ETC`w)``|0|0|\nadd_button|view|`1View `w(`4Check IP`1/`4Information`w)``|0|0|\nadd_spacer|small|\nadd_button|close|`wCancel``|0|0|\nadd_quick_exit");
    }
    if (btn == "punish")
    {
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`1Punish/View player``|left|1432|\nadd_button|toban|`4BAN`|0|0|\nadd_button|tomute|`#MUTE``|0|0|\nadd_button|tocurse|`bCURSE``|0|0|\nadd_spacer|small|\nadd_button|close|`wCancel``|0|0|\nadd_quick_exit");
    }
    if (btn == "toban")
    {
        p.dialog(peer, "add_label_with_icon|big|`4Ban Panel``|left|32|\nadd_textbox|`1User: " + pinfo(peer)->lastplayerwrench + "|\nadd_textbox|`rKeep in mind! Maximum ban you can apply is 730 days.|\nadd_textbox|`1Choose the ban amount|\nadd_text_input|b-days|`oDays:|0|4|\nadd_text_input|b-hours|`oHours:|0|4|\nadd_text_input|b-mint|`oMinutes:|0|4|\nend_dialog|punish-ban-" + pinfo(peer)->lastplayerwrench + "|Cancel|`4BAN|");
    }
    if (btn == "tomute")
    {
        p.dialog(peer, "add_label_with_icon|big|`#Mute Panel``|left|32|\nadd_textbox|`1User: " + pinfo(peer)->lastplayerwrench + "|\nadd_textbox|`rKeep in mind! Maximum mute you can apply is 60 days.|\nadd_textbox|`1Choose the mute amount|\nadd_text_input|m-days|`oDays:|0|4|\nadd_text_input|m-hours|`oHours:|0|4|\nadd_text_input|m-mint|`oMinutes:|0|4|\nend_dialog|punish-mute-" + pinfo(peer)->lastplayerwrench + "|Cancel|`#MUTE|");
    }
    if (btn == "tocurse")
    {
        p.dialog(peer, "add_label_with_icon|big|`bCurse Panel``|left|32|\nadd_textbox|`1User: " + pinfo(peer)->lastplayerwrench + "|\nadd_textbox|`rKeep in mind! Maximum curse you can apply is 60 days.|\nadd_textbox|`1Choose the curse amount|\nadd_text_input|c-days|`oDays:|0|4|\nadd_text_input|c-hours|`oHours:|0|4|\nadd_text_input|c-mint|`oMinutes:|0|4|\nend_dialog|punish-curse-" + pinfo(peer)->lastplayerwrench + "|Cancel|`bCURSE|");
    }
    if (btn == "view")
    {
        ENetPeer *currentPeer = player(Host, pinfo(peer)->lastplayerwrench);
        if (currentPeer != NULL)
        {
            if (pinfo(currentPeer)->username != "hadi" && pinfo(currentPeer)->username != "cmd" && pinfo(currentPeer)->username != "imbusy" && pinfo(currentPeer)->username != "secret" && pinfo(currentPeer)->username != "terminal")
            {
                char clientConnection[16];
                enet_address_get_host_ip(&currentPeer->address, clientConnection, 16);
                std::string ip = clientConnection;
                std::string mac = pinfo(currentPeer)->mac;
                p.dialog(peer, "add_label_with_icon|big|`1View``|left|1432|\nadd_spacer|small|\nadd_label|small|`1IP: " + ip + "``|left|1432|\nadd_label|small|`1MAC: " + mac + "``|left|1432|\nadd_spacer|small|\nadd_label|small|`4Warning:`wSharing player info can lead to lose your mod role and perma ban.``|left|1432|\nadd_quick_exit|\n");
            }
        }
    }
    if (btn == "showfriends")
    {
        std::string onlinefrnlist = "";
        int onlinecount = 0;
        int totalcount = pinfo(peer)->friends.size();
        std::vector<ENetPeer *> all = All(Host);
        for (ENetPeer *currentPeer : all)
        {
            std::string name = pinfo(currentPeer)->username;
            if (find(pinfo(peer)->friends.begin(), pinfo(peer)->friends.end(), name) != pinfo(peer)->friends.end())
            {
                onlinefrnlist += "\nadd_button|onlinefrns_" + pinfo(currentPeer)->username + "|`2ONLINE: `o" + pinfo(currentPeer)->name + "``|0|0|";
                onlinecount++;
            }
        }
        if (totalcount == 0)
        {
            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online``|left|1366|\n\nadd_spacer|small|\nadd_label|small|`oYou currently have no friends.  That's just sad.  To make some, click a person's wrench icon, then choose `5Add as friend`o.``|left|4|\n\nadd_spacer|small|\nadd_button|froption|`oFriend Options``|0|0|\nadd_button|backfr|Back|0|0|\nadd_button||`oClose``|0|0|\nadd_quick_exit|");
        }
        else if (onlinecount == 0)
        {
            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online``|left|1366|\n\nadd_spacer|small|\nadd_button|chc0|`wClose``|0|0|\nadd_label|small|`oNone of your friends are currently online.``|left|4|\n\nadd_spacer|small|\nadd_button|showoffline|`oShow offline``|0|0|\nadd_button|froption|`oFriend Options``|0|0|\nadd_button|backfr|Back|0|0|\nadd_button||`oClose``|0|0|\nadd_quick_exit|");
        }
        else
        {
            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online``|left|1366|\n\nadd_spacer|small|\nadd_button|chc0|`wClose``|0|0|" + onlinefrnlist + "\n\nadd_spacer|small|\nadd_button|showoffline|`oShow offline``|0|0|\nadd_button|froption|`oFriend Options``|0|0|\nadd_button|backfr|Back|0|0|\nadd_button||`oClose``|0|0|\nadd_quick_exit|");
        }
    }
    if (btn == "froption")
    {
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wFriend Options``|left|1366|\nadd_spacer|small|\nadd_checkbox|showlocation|Show location to friends|" + std::to_string(pinfo(peer)->warpme) + "|\nadd_spacer|small|\nadd_button|showfriends|OK``|0|0|");
    }
    if (btn == "backfr")
    {
        p.dialog(peer, "set_default_color|`w\n\nadd_label_with_icon|big|Social Portal``|left|1366|\n\nadd_spacer|small|\nadd_button|showfriends|Show Friends``|0|0|\nend_dialog||OK||\nadd_quick_exit|");
    }
    if (btn == "showoffline")
    {
        std::string onlinelist = "";
        std::string offlinelist = "";
        std::string offname = "";
        int onlinecount = 0;
        int totalcount = pinfo(peer)->friends.size();
        std::vector<std::string> offliness = pinfo(peer)->friends;
        std::vector<ENetPeer *> all = All(Host);
        for (ENetPeer *currentPeer : all)
        {
            std::string name = pinfo(currentPeer)->username;
            if (find(pinfo(peer)->friends.begin(), pinfo(peer)->friends.end(), name) != pinfo(peer)->friends.end())
            {
                onlinelist += "\nadd_button|onlinefrns_" + pinfo(currentPeer)->username + "|`2ONLINE: `o" + pinfo(currentPeer)->name + "``|0|0|";
                onlinecount++;

                offliness.erase(std::remove(offliness.begin(), offliness.end(), name), offliness.end());
            }
        }
        for (std::vector<std::string>::const_iterator i = offliness.begin(); i != offliness.end(); ++i)
        {
            offname = *i;
            offlinelist += "\nadd_button|offlinefrns_" + offname + "|`4OFFLINE: `o" + offname + "``|0|0|";
        }
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online|left|1366|\n\nadd_spacer|small|\nadd_button|chc0|`wClose``|0|0|\nadd_spacer|small|" + offlinelist + "\nadd_spacer|small|\n\nadd_button|froption|`oFriend Options``|0|0|\nadd_button|showfriends|Back``|0|0|\nadd_button||`oClose``|0|0|\nadd_quick_exit|");
    }
    if (btn.substr(0, 11) == "onlinefrns_")
    {
        ENetPeer *currentPeer = player(Host, btn.substr(11, cch.length() - 11 - 1));
        if (currentPeer != NULL)
        {
            pinfo(peer)->lastFrnWorld = pinfo(currentPeer)->currentWorld;
            pinfo(peer)->lastFrn = pinfo(currentPeer)->username;
            if (pinfo(currentPeer)->warpme == 1)
            {
                p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + pinfo(currentPeer)->name + "``|left|1366|\n\nadd_spacer|small|\nadd_textbox|`o" + pinfo(currentPeer)->name + " is `2online `onow in the world `5" + pinfo(currentPeer)->currentWorld + "`o.|\n\nadd_spacer|small|\nadd_button|friendwarp|`oWarp to `5" + pinfo(currentPeer)->currentWorld + "``|0|0|\n\nadd_spacer|small|\nadd_button|removeconoff|`oRemove as friend``|0|0|\nadd_button|showfriends|`oBack``|0|0|\nadd_quick_exit|");
            }
            else
            {
                p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + pinfo(currentPeer)->name + "``|left|1366|\n\nadd_spacer|small|\nadd_textbox|`o" + pinfo(currentPeer)->name + " is `2online `onow, but has not made his/her location public to friends.|\nadd_spacer|small|\nadd_button|removeconoff|`oRemove as friend``|0|0|\nadd_button|showfriends|`oBack``|0|0|\nadd_quick_exit|");
            }
        }
    }
    if (btn.substr(0, 12) == "offlinefrns_")
    {
        pinfo(peer)->lastFrn = btn.substr(12, cch.length() - 12 - 1);
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w" + pinfo(peer)->lastFrn + "``|left|1366|\n\nadd_spacer|small|\nadd_textbox|`o" + pinfo(peer)->lastFrn + " is `4offline`o.``|\nadd_spacer|small|\nadd_button|removeconoff|`oRemove as friend``|0|0|\nadd_button|showoffline|`oBack``|0|0|\nadd_quick_exit|");
    }
    if (btn == "friendwarp")
    {
        if (pinfo(peer)->currentWorld == pinfo(peer)->lastFrnWorld)
        {
            p.onoverlay(peer, "Cant warp to the same world");
        }
        else
        {
            sendPlayerLeave(Host, peer);
            joinWorld(server, Host, peer, pinfo(peer)->lastFrnWorld, 0, 0);
        }
    }
    if (btn == "removeconoff")
    {
        pinfo(peer)->friends.erase(std::remove(pinfo(peer)->friends.begin(), pinfo(peer)->friends.end(), pinfo(peer)->lastFrn), pinfo(peer)->friends.end());
        ENetPeer *currentPeer = player(Host, pinfo(peer)->lastFrn);
        if (currentPeer != NULL)
        {
            p.consoleMessage(currentPeer, "You are no longer friend with " + pinfo(peer)->name);
            pinfo(currentPeer)->friends.erase(std::remove(pinfo(currentPeer)->friends.begin(), pinfo(currentPeer)->friends.end(), pinfo(peer)->username), pinfo(currentPeer)->friends.end());
        }
        else
        {
            std::istream *blobdata = PLAYER_DATA(pinfo(peer)->lastFrn);
            if (blobdata != NULL)
            {
                Players *ply = Desrialize(blobdata);
                ply->friends.erase(std::remove(ply->friends.begin(), ply->friends.end(), pinfo(peer)->username), ply->friends.end());
                UPDATE(serialize_player(ply), pinfo(peer)->lastFrn);
            }
        }
    }
    if (btn == "addfriend")
    {
        ENetPeer *currentPeer = player(Host, pinfo(peer)->lastplayerwrench);
        if (currentPeer != NULL)
        {
            if (pinfo(peer)->lastfriend == pinfo(currentPeer)->username)
            {
                pinfo(peer)->lastfriend = "";
                pinfo(currentPeer)->lastfriend = "";
                pinfo(peer)->friends.push_back(pinfo(currentPeer)->username);
                pinfo(currentPeer)->friends.push_back(pinfo(peer)->username);
                sendmusic(peer, "love_in");
                sendmusic(currentPeer, "love_in");
                p.consoleMessage(peer, "`3FRIEND ADDED: `oYou're now friends with `w" + pinfo(currentPeer)->name + "`o!");
                p.consoleMessage(currentPeer, "`3FRIEND ADDED: `oYou're now friends with `w" + pinfo(peer)->name + "`o!");
            }
            else
            {
                pinfo(currentPeer)->lastfriend = pinfo(peer)->username;
                p.ontalkbubble(peer, pinfo(peer)->netID, "`5[`wFriend request sent to `2" + pinfo(currentPeer)->name + "`5]", 1);
                sendmusic(currentPeer, "tip_start");
                p.consoleMessage(currentPeer, "`3FRIEND REQUEST: `oYou've received a `wfriend request `ofrom `w" + pinfo(peer)->name + "`o! To accept, click the `wwrench by his/her name `oand then choose `wAdd as friend`o.");
            }
        }
    }
    if (btn == "floatitems")
    {
        Worlds *world = getPlyersWorld(server, peer);
        if (world->droppeditems.size() == 0)
        { // found nothing
            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wFloating Items|left|6016|\n\nadd_spacer|small|\nadd_label|small|`wNo floating items was found!|left|4|\n\nadd_spacer|small|\nadd_button|goscan|`oBack``|0|0|\nend_dialog|noty|||gazette||");
        }
        else
        {
            int total = world->droppeditems.size();
            int totalgem = 0;
            std::string floatlist = "";
            for (int i = 0; i < total; i++)
            {
                if (world->droppeditems[i].id == 112)
                {
                    totalgem += world->droppeditems[i].count;
                }
                else
                {
                    floatlist += "\nadd_label_with_icon_button|small|`w" + itemDefs[world->droppeditems[i].id].name + ": " + std::to_string(world->droppeditems[i].count) + "|left|" + std::to_string(world->droppeditems[i].id) + "|arrow-" + std::to_string(world->droppeditems[i].id) + "|" + std::to_string(world->droppeditems[i].id) + "|\n";
                }
            }
            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wFloating Items|left|6016|\n\nadd_spacer|small|\nadd_label|small|`wTotal floating items in world: " + std::to_string(total) + "|left|4|\nadd_label|small|`wTotal gems: " + std::to_string(totalgem) + "|left|4|\n\nadd_spacer|small|\nadd_button|goscan|`oBack``|0|0|\n\nadd_spacer|small|" + floatlist + "\n\nadd_spacer|small|\nadd_button|goscan|`oBack``|0|0|\nend_dialog|noty|||gazette||");
        }
    }
    if (btn == "goscan")
    {
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wWorld Stats|left|6016|\nadd_label|small|`wThis amazing block can show the stats for the whole world!|left|4|\n\nadd_spacer|small|\nadd_label|small|`wWhich stats would you like to view?|left|4|\nadd_button|floatitems|`oFloating Items``|0|0|\nadd_quick_exit|\nend_dialog|noty|Cancel||gazette||");
    }
    if (btn.substr(0, 6) == "arrow-")
    {
        int id = -1;
        if (is_digits(btn.substr(6, btn.length() - 6)))
            id = atoi(btn.substr(6, btn.length() - 6).c_str());
        if (id != -1)
            sendArrow(peer, id, true);
    }
    if (btn == "wban")
    {
        Worlds *world = getPlyersWorld(server, peer);
        if (pinfo(peer)->username == world->owner || pinfo(peer)->adminLevel >= 666)
        {
            std::string name = pinfo(peer)->lastplayerwrench;
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
    if (btn == "kick")
    {
        std::string name = pinfo(peer)->lastplayerwrench;
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
    if (btn == "pull")
    {
        std::string name = pinfo(peer)->lastplayerwrench;
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
    if (btn == "dothetrade")
    {
        ENetPeer *currentPeer = player(Host, pinfo(peer)->tradingme);
        if (currentPeer != NULL)
        {
            if (pinfo(peer)->canceled)
            {
                reset_trade(peer, false);
                return;
            }
            if (pinfo(currentPeer)->dotrade)
            {
                for (int i = 0; i < static_cast<int>(pinfo(peer)->trade.size()); i++)
                {
                    int val = 0;
                    if (i == 0)
                        val = 300;
                    if (i == 1)
                        val = 800;
                    if (i == 2)
                        val = 1300;
                    if (i == 3)
                        val = 1800;
                    sendtrade(Host, currentPeer, pinfo(peer)->trade[i].itemid, pinfo(peer)->netID, pinfo(currentPeer)->netID, val);
                    sendtrade(Host, peer, pinfo(peer)->trade[i].itemid, pinfo(peer)->netID, pinfo(currentPeer)->netID, val);
                    Add(currentPeer, pinfo(peer)->trade[i].itemid, pinfo(peer)->trade[i].count, true);
                    Rem(peer, pinfo(peer)->trade[i].itemid, pinfo(peer)->trade[i].count, true);
                    sendmusic(peer, "change_clothes");
                }
                for (int i = 0; i < static_cast<int>(pinfo(currentPeer)->trade.size()); i++)
                {
                    int val = 0;
                    if (i == 0)
                        val = 300;
                    if (i == 1)
                        val = 800;
                    if (i == 2)
                        val = 1300;
                    if (i == 3)
                        val = 1800;
                    sendtrade(Host, peer, pinfo(currentPeer)->trade[i].itemid, pinfo(currentPeer)->netID, pinfo(peer)->netID, val);
                    sendtrade(Host, currentPeer, pinfo(currentPeer)->trade[i].itemid, pinfo(currentPeer)->netID, pinfo(peer)->netID, val);
                    Add(peer, pinfo(currentPeer)->trade[i].itemid, pinfo(currentPeer)->trade[i].count, true);
                    Rem(currentPeer, pinfo(currentPeer)->trade[i].itemid, pinfo(currentPeer)->trade[i].count, true);
                    sendmusic(currentPeer, "change_clothes");
                }
                reset_trade(peer, false);
                reset_trade(currentPeer, false);
                sendInventory(peer);
                sendInventory(currentPeer);
                sendmusic(peer, "keypad_hit");
                sendmusic(currentPeer, "keypad_hit");
            }
            else
            {
                pinfo(peer)->dotrade = true;
                p.ontalkbubble(peer, pinfo(peer)->netID, "`o[`wTrade accepted, waiting for other player to accept`o]", 1);
            }
        }
    }
    if (btn == "notrade")
    {
        ENetPeer *currentPeer = player(Host, pinfo(peer)->tradingme);
        if (currentPeer != NULL)
        {
            pinfo(currentPeer)->canceled = true;
            reset_trade(currentPeer, false);
            p.onoverlay(currentPeer, "The player canceled the trade");
        }
        reset_trade(peer, false);
    }
    if (btn == "spsschange")
    {
        p.dialog(peer, "add_label_with_icon|big|`wSafe Vault|left|8878|\nadd_label|small|`oThis Safe Vault is `2password protected`o!|left|8874|\nadd_spacer|small|\nadd_label|small|`oUpdate Password.|left|8874|\nadd_text_input|chnpss|||12|\nend_dialog|spss|`wExit|`wEnter Password|\nadd_quick_exit|\n");
    }
    /*if (btn == "stakeit")
    {
        Worlds *world = getPlyersWorld(server, peer);
        int amount = -1;
        int idx = -1;
        if (is_digits(stake))
            amount = std::stoi(stake);
        if (amount == -1)
            return;
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        for (int i = 0; i < static_cast<int>(world->safe.size()); i++)
        {
            if (world->safe[i].x == x && world->safe[i].y == y)
            {
                if (world->safe[i].id == pinfo(peer)->lastakeid)
                {
                    idx = i;
                }
            }
        }
        if (idx == -1)
            return;
        int amount_in_safe = world->safe[idx].count;
        if (amount > amount_in_safe)
        {
            p.consoleMessage(peer, "This Safe Vault doesnt have this amount");
            return;
        }
        if (amount == amount_in_safe)
        { //should be removed from safe vault and be added to inventory
            if (!MoreThan200(peer, world->safe[idx].id, amount))
            {
                world->safe.erase(world->safe.begin() + idx);
                inventory::plus(peer, pdata(peer)->lastakeid, amount_in_safe);
                world->safesize = world->safe.size();
                sendInventory(peer, pdata(peer)->inventory);
            }
            else
            {
                
            }
        }
        if (amount < amount_in_safe)
        {
            world->safe[idx].count = world->safe[idx].count - amount;
            inventory::plus(peer, pdata(peer)->lastakeid, amount);
            sendInventory(peer, pdata(peer)->inventory);
        }
    }*/
    if (btn.substr(0, 5) == "store")
    {
        std::string name = btn.substr(5, cch.length() - 5 - 1);
    }
    if (btn == "ssbac")
    {
        Worlds *world = getPlyersWorld(server, peer);
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        int count = 0;
        std::string items = "";
        std::string endlist = "";
        for (int i = 0; i < static_cast<int>(world->safe.size()); i++)
        {
            if (world->safe[i].x == x && world->safe[i].y == y)
            {
                if (world->safe[i].id != 0)
                {
                    count++;
                    items += "\nadd_button_with_icon|safe" + std::to_string(world->safe[i].id) + "|" + itemDefs[world->safe[i].id].name + "|staticBlueFrame|" + std::to_string(world->safe[i].id) + "|" + std::to_string(world->safe[i].count) + "|";
                }
            }
        }
        if (items != "")
            endlist = "\nadd_button_with_icon||END_LIST|noflags|0|0|";
        p.dialog(peer, "add_label_with_icon|big|`wSafe Vault|left|8878|\nadd_spacer|small|" + items + "" + endlist + "\nadd_label|small|`w" + std::to_string(count) + "/20 `oitems stored.|left|8874|\nadd_spacer|small|\nadd_item_picker|dpitem|`wDeposit Item|Choose an item to store|\nadd_label|small|`oThis Safe Vault is `2password protected`o!|left|8874|\nadd_spacer|small|\nadd_label|small|`oChange your password.|left|8874|\nadd_button|spsschange|`wUpdate Password``|0|0|\nend_dialog|fds|Exit|");
    }
    /*if (btn.substr(0, 4) == "safe")
    {
        Worlds *world = getPlyersWorld(server , peer);
        int id = -1;
        if (is_digits(btn.substr(4, btn.length() - 4)))
            id = atoi(btn.substr(4, btn.length() - 4).c_str());
        if (id == -1)
            return;
        pinfo(peer)->lastakeid = id;
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        int count = -1;
        int idx = -1;
        for (int i = 0; i < static_cast<int>(world->safe.size()); i++)
        {
            if (world->safe[i].x == x && world->safe[i].y == y)
            {
                if (world->safe[i].id == id)
                {
                    count = world->safe[i].count;
                    idx = i;
                }
            }
        }
        if (count == -1)
            return;
        p.dialog(peer, "add_label_with_icon|big|`wSafe Vault|left|8878|\nadd_label|small|`oYou have `w" + to_string(count) + " " + getItemDef(id).name + " `ostored.|left|8874|\nadd_label|small|`oWithdraw how many?|left|8874|\nadd_text_input|stake||" + to_string(count) + "|3|\nadd_spacer|small|\nadd_button|stakeit|`oRemove Items``|0|0|\nadd_button|ssbac|`oBack``|0|0|\nend_dialog|fdss||");
    }*/
    if (btn == "create_guild")
    {
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild``|left|5814|\nadd_textbox|`oWelcome to Grow Guilds where you can create a Guild! With a Guild you can complete in Guild Leaderboards to earn rewards and level up the Guild to add more members.``|\nadd_spacer|small|\nadd_textbox|`oYou will be charged: `6100,000 `oGems.``|\nadd_spacer|small|\nadd_textbox|`8Caution`o: A guild can only be created in a world owned by you and locked with a `5World Lock`o!``|\nadd_button|guild_create|`oCreate a Guild``|left|||\nadd_button|back_social|`oBack``|left|||\nend_dialog||Close||");
    }
    if (btn == "guild_create")
    {
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation``|left|5814|\nadd_text_input|g-name|Guild Name:||18|\nadd_text_input|g-state|Guild Statement:||30|\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.``|\nadd_textbox|`8Remember`o: A guild can only be created in a world owned by you and locked with a `5World Lock`o!``|\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!``|\nadd_button|ok-guild|`oCreate Guild``|left|||\nend_dialog|guild|Close||");
    }
    if (btn == "back_social")
    {
        p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wSocial Portal``|left|1366|\n\nadd_spacer|small|\nadd_button|friends|`wShow Friends``|left|||\nadd_button|create_guild|`wCreate Guild``|left|||\nend_dialog||OK||");
    }
    /*if (btn.substr(0, 6) == "guild-")
    {
        std::vector<std::string> ex = explode("-", btn);
        Guilds guild;
        guild.leader = pinfo(peer)->username;
        guild.name = ex[1];
        guild.statement = ex[2];
        guild.level = 1;
        guild.max = 5;
        guild.world = pinfo(peer)->currentWorld;
        guild.xp = 0;
        INSERT_GUILD(ex[1], serialize_guild(guild));
        pinfo(peer)->guild = ex[1];
        //now to change the World Lock to Guild Lock
        int x = 0;
        int y = 0;
        Worlds *wld = getPlyersWorld(peer);
        for (int j = 0; j < wld->width * wld->height; j++)
        {
            if (itemDefs[wld->items[j % wld->width][j / wld->width].foreground].blockType == BlockTypes::LOCK)
            {
                x = (j % wld->width);
                y = (j / wld->width);
            }
        }
        wld->items[x][y].foreground = 5814;
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
        data.plantingTree = 5814;
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
    }*/
    if (ismgplant)
    {
        if (is_digits(mgitem))
        {
            int a = stoi(mgitem);
            int x = pinfo(peer)->wrenchx;
            int y = pinfo(peer)->wrenchy;
            pushmag(server, peer, a);
            std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
            for (ENetPeer *currentPeer : ishere)
            {
                sendMag(peer, x, y, a, 1, false, false);
            }
        }
    }
    if (isupdatemag)
    {
        if (mgitem != "")
        {
            if (is_digits(mgitem))
            {
                int b = 0;
                if (is_digits(ismagactive))
                {
                    b = stoi(ismagactive);
                }
                Worlds *world = getPlyersWorld(server, peer);
                int a = stoi(mgitem);
                int x = pinfo(peer)->wrenchx;
                int y = pinfo(peer)->wrenchy;
                int idx = -1;
                for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
                {
                    if (world->magplant[i].x == x && world->magplant[i].y == y)
                        idx = i;
                }
                if (idx != -1)
                {
                    world->magplant[idx].id = a;
                    world->magplant[idx].active = b;
                    std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
                    for (ENetPeer *currentPeer : ishere)
                    {
                        sendMag(peer, x, y, a, 1, false, false);
                    }
                }
            }
        }
        else
        {
            int b = 0;
            if (is_digits(ismagactive))
            {
                b = stoi(ismagactive);
            }
            Worlds *world = getPlyersWorld(server, peer);
            int x = pinfo(peer)->wrenchx;
            int y = pinfo(peer)->wrenchy;
            int idx = -1;
            for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
            {
                if (world->magplant[i].x == x && world->magplant[i].y == y)
                    idx = i;
            }
            if (idx != -1)
            {
                world->magplant[idx].active = b;
            }
        }
    }
    if (ispdoor)
    {
        Worlds *world = getPlyersWorld(server, peer);
        std::string asd = "";
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        int idx = -1;
        for (int i = 0; i < static_cast<int>(world->door.size()); i++)
        {
            if (world->door[i].x == x && world->door[i].y == y)
            {
                asd = world->door[i].id;
                idx = i;
            }
        }
        if (ppass == asd)
        {
            std::string dest = world->door[idx].dest;
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
        else
        {
            p.ontalkbubble(peer, pinfo(peer)->netID, "`4Wrong password!", 1);
        }
    }
    if (istradedialog)
    {
        if (is_digits(itemcount))
        {
            int count = stoi(itemcount);
            if (count < 0 || count > 200)
            {
                p.consoleMessage(peer, "You cant trade more than 200 or less than 0");
            }
            else
            {
                int am = 0;
                int idx = -1;
                for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
                {
                    if (pinfo(peer)->inv[i].itemID == pinfo(peer)->lasttradeid)
                    {
                        am = pinfo(peer)->inv[i].itemCount;
                    }
                }
                if (am < count)
                {
                    p.consoleMessage(peer, "You dont have that amount");
                    return;
                }
                ENetPeer *currentPeer = player(Host, pinfo(peer)->tradingme);
                if (currentPeer != NULL)
                {
                    for (int i = 0; i < static_cast<int>(pinfo(currentPeer)->inv.size()); i++)
                    {
                        if (pinfo(currentPeer)->inv[i].itemID == pinfo(peer)->lasttradeid)
                        {
                            if (pinfo(currentPeer)->inv[i].itemCount + count > 200)
                            {
                                p.onoverlay(peer, "`4the player cant handle more than 200");
                                return;
                            }
                        }
                    }
                }
                else
                {
                    return;
                }
                //check if it already in trade or no
                for (int i = 0; i < static_cast<int>(pinfo(peer)->trade.size()); i++)
                {
                    if (pinfo(peer)->trade[i].itemid == pinfo(peer)->lasttradeid)
                    {
                        idx = i;
                    }
                }
                if (idx != -1)
                {
                    pinfo(peer)->trade[idx].count = count;
                }
                else
                {
                    Trade trd;
                    trd.itemid = pinfo(peer)->lasttradeid;
                    trd.count = count;
                    pinfo(peer)->trade.push_back(trd);
                }
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
    if (Accesspicker)
    {
        if (!is_digits(netid))
            return;
        Worlds *world = getPlyersWorld(server, peer);
        std::vector<ENetPeer *> all = All(Host);
        for (ENetPeer *currentPeer : all)
        {
            if (pinfo(currentPeer)->netID == std::stoi(netid))
            {
                if (pinfo(peer)->username == pinfo(currentPeer)->username)
                {
                    p.consoleMessage(peer, "`oyou cant access your self");
                    return;
                }
                std::string accessnames = "";
                for (std::vector<std::string>::const_iterator i = world->access.begin(); i != world->access.end(); ++i)
                {
                    accessnames = *i;
                }
                if (pinfo(currentPeer)->username == accessnames)
                {
                    p.consoleMessage(peer, "This player already have access.");
                }
                else
                {
                    std::string accessname = pinfo(currentPeer)->username;
                    world->access.push_back(accessname);
                    p.consoleMessage(peer, "You gave access to `2" + accessname);
                    p.consoleMessage(currentPeer, "`2You got world access!");
                    sendmusic(currentPeer, "secret");
                }
            }
        }
    }
    if (isaddpss)
    {
        if (safepass != "")
        {
            pushsafe(server, peer, 0, 0, safepass);
            p.ontalkbubble(peer, pinfo(peer)->netID, "`2Your password has been updated!", 1);
        }
        if (chnpass != "")
        {
            Worlds *world = getPlyersWorld(server, peer);
            int x = pinfo(peer)->wrenchx;
            int y = pinfo(peer)->wrenchy;
            int idx = -1;
            for (int i = 0; i < static_cast<int>(world->safe.size()); i++)
            {
                if (world->safe[i].x == x && world->safe[i].y == y)
                {
                    if (world->safe[i].pass != "")
                        idx = i; //to check if it have a password or no
                }
            }
            if (idx == -1)
                return;
            world->safe[idx].pass = chnpass;
            p.ontalkbubble(peer, pinfo(peer)->netID, "`2Your password has been updated!", 1);
        }
    }
    if (iscWorldDialog)
    {
        if (newworld.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
        {
            p.consoleMessage(peer, "`rPlease remove special characters");
            return;
        }
        int id = GET_WORLD(server, newworld).id;
        int idd = GET_WORLD(server, pinfo(peer)->currentWorld).id;
        if (id != -1 && idd != -1)
        {
            Worlds *world = getPlyersWorld(server, peer);
            Worlds *world1 = GET_WORLD(server, newworld).ptr;
            if (world->owner == pinfo(peer)->username && world1->owner == pinfo(peer)->username)
            {
                std::string name1 = world->Name;
                std::string name2 = world1->Name;
                world->Name = name2;
                world1->Name = name1;
                Rem(peer, 2580, 1, true);
                std::vector<ENetPeer *> all = All(Host);
                for (ENetPeer *currentPeer : all)
                {
                    if (pinfo(currentPeer)->currentWorld == name1 || pinfo(currentPeer)->currentWorld == name2)
                    {
                        p.consoleMessage(currentPeer, "`rWorld Name of this world has been changed");
                        sendPlayerLeave(Host, currentPeer);
                    }
                }
            }
            else
            {
                p.consoleMessage(peer, "`ryou are not owner of either this world or the other one");
            }
        }
        else
        {
            p.consoleMessage(peer, "`rif you see this please contact `6@Developers");
        }
    }
    /*if (issafedialog)
    {
        Worlds *world = getPlyersWorld(server, peer);
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        std::string pss = "";
        std::string endlist = "";
        for (int i = 0; i < world->safe.size(); i++)
        {
            if (world->safe[i].x == x && world->safe[i].y == y)
            {
                if (world->safe[i].pass != "")
                    pss = world->safe[i].pass;
            }
        }
        if (pss == uspass)
        {
            int count = 0;
            std::string items = "";
            for (int i = 0; i < world->safe.size(); i++)
            {
                if (world->safe[i].x == x && world->safe[i].y == y)
                {
                    if (world->safe[i].id != 0)
                    {
                        count++;
                        items += "\nadd_button_with_icon|safe" + std::to_string(world->safe[i].id) + "|" + itemDefs[world->safe[i].id].name + "|staticBlueFrame|" + std::to_string(world->safe[i].id) + "|" + std::to_string(world->safe[i].count) + "|";
                    }
                }
            }
            if (items != "")
                endlist = "\nadd_button_with_icon||END_LIST|noflags|0|0|";
            p.dialog(peer, "add_label_with_icon|big|`wSafe Vault|left|8878|\nadd_spacer|small|" + items + endlist + "\nadd_label|small|`w" + std::to_string(count) + "/20 `oitems stored.|left|8874|\nadd_spacer|small|\nadd_item_picker|dpitem|`wDeposit Item|Choose an item to store|\nadd_label|small|`oThis Safe Vault is `2password protected`o!|left|8874|\nadd_spacer|small|\nadd_label|small|`oChange your password.|left|8874|\nadd_button|spsschange|`wUpdate Password``|0|0|\nend_dialog|fds|Exit|");
        }
        else
        {
            p.dialog(peer, "add_label_with_icon|big|`wSafe Vault|left|8878|\nadd_label|small|`4Wrong Password.|left|8874|\nadd_label|small|`oPlease enter your password to access the Safe Vault.|left|8874|\nadd_text_input|safepss|||12|\nend_dialog|safe|`wExit|`wEnter Password|\nadd_quick_exit|\n");
        }
    }*/
    if (iscNameDialog)
    {
        if (newname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
        {
            p.consoleMessage(peer, "`rPlease remove special characters");
            return;
        }
        if (pinfo(peer)->gem < 100000)
        {
            p.dialog(peer, "add_label_with_icon|big|`wChange your GrowID|left|1280|\nadd_label|small|`4Sorry! `2you dont have enough gems to change your GrowID``|left|4|\nadd_label|small|`oThis will change your GrowID `4permanently`o.``|left|4|\nadd_label|small|`oYou will pay `2100,000 $ `oif you click on `5Change it`o.``|left|4|\nadd_textbox|`oEnter your new name: |\nadd_text_input|newname|||20|\nend_dialog|cnamedialog|Cancel|Change it!|\n");
            return;
        }
        if (!ACCOUNT_EXIST(newname))
        {
            UPDATE_NAME(newname, pinfo(peer)->userID);
            pinfo(peer)->username = newname;
            p.sethasgrowid(peer, 1, pinfo(peer)->username, pinfo(peer)->password);
            pinfo(peer)->gem -= 100000;
            p.onsetbux(peer);
            Rem(peer, 1280, 1, true);
        }
        else
        {
            p.dialog(peer, "add_label_with_icon|big|`wChange your GrowID|left|1280|\nadd_label|small|`4I know `othis GrowID name is nice but `2" + newname + " `4is already in use!`o.``|left|4|\nadd_label|small|`oThis will change your GrowID `4permanently`o.``|left|4|\nadd_label|small|`oYou will pay `2100,000 $ `oif you click on `5Change it`o.``|left|4|\nadd_textbox|`oEnter your new name: |\nadd_text_input|newname|||20|\nend_dialog|cnamedialog|Cancel|Change it!|\n");
            return;
        }
    }
    if (isEditLock)
    {
        Worlds *world = getPlyersWorld(server, peer);
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        int fg = world->items[x][y].foreground;
        int bg = world->items[x][y].background;
        uint8_t lol = 0x00;
        if (!is_digits(tempo))
            return;
        int tempoint = atoi(tempo.c_str());
        if (disable_music == "1")
        {
            lol |= 0x10;
        }
        if (disable_music == "1")
            world->dmb = 1;
        if (disable_music == "0")
            world->dmb = 0;
        world->pbm = tempoint;
        std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
        for (ENetPeer *currentPeer : ishere)
        {
            sendTileData(currentPeer, x, y, 0x00, fg, bg, lockTileDatas(lol, world->id, 0, 0, false, tempoint));
        }
    }
    if (isDoor)
    {
        Worlds *world = getPlyersWorld(server, peer);
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        int fg = world->items[x][y].foreground;
        int bg = world->items[x][y].background;
        int idx = -1;
        for (int i = 0; i < static_cast<int>(world->door.size()); i++)
        {
            if (world->door[i].x == x && world->door[i].y == y)
                idx = i;
        }
        if (idx != -1)
        {
            world->door[idx].dest = dest;
            world->door[idx].id = doorid;
            world->door[idx].label = label;
            world->door[idx].is_open = iop;
        }
        else
        {
            pushdoor(server, peer, dest, label, doorid, iop);
        }
        if (label == "")
            label = "...";
        std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
        for (ENetPeer *currentPeer : ishere)
        {
            if (iop == "1")
            {
                updatedoor(currentPeer, fg, bg, x, y, label, false);
            }
            else
            {
                if (pinfo(currentPeer)->userID != world->id && pinfo(currentPeer)->adminLevel < 666)
                {
                    updatedoor(currentPeer, fg, bg, x, y, label, true);
                }
                else
                {
                    updatedoor(currentPeer, fg, bg, x, y, label, false);
                }
            }
        }
    }
    if (isBan)
    {
        server->sendGlobalMessage("`#** `$The Ancients `ohave used `4Ban `oon `w" + p_name + "`o! `#**", "beep");
        server->sendModsLogs("`1" + pinfo(peer)->name + " `$have `4ban `$" + p_name, "beep", true);
        ENetPeer *currentPeer = player(Host, p_name);
        if (currentPeer != NULL)
        {
            if (b_days != "0" && b_hours == "0" && b_mint == "0")
            {
                if (is_digits(b_days))
                {
                    pinfo(currentPeer)->ban = time(NULL);
                    int days = std::stoi(b_days) * 24 * 60 * 60;
                    pinfo(currentPeer)->bantime = days;
                    p.onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `4BANNED `0from Private Server for " + b_days + " days.", "audio/hub_open.wav");
                    enet_peer_disconnect_later(currentPeer, 0);
                }
            }
            if (b_days == "0" && b_hours != "0" && b_mint == "0")
            {
                if (is_digits(b_hours))
                {
                    pinfo(currentPeer)->ban = time(NULL);
                    int hours = std::stoi(b_hours) * 60 * 60;
                    pinfo(currentPeer)->bantime = hours;
                    p.onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `4BANNED `0from Private Server for " + b_hours + " hours.", "audio/hub_open.wav");
                    enet_peer_disconnect_later(currentPeer, 0);
                }
            }
            if (b_days == "0" && b_hours == "0" && b_mint != "0")
            {
                if (is_digits(b_mint))
                {
                    pinfo(currentPeer)->ban = time(NULL);
                    int mint = std::stoi(b_mint) * 60;
                    pinfo(currentPeer)->bantime = mint;
                    p.onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `4BANNED `0from Private Server for " + b_mint + " minutes.", "audio/hub_open.wav");
                    enet_peer_disconnect_later(currentPeer, 0);
                }
            }
        }
        else
        {
            std::istream *blobdata = PLAYER_DATA(p_name);
            if (blobdata != NULL)
            {
                if (b_days != "0" && b_hours == "0" && b_mint == "0")
                {
                    if (is_digits(b_days))
                    {
                        Players *ply = Desrialize(blobdata);
                        ply->ban = time(NULL);
                        int days = std::stoi(b_days) * 24 * 60 * 60;
                        ply->bantime = days;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
                if (b_days == "0" && b_hours != "0" && b_mint == "0")
                {
                    if (is_digits(b_hours))
                    {
                        Players *ply = Desrialize(blobdata);
                        ply->ban = time(NULL);
                        int hours = std::stoi(b_hours) * 60 * 60;
                        ply->bantime = hours;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
                if (b_days == "0" && b_hours == "0" && b_mint != "0")
                {
                    if (is_digits(b_mint))
                    {
                        Players *ply = Desrialize(blobdata);
                        ply->ban = time(NULL);
                        int mint = std::stoi(b_mint) * 60;
                        ply->bantime = mint;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
            }
        }
    }
    if (isMute)
    {
        server->sendGlobalMessage("`#** `$The Ancients `ohave used `#Mute `oon `w" + p_name + "`o! `#**", "beep");
        server->sendModsLogs("`1" + pinfo(peer)->name + " `$have `#mute `$" + p_name, "beep", true);
        ENetPeer *currentPeer = player(Host, p_name);
        if (currentPeer != NULL)
        {
            if (m_days != "0" && m_hours == "0" && m_mint == "0")
            {
                if (is_digits(m_days))
                {
                    pinfo(currentPeer)->mute = time(NULL);
                    int days = std::stoi(m_days) * 24 * 60 * 60;
                    pinfo(currentPeer)->mutetime = days;
                    p.onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `#MUTED `0from Private Server for " + m_days + " days.", "audio/hub_open.wav");
                }
            }
            if (m_days == "0" && m_hours != "0" && m_mint == "0")
            {
                if (is_digits(m_hours))
                {
                    pinfo(currentPeer)->mute = time(NULL);
                    int hours = std::stoi(m_hours) * 60 * 60;
                    pinfo(currentPeer)->mutetime = hours;
                    p.onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `#MUTED `0from Private Server for " + m_hours + " hours.", "audio/hub_open.wav");
                }
            }
            if (m_days == "0" && m_hours == "0" && m_mint != "0")
            {
                if (is_digits(m_mint))
                {
                    pinfo(currentPeer)->mute = time(NULL);
                    int mint = std::stoi(m_mint) * 60;
                    pinfo(currentPeer)->mutetime = mint;
                    p.onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `4BANNED `0from Private Server for " + m_mint + " minutes.", "audio/hub_open.wav");
                }
            }
        }
        else
        {
            std::istream *blobdata = PLAYER_DATA(p_name);
            if (blobdata != NULL)
            {
                if (m_days != "0" && m_hours == "0" && m_mint == "0")
                {
                    if (is_digits(m_days))
                    {
                        Players *ply = Desrialize(blobdata);
                        ply->mute = time(NULL);
                        int days = std::stoi(m_days) * 24 * 60 * 60;
                        ply->mutetime = days;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
                if (m_days == "0" && m_hours != "0" && m_mint == "0")
                {
                    if (is_digits(m_hours))
                    {
                        Players *ply = Desrialize(blobdata);
                        ply->mute = time(NULL);
                        int hours = std::stoi(m_hours) * 60 * 60;
                        ply->mutetime = hours;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
                if (m_days == "0" && m_hours == "0" && m_mint != "0")
                {
                    if (is_digits(m_mint))
                    {
                        Players *ply = Desrialize(blobdata);
                        ply->mute = time(NULL);
                        int mint = std::stoi(m_mint) * 60;
                        ply->mutetime = mint;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
            }
        }
    }
    if (isCurse)
    {
        server->sendGlobalMessage("`#** `$The Ancients `ohave used `bCurse `oon `w" + p_name + "`o! `#**", "beep");
        server->sendModsLogs("`1" + pinfo(peer)->name + " `$have `bcurse `$" + p_name, "beep", true);
        ENetPeer *currentPeer = player(Host, p_name);
        if (currentPeer != NULL)
        {
            if (c_days != "0" && c_hours == "0" && c_mint == "0")
            {
                if (is_digits(c_days))
                {
                    pinfo(currentPeer)->curse = time(NULL);
                    int days = std::stoi(c_days) * 24 * 60 * 60;
                    pinfo(currentPeer)->cursetime = days;
                    p.onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `bCURSED `0from Private Server for " + c_days + " days.", "audio/hub_open.wav");
                    sendPlayerLeave(Host, currentPeer);
                    joinWorld(server, Host, currentPeer, "HELL", 0, 0);
                }
            }
            if (c_days == "0" && c_hours != "0" && c_mint == "0")
            {
                if (is_digits(c_hours))
                {
                    pinfo(currentPeer)->curse = time(NULL);
                    int hours = std::stoi(c_hours) * 60 * 60;
                    pinfo(currentPeer)->cursetime = hours;
                    p.onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `bCURSED `0from Private Server for " + c_hours + " hours.", "audio/hub_open.wav");
                    sendPlayerLeave(Host, currentPeer);
                    joinWorld(server, Host, currentPeer, "HELL", 0, 0);
                }
            }
            if (c_days == "0" && c_hours == "0" && c_mint != "0")
            {
                if (is_digits(c_mint))
                {
                    pinfo(currentPeer)->curse = time(NULL);
                    int mint = std::stoi(c_mint) * 60;
                    pinfo(currentPeer)->cursetime = mint;
                    p.onaddnotification(currentPeer, "`0Warning from `4System`0: You've been `bCURSED `0from Private Server for " + c_mint + " minutes.", "audio/hub_open.wav");
                    sendPlayerLeave(Host, currentPeer);
                    joinWorld(server, Host, currentPeer, "HELL", 0, 0);
                }
            }
        }
        else
        {
            std::istream *blobdata = PLAYER_DATA(p_name);
            if (blobdata != NULL)
            {
                if (c_days != "0" && c_hours == "0" && c_mint == "0")
                {
                    if (is_digits(c_days))
                    {
                        Players *ply = Desrialize(blobdata);
                        ply->curse = time(NULL);
                        int days = std::stoi(c_days) * 24 * 60 * 60;
                        ply->cursetime = days;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
                if (c_days == "0" && c_hours != "0" && c_mint == "0")
                {
                    if (is_digits(c_hours))
                    {
                        Players *ply = Desrialize(blobdata);
                        ply->curse = time(NULL);
                        int hours = std::stoi(c_hours) * 60 * 60;
                        ply->cursetime = hours;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
                if (c_days == "0" && c_hours == "0" && c_mint != "0")
                {
                    if (is_digits(c_mint))
                    {
                        Players *ply = Desrialize(blobdata);
                        ply->curse = time(NULL);
                        int mint = std::stoi(c_mint) * 60;
                        ply->cursetime = mint;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
            }
        }
    }
    if (isSign)
    {
        Worlds *world = getPlyersWorld(server, peer);
        int x = pinfo(peer)->wrenchx;
        int y = pinfo(peer)->wrenchy;
        int fg = world->items[x][y].foreground;
        int bg = world->items[x][y].background;
        int idx = -1;
        for (int i = 0; i < static_cast<int>(world->sign.size()); i++)
        {
            if (world->sign[i].x == x && world->sign[i].y == y)
            {
                idx = i;
            }
        }
        if (idx != -1)
        {
            world->sign[idx].text = s_text;
        }
        else
        {
            pushsign(server, peer, s_text);
        }
        std::vector<ENetPeer *> ishere = isHere(Host, peer, true);
        for (ENetPeer *currentPeer : ishere)
        {

            updatesign(currentPeer, fg, bg, x, y, s_text);
        }
    }

    /*if (isGuildDialog)
    {
        Worlds wld = getPlyersWorld(server, peer);
        if (g_name.length() < 1)
        {
            return;
        }
        if (g_name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
        {
            p.consoleMessage(peer, "`4Guild Name`o cant contain symbols");
            return;
        }
        if (g_state.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
        {
            p.consoleMessage(peer, "`4Guild Statement`o cant contain symbols");
            return;
        }
        if (wld.owner != pinfo(peer)->username)
        {
            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation``|left|5814|\nadd_text_input|g-name|Guild Name:|" + g_name + "|18|\nadd_text_input|g-state|Guild Statement:|" + g_state + "|30|\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.``|\nadd_textbox|`8Remember`o: A guild can only be created in a world owned by you and locked with a `5World Lock`o!``|\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!``|\nadd_button|ok-guild|`oCreate Guild``|left|||\nend_dialog|guild|Close||");
            return;
        }
        if (!GUILD_EXIST(g_name))
        {
            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild``|left|5814|\nadd_textbox|`1Guild Name: `o" + g_name + "``|\nadd_textbox|`1Guild Statement: `o" + g_state + "``|\nadd_spacer|small|\nadd_textbox|`oCost `4100000 Gems``|\nadd_spacer|small|\nadd_button|guild-" + g_name + "-" + g_state + "|`oCreate Guild``|left|||\nend_dialog||Close||");
        }
        else
        {
            p.consoleMessage(peer, "`4Other Guild have this name");
        }
    }*/
    if (isDropDialog)
    {
        if (is_digits(d_count))
        {
            int amount = stoi(d_count);
            if (amount > 0 && amount <= 200)
            {
                int idx = -1;
                for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
                {
                    if (pinfo(peer)->inv[i].itemID == pinfo(peer)->dropid)
                    {
                        idx = i;
                    }
                }
                if (idx != -1)
                {
                    Worlds *world = getPlyersWorld(server, peer);
                    float xx = pinfo(peer)->x + (32 * (pinfo(peer)->isRotatedLeft ? -1 : 1));
                    float yy = pinfo(peer)->y;

                    int am = pinfo(peer)->inv[idx].itemCount;
                    int new_value = am - amount;
                    if (new_value < 0)
                    {
                        return;
                    }
                    else if (new_value == 0)
                    {
                        pinfo(peer)->inv.erase(pinfo(peer)->inv.begin() + idx);
                        remove_clothes_if_wearing(peer, Host, pinfo(peer)->dropid);
                    }
                    else
                    {
                        pinfo(peer)->inv[idx].itemCount = new_value;
                    }
                    sendInventory(peer);
                    sendclothes(Host, peer);
                    bool drop = true;
                    int c = 0;
                    for (int i = 0; i < static_cast<int>(world->magplant.size()); i++)
                    {
                        if (world->magplant[i].id == pinfo(peer)->dropid && world->magplant[i].active == 1  && c == 0)
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
                            world->magplant[i].amount += amount;
                        }
                    }
                    if (drop)
                    {
                        DropItem(server, Host, peer, -1, xx, yy, pinfo(peer)->dropid, amount, 0);
                    }
                }
            }
        }
    }

    if (isRegister)
    {
        if (username.length() < 1 || password.length() < 1)
        {
            return;
        }
        if (username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
        {
            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGet a GrowID``|left|32|\n\nadd_spacer|small|\n\nadd_spacer|small|\nadd_text_input|username|GrowID||30|\nadd_text_input|password|Password||100|\nadd_text_input|email|Email||30|\nend_dialog|register|Cancel|Get My GrowID!|\n");
            return;
        }
        if (!ACCOUNT_EXIST(username))
        {
            Add_ITEM(peer, 18, 200);
            Add_ITEM(peer, 32, 200);
            pinfo(peer)->username = username;
            pinfo(peer)->password = password;
            INSERT_ACCOUNT(username, password, email, serialize_player((Players *)peer->data));
            p.consoleMessage(peer, "`rYour account has been created.");
            p.sethasgrowid(peer, 1, username, password);
            enet_peer_disconnect_later(peer, 0);
        }
        else
        {
            p.dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGet a GrowID``|left|32|\n\nadd_spacer|small|\n\nadd_label|small|`oThis `4Growid `oexist already `2please use other one``|left|32|\n\nadd_spacer|small|\nadd_text_input|username|GrowID||30|\nadd_text_input|password|Password||100|\nadd_text_input|email|Email||30|\nend_dialog|register|Cancel|Get My GrowID!|\n");
        }
    }
    if (isFindDialog && btn.substr(0, 4) == "tool")
    {
        if (is_digits(btn.substr(4, btn.length() - 4)))
        {
            int id = atoi(btn.substr(4, btn.length() - 4).c_str());
            int idx = -1;
            if (id < 0 || id > static_cast<int>(itemDefs.size()))
            {
                return;
            }
            for (int i = 0; i < static_cast<int>(cant_take.size()); i++)
            {
                if (cant_take[i] == id)
                {
                    idx = i;
                }
            }
            if (idx == -1)
            {
                int o = -1;
                for (int i = 0; i < static_cast<int>(pinfo(peer)->inv.size()); i++)
                {
                    if (pinfo(peer)->inv[i].itemID == id)
                    {
                        o = i;
                        pinfo(peer)->inv[i].itemCount = 200;
                        sendInventory(peer);
                    }
                }
                if (o == -1)
                {
                    Add_ITEM(peer, id, 200, true);
                }
            }
            else
            {
                p.consoleMessage(peer, "`rYou can purchase " + itemDefs[id].name + " from the store");
            }
        }
    }
    else if (isFindDialog)
    {
        std::string listFull = "";
        std::string listMiddle = "";
        for (char c : itemFind)
        {
            if (c < 0x20 || c > 0x7A)
            {
                return;
            }
        }
        if (itemFind.length() >= 3)
        {
            boost::to_lower(itemFind);
            int count = 0;
            for (int i = 0; i < static_cast<int>(itemDefs.size()); i++)
            {
                boost::to_lower(itemDefs[i].name);
                if (itemDefs[i].name.find(itemFind) != std::string::npos && (itemDefs[i].id % 2 == 0))
                {
                    count++;
                    listMiddle += "add_button_with_icon|tool" + std::to_string(itemDefs[i].id) + "|`$" + itemDefs[i].name + "``|left|" + std::to_string(itemDefs[i].id) + "||\n";
                }
            }
            if (count != 0)
            {
                p.dialog(peer, "add_label_with_icon|big|`wFound item : " + itemFind + "``|left|6016|\nadd_spacer|small|\nadd_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||20|\nend_dialog|findid|Cancel|Find the item!|\nadd_spacer|big|\n" + listMiddle + "add_quick_exit|\n");
            }
            else
            {
                p.dialog(peer, "add_label_with_icon|big|`wFind item: " + itemFind + "``|left|206|\nadd_spacer|small|\n" + listFull + "add_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\n");
            }
        }
        else
        {
            listFull = "add_textbox|`4Word is less then 3 letters!``|\nadd_spacer|small|\n";
            p.dialog(peer, "add_label_with_icon|big|`wFind item: " + itemFind + "``|left|206|\nadd_spacer|small|\n" + listFull + "add_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\n");
        }
    }
}