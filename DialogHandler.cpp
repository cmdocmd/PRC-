#include <iostream>
#include "enet/include/enet/enet.h"
#include "player.cpp"

bool ACCOUNT_EXIST(std::string growid);
void INSERT_ACCOUNT(std::string growid, std::string pass, std::stringstream blob);
std::vector<string> explode(const string &delimiter, const string &str);

void Dialog_Handler(std::string cch, ENetPeer *peer)
{
    pinfo(peer)->usingDialog = false;
    std::stringstream ss(cch);
    std::string to;
    std::string username = "", password = "";
    bool isRegister = false;
    while (std::getline(ss, to, '\n'))
    {
        std::vector<string> ex = explode("|", to);
        if (ex.size() == 2)
        {
            if (ex[0] == "dialog_name")
            {
                string dialog = ex[1];
                if (dialog == "register")
                {
                    isRegister = true;
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
}