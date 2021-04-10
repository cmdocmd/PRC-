#include <iostream>
#include "enet/include/enet/enet.h"
#include "player.cpp"
#include <boost/algorithm/string.hpp>
#include "Itemsbuilder.cpp"
#include "worlds.cpp"
#include "guilds.cpp"
#include "visuals.cpp"

bool ACCOUNT_EXIST(std::string growid);
bool GUILD_EXIST(std::string guild);
void INSERT_ACCOUNT(std::string growid, std::string pass, std::stringstream blob);
void INSERT_GUILD(std::string name, std::stringstream blob);
void UPDATE(std::stringstream str, std::string growid);
std::istream *PLAYER_DATA(std::string growid);
std::vector<string> explode(const string &delimiter, const string &str);

bool is_digits(const std::string &str)
{
    if (str == "")
    {
        return false;
    }
    return str.find_first_not_of("0123456789") == std::string::npos;
}

void Dialog_Handler(std::string cch, ENetPeer *peer, ENetHost *server)
{
    pinfo(peer)->usingDialog = false;
    std::stringstream ss(cch);
    std::string to;
    std::string username = "", password = "", itemFind = "", btn = "", g_name = "", g_state = "", d_count = "", s_text = "", b_days = "", b_hours = "", b_mint = "", m_days = "", m_hours = "", m_mint = "", c_days = "", c_hours = "", c_mint = "", p_name = "", dest = "", label = "", doorid = "", iop = "";
    bool isRegister = false, isFindDialog = false, isGuildDialog = false, isDropDialog = false, isSign = false, isBan = false, isMute = false, isCurse = false, isDoor = false;
    ;
    while (std::getline(ss, to, '\n'))
    {
        std::vector<string> ex = explode("|", to);
        if (ex.size() == 2)
        {
            if (ex[0] == "buttonClicked")
            {
                btn = ex[1];
            }
            if (ex[0] == "dialog_name")
            {
                string dialog = ex[1];
                if (dialog == "register")
                {
                    isRegister = true;
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
                if (dialog.substr(0, 7) == "punish-")
                {
                    std::vector<string> ex = explode("-", dialog);
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
            }
            if (isFindDialog)
            {
                if (ex[0] == "item")
                {
                    itemFind = ex[1];
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
    if (btn == "create_guild")
    {
        Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild``|left|5814|\nadd_textbox|`oWelcome to Grow Guilds where you can create a Guild! With a Guild you can complete in Guild Leaderboards to earn rewards and level up the Guild to add more members.``|\nadd_spacer|small|\nadd_textbox|`oYou will be charged: `6100,000 `oGems.``|\nadd_spacer|small|\nadd_textbox|`8Caution`o: A guild can only be created in a world owned by you and locked with a `5World Lock`o!``|\nadd_button|guild_create|`oCreate a Guild``|left|||\nadd_button|back_social|`oBack``|left|||\nend_dialog||Close||");
    }
    if (btn == "guild_create")
    {
        Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation``|left|5814|\nadd_text_input|g-name|Guild Name:||18|\nadd_text_input|g-state|Guild Statement:||30|\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.``|\nadd_textbox|`8Remember`o: A guild can only be created in a world owned by you and locked with a `5World Lock`o!``|\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!``|\nadd_button|ok-guild|`oCreate Guild``|left|||\nend_dialog|guild|Close||");
    }
    if (btn == "back_social")
    {
        Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wSocial Portal``|left|1366|\n\nadd_spacer|small|\nadd_button|friends|`wShow Friends``|left|||\nadd_button|create_guild|`wCreate Guild``|left|||\nend_dialog||OK||");
    }
    if (btn.substr(0, 6) == "guild-")
    {
        std::vector<string> ex = explode("-", btn);
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
    }
    if (isDoor)
    {
        Worlds *world = getPlyersWorld(peer);
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
            pushdoor(peer, dest, label, doorid, iop);
        }
        if (label == "")
            label = "...";
        ENetPeer *currentPeer;
        for (currentPeer = server->peers;
             currentPeer < &server->peers[server->peerCount];
             ++currentPeer)
        {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                continue;
            if (isHere(peer, currentPeer))
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
    }
    if (isBan)
    {
        bool online = false;
        ENetPeer *currentPeer;
        ENetPeer *peer2;
        for (currentPeer = server->peers;
             currentPeer < &server->peers[server->peerCount];
             ++currentPeer)
        {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                continue;
            Packets::consoleMessage(currentPeer, "`#** `$The Ancients `ohave used `4Ban `oon `w" + p_name + "`o! `#**");
            sendmusic(currentPeer, "beep");
            if (pinfo(currentPeer)->adminLevel >= 666)
            {
                Packets::consoleMessage(peer, "`r[MODS LOGS] `1" + pinfo(peer)->name + " `$have `4ban " + p_name);
            }
            if (pinfo(currentPeer)->username == p_name)
            {
                online = true;
                peer2 = currentPeer;
            }
        }
        if (b_days != "0" && b_hours == "0" && b_mint == "0")
        {
            if (is_digits(b_days))
            {
                if (online)
                {
                    pinfo(peer2)->ban = time(NULL);
                    int days = std::stoi(b_days) * 24 * 60 * 60;
                    pinfo(peer2)->bantime = days;
                    Packets::onaddnotification(peer2, "`0Warning from `4System`0: You've been `4BANNED `0from Private Server for " + b_days + " days.");
                    enet_peer_disconnect_later(peer2, 0);
                }
                else
                {
                    std::istream *blobdata = PLAYER_DATA(p_name);
                    if (blobdata != NULL)
                    {
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
                        ply->ban = time(NULL);
                        int days = std::stoi(b_days) * 24 * 60 * 60;
                        ply->bantime = days;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
            }
        }
        if (b_days == "0" && b_hours != "0" && b_mint == "0")
        {
            if (is_digits(b_hours))
            {
                if (online)
                {
                    pinfo(peer2)->ban = time(NULL);
                    int hours = std::stoi(b_hours) * 60 * 60;
                    pinfo(peer2)->bantime = hours;
                    Packets::onaddnotification(peer2, "`0Warning from `4System`0: You've been `4BANNED `0from Private Server for " + b_hours + " Hours.");
                    enet_peer_disconnect_later(peer2, 0);
                }
                else
                {
                    std::istream *blobdata = PLAYER_DATA(p_name);
                    if (blobdata != NULL)
                    {
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
                        ply->ban = time(NULL);
                        int hours = std::stoi(b_hours) * 60 * 60;
                        ply->bantime = hours;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
            }
        }
        if (b_days == "0" && b_hours == "0" && b_mint != "0")
        {
            if (is_digits(b_mint))
            {
                if (online)
                {
                    pinfo(peer2)->ban = time(NULL);
                    int mint = std::stoi(b_mint) * 60;
                    pinfo(peer2)->bantime = mint;
                    Packets::onaddnotification(peer2, "`0Warning from `4System`0: You've been `4BANNED `0from Private Server for " + b_mint + " Minutes.");
                    enet_peer_disconnect_later(peer2, 0);
                }
                else
                {
                    std::istream *blobdata = PLAYER_DATA(p_name);
                    if (blobdata != NULL)
                    {
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
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
        bool online = false;
        ENetPeer *currentPeer;
        ENetPeer *peer2;
        for (currentPeer = server->peers;
             currentPeer < &server->peers[server->peerCount];
             ++currentPeer)
        {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                continue;
            Packets::consoleMessage(currentPeer, "`#** `$The Ancients `ohave used `#Mute `oon `w" + p_name + "`o! `#**");
            sendmusic(currentPeer, "beep");
            if (pinfo(currentPeer)->adminLevel >= 666)
            {
                Packets::consoleMessage(peer, "`r[MODS LOGS] `1" + pinfo(peer)->name + " `$have `4mute " + p_name);
            }
            if (pinfo(currentPeer)->username == p_name)
            {
                online = true;
                peer2 = currentPeer;
            }
        }
        if (m_days != "0" && m_hours == "0" && m_mint == "0")
        {
            if (is_digits(m_days))
            {
                if (online)
                {
                    pinfo(peer2)->mute = time(NULL);
                    int days = std::stoi(m_days) * 24 * 60 * 60;
                    pinfo(peer2)->mutetime = days;
                    Packets::onaddnotification(peer2, "`0Warning from `4System`0: You've been `#MUTED `0from Private Server for " + m_days + " days.");
                }
                else
                {
                    std::istream *blobdata = PLAYER_DATA(p_name);
                    if (blobdata != NULL)
                    {
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
                        ply->mute = time(NULL);
                        int days = std::stoi(m_days) * 24 * 60 * 60;
                        ply->mutetime = days;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
            }
        }
        if (m_days == "0" && m_hours != "0" && m_mint == "0")
        {
            if (is_digits(m_hours))
            {
                if (online)
                {
                    pinfo(peer2)->mute = time(NULL);
                    int hours = std::stoi(m_hours) * 60 * 60;
                    pinfo(peer2)->mutetime = hours;
                    Packets::onaddnotification(peer2, "`0Warning from `4System`0: You've been `#MUTED `0from Private Server for " + m_hours + " Hours.");
                }
                else
                {
                    std::istream *blobdata = PLAYER_DATA(p_name);
                    if (blobdata != NULL)
                    {
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
                        ply->mute = time(NULL);
                        int hours = std::stoi(m_hours) * 60 * 60;
                        ply->mutetime = hours;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
            }
        }
        if (m_days == "0" && m_hours == "0" && m_mint != "0")
        {
            if (is_digits(m_mint))
            {
                if (online)
                {
                    pinfo(peer2)->mute = time(NULL);
                    int mint = std::stoi(m_mint) * 60;
                    pinfo(peer2)->mutetime = mint;
                    Packets::onaddnotification(peer2, "`0Warning from `4System`0: You've been `4BANNED `0from Private Server for " + m_mint + " Minutes.");
                }
                else
                {
                    std::istream *blobdata = PLAYER_DATA(p_name);
                    if (blobdata != NULL)
                    {
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
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
        bool online = false;
        ENetPeer *currentPeer;
        ENetPeer *peer2;
        for (currentPeer = server->peers;
             currentPeer < &server->peers[server->peerCount];
             ++currentPeer)
        {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                continue;
            Packets::consoleMessage(currentPeer, "`#** `$The Ancients `ohave used `bCurse `oon `w" + p_name + "`o! `#**");
            sendmusic(currentPeer, "beep");
            if (pinfo(currentPeer)->adminLevel >= 666)
            {
                Packets::consoleMessage(peer, "`r[MODS LOGS] `1" + pinfo(peer)->name + " `$have `4ban " + p_name);
            }
            if (pinfo(currentPeer)->username == p_name)
            {
                online = true;
                peer2 = currentPeer;
            }
        }
        if (c_days != "0" && c_hours == "0" && c_mint == "0")
        {
            if (is_digits(c_days))
            {
                if (online)
                {
                    pinfo(peer2)->curse = time(NULL);
                    int days = std::stoi(c_days) * 24 * 60 * 60;
                    pinfo(peer2)->cursetime = days;
                    Packets::onaddnotification(peer2, "`0Warning from `4System`0: You've been `bCURSED `0from Private Server for " + c_days + " days.");
                    sendPlayerLeave(server, peer2, pinfo(peer2));
                    joinWorld(server, peer2, "HELL", 0, 0);
                }
                else
                {
                    std::istream *blobdata = PLAYER_DATA(p_name);
                    if (blobdata != NULL)
                    {
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
                        ply->curse = time(NULL);
                        int days = std::stoi(c_days) * 24 * 60 * 60;
                        ply->cursetime = days;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
            }
        }
        if (c_days == "0" && c_hours != "0" && c_mint == "0")
        {
            if (is_digits(c_hours))
            {
                if (online)
                {
                    pinfo(peer2)->curse = time(NULL);
                    int hours = std::stoi(c_hours) * 60 * 60;
                    pinfo(peer2)->cursetime = hours;
                    Packets::onaddnotification(peer2, "`0Warning from `4System`0: You've been `bCURSED `0from Private Server for " + c_hours + " Hours.");
                    sendPlayerLeave(server, peer2, pinfo(peer2));
                    joinWorld(server, peer2, "HELL", 0, 0);
                }
                else
                {
                    std::istream *blobdata = PLAYER_DATA(p_name);
                    if (blobdata != NULL)
                    {
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
                        ply->curse = time(NULL);
                        int hours = std::stoi(c_hours) * 60 * 60;
                        ply->cursetime = hours;
                        UPDATE(serialize_player(ply), p_name);
                    }
                }
            }
        }
        if (c_days == "0" && c_hours == "0" && c_mint != "0")
        {
            if (is_digits(c_mint))
            {
                if (online)
                {
                    pinfo(peer2)->curse = time(NULL);
                    int mint = std::stoi(c_mint) * 60;
                    pinfo(peer2)->cursetime = mint;
                    Packets::onaddnotification(peer2, "`0Warning from `4System`0: You've been `bCURSED `0from Private Server for " + c_mint + " Minutes.");
                    sendPlayerLeave(server, peer2, pinfo(peer2));
                }
                else
                {
                    std::istream *blobdata = PLAYER_DATA(p_name);
                    if (blobdata != NULL)
                    {
                        player *ply;
                        {
                            boost::archive::binary_iarchive ia(*blobdata);
                            ia >> ply;
                        }
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
        Worlds *world = getPlyersWorld(peer);
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
            pushsign(peer, s_text);
        }
        ENetPeer *currentPeer;
        for (currentPeer = server->peers;
             currentPeer < &server->peers[server->peerCount];
             ++currentPeer)
        {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                continue;
            if (isHere(peer, currentPeer))
            {
                updatesign(currentPeer, fg, bg, x, y, s_text);
            }
        }
    }
    if (isGuildDialog)
    {
        Worlds *wld = getPlyersWorld(peer);
        if (wld == NULL)
        {
            return;
        }
        if (g_name.length() < 1)
        {
            return;
        }
        if (g_name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
        {
            Packets::consoleMessage(peer, "`4Guild Name`o cant contain symbols");
            return;
        }
        if (g_state.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
        {
            Packets::consoleMessage(peer, "`4Guild Statement`o cant contain symbols");
            return;
        }
        if (wld->owner != pinfo(peer)->username)
        {
            Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation``|left|5814|\nadd_text_input|g-name|Guild Name:|" + g_name + "|18|\nadd_text_input|g-state|Guild Statement:|" + g_state + "|30|\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.``|\nadd_textbox|`8Remember`o: A guild can only be created in a world owned by you and locked with a `5World Lock`o!``|\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!``|\nadd_button|ok-guild|`oCreate Guild``|left|||\nend_dialog|guild|Close||");
            return;
        }
        if (!GUILD_EXIST(g_name))
        {
            Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild``|left|5814|\nadd_textbox|`1Guild Name: `o" + g_name + "``|\nadd_textbox|`1Guild Statement: `o" + g_state + "``|\nadd_spacer|small|\nadd_textbox|`oCost `4100000 Gems``|\nadd_spacer|small|\nadd_button|guild-" + g_name + "-" + g_state + "|`oCreate Guild``|left|||\nend_dialog||Close||");
        }
        else
        {
            Packets::consoleMessage(peer, "`4Other Guild have this name");
        }
    }
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
                    int am = pinfo(peer)->inv[idx].itemCount;
                    int new_value = am - amount;
                    if (new_value < 0)
                    {
                        return;
                    }
                    else if (new_value == 0)
                    {
                        pinfo(peer)->inv.erase(pinfo(peer)->inv.begin() + idx);
                        remove_clothes_if_wearing(peer, server, pinfo(peer)->dropid);
                    }
                    else
                    {
                        pinfo(peer)->inv[idx].itemCount = new_value;
                    }
                    float xx = pinfo(peer)->x + (32 * (pinfo(peer)->isRotatedLeft ? -1 : 1));
                    float yy = pinfo(peer)->y;
                    sendInventory(peer);
                    Packets::sendclothes(peer, server);
                    DropItem(server, peer, -1, xx, yy, pinfo(peer)->dropid, amount, 0);
                }
            }
        }
    }
    if (isRegister)
    {
        if (username.length() < 1)
        {
            return;
        }
        if (username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
        {
            Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGet a GrowID``|left|32|\n\nadd_spacer|small|\n\nadd_spacer|small|\nadd_text_input|username|GrowID||30|\nadd_text_input|password|Password||100|\nend_dialog|register|Cancel|Get My GrowID!|\n");
            return;
        }
        if (!ACCOUNT_EXIST(username))
        {
            Add_ITEM(peer, 18, 200);
            Add_ITEM(peer, 32, 200);
            pinfo(peer)->username = username;
            pinfo(peer)->password = password;
            pinfo(peer)->name = username;
            INSERT_ACCOUNT(username, password, serialize_player((player *)peer->data));
            Packets::consoleMessage(peer, "`rYour account has been created.");
            Packets::sethasgrowid(peer, 1, username, password);
            enet_peer_disconnect_later(peer, 0);
        }
        else
        {
            Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGet a GrowID``|left|32|\n\nadd_spacer|small|\n\nadd_label|small|`oThis `4Growid `oexist already `2please use other one``|left|32|\n\nadd_spacer|small|\nadd_text_input|username|GrowID||30|\nadd_text_input|password|Password||100|\nend_dialog|register|Cancel|Get My GrowID!|\n");
        }
    }
    if (isFindDialog && btn.substr(0, 4) == "tool")
    {
        if (is_digits(btn.substr(4, btn.length() - 4)))
        {
            int id = atoi(btn.substr(4, btn.length() - 4).c_str());
            if (id < 0 || id > static_cast<int>(itemDefs.size()))
            {
                return;
            }
            Add_ITEM(peer, id, 200, true);
        }
    }
    else if (isFindDialog)
    {
        string listFull = "";
        string listMiddle = "";
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
                Packets::dialog(peer, "add_label_with_icon|big|`wFound item : " + itemFind + "``|left|6016|\nadd_spacer|small|\nadd_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||20|\nend_dialog|findid|Cancel|Find the item!|\nadd_spacer|big|\n" + listMiddle + "add_quick_exit|\n");
            }
            else
            {
                Packets::dialog(peer, "add_label_with_icon|big|`wFind item: " + itemFind + "``|left|206|\nadd_spacer|small|\n" + listFull + "add_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\n");
            }
        }
        else
        {
            listFull = "add_textbox|`4Word is less then 3 letters!``|\nadd_spacer|small|\n";
            Packets::dialog(peer, "add_label_with_icon|big|`wFind item: " + itemFind + "``|left|206|\nadd_spacer|small|\n" + listFull + "add_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\n");
        }
    }
}