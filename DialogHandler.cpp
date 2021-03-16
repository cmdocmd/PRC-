#include <iostream>
#include "enet/include/enet/enet.h"
#include "player.cpp"
#include <boost/algorithm/string.hpp>
#include "Itemsbuilder.cpp"

bool ACCOUNT_EXIST(std::string growid);
void INSERT_ACCOUNT(std::string growid, std::string pass, std::stringstream blob);
std::vector<string> explode(const string &delimiter, const string &str);

bool is_digits(const std::string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

void Dialog_Handler(std::string cch, ENetPeer *peer)
{
    pinfo(peer)->usingDialog = false;
    std::stringstream ss(cch);
    std::string to;
    std::string username = "", password = "", itemFind = "", btn = "";
    bool isRegister = false, isFindDialog = false;
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
        }
    }
    if (isRegister)
    {
        if (username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
        {
            Packets::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGet a GrowID``|left|32|\n\nadd_spacer|small|\n\nadd_label|small|`4Growid `ocant contains symbols``|left|32|\n\nadd_spacer|small|\nadd_text_input|username|GrowID||30|\nadd_text_input|password|Password||100|\nend_dialog|register|Cancel|Get My GrowID!|\n");
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