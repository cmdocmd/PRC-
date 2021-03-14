#include <iostream>
#include "enet/include/enet/enet.h"
#include "player.cpp"
#include "GamePacketBuilder.cpp"

using std::string;
std::vector<string> explode(const string &delimiter, const string &str);

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