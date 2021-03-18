#include <iostream>
#include "enet/include/enet/enet.h"
#include "player.cpp"
#include <boost/algorithm/string.hpp>
#include "Itemsbuilder.cpp"
#include "worlds.cpp"
#include "guilds.cpp"

bool ACCOUNT_EXIST(std::string growid);
void INSERT_ACCOUNT(std::string growid, std::string pass, std::stringstream blob);
bool GUILD_EXIST(std::string guild);
std::vector<string> explode(const string &delimiter, const string &str);

bool is_digits(const std::string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

void Dialog_Handler(std::string cch, ENetPeer *peer, ENetHost *server)
{
    pinfo(peer)->usingDialog = false;
    std::stringstream ss(cch);
    std::string to;
    std::string username = "", password = "", itemFind = "", btn = "", g_name = "", g_state = "";
    bool isRegister = false, isFindDialog = false, isGuildDialog = false;
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

        //now to change the World Lock to Guild Lock
        int x = 0;
        int y = 0;
        Worlds *wld = getPlyersWorld(peer);
        for (int j = 0; j < wld->width * wld->height; j++)
        {
            if (itemDefs[wld->items[j].foreground].blockType == BlockTypes::LOCK)
            {
                x = (j % wld->width);
                y = (j / wld->width);
            }
        }
        wld->items[x + (y * wld->width)].foreground = 5814;
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