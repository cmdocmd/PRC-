#include <iostream>
#include "enet/include/enet/enet.h"
#include <string.h>

typedef unsigned char BYTE;

using std::string;

struct GamePacket
{
private:
    GamePacket(GamePacket const &) = delete;
    GamePacket &operator=(GamePacket const &) = delete;

    GamePacket(int delay = 0, int NetID = -1)
    {
        int len = 61;
        BYTE *packetData = new BYTE[len];
        int PacketType = 1;
        int MessageType = 0x4;
        int CharState = 0x8;
        memset(packetData, 0, len);
        memcpy(packetData, &MessageType, 4);
        memcpy(packetData + 4, &PacketType, 4);
        memcpy(packetData + 8, &NetID, 4);
        memcpy(packetData + 16, &CharState, 4);
        memcpy(packetData + 24, &delay, 4);

        this->data = packetData;
        this->len = 61;
        this->indexes = 0;
    }

    friend class GamePacketBuilder;

public:
    BYTE *data;
    int len;
    int indexes;

    void send(ENetPeer *peer)
    {
        ENetPacket *packet = enet_packet_create(this->data,
                                                this->len,
                                                ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
    }

    GamePacket(GamePacket &&packet) noexcept
    {
        this->data = packet.data;
        this->len = packet.len;
        this->indexes = packet.indexes;

        packet.data = NULL;
        packet.len = 0;
        packet.indexes = 0;
    };

    GamePacket &operator=(GamePacket &&packet) noexcept
    {
        this->data = packet.data;
        this->len = packet.len;
        this->indexes = packet.indexes;

        packet.data = NULL;
        packet.len = 0;
        packet.indexes = 0;
        return *this;
    };

    ~GamePacket()
    {
        delete[] this->data;
    }
};

class GamePacketBuilder
{
private:
    GamePacket p;

public:
    GamePacketBuilder &appendFloat(float val)
    {
        BYTE *n = new BYTE[p.len + 2 + 4];
        memcpy(n, p.data, p.len);
        delete[] p.data;
        p.data = n;
        n[p.len] = p.indexes;
        n[p.len + 1] = 1;
        memcpy(n + p.len + 2, &val, 4);
        p.len = p.len + 2 + 4;
        p.indexes++;
        return *this;
    }

    GamePacketBuilder &appendFloat(float val, float val2)
    {
        BYTE *n = new BYTE[p.len + 2 + 8];
        memcpy(n, p.data, p.len);
        delete[] p.data;
        p.data = n;
        n[p.len] = p.indexes;
        n[p.len + 1] = 3;
        memcpy(n + p.len + 2, &val, 4);
        memcpy(n + p.len + 6, &val2, 4);
        p.len = p.len + 2 + 8;
        p.indexes++;
        return *this;
    }

    GamePacketBuilder &appendFloat(float val, float val2, float val3)
    {
        BYTE *n = new BYTE[p.len + 2 + 12];
        memcpy(n, p.data, p.len);
        delete[] p.data;
        p.data = n;
        n[p.len] = p.indexes;
        n[p.len + 1] = 4;
        memcpy(n + p.len + 2, &val, 4);
        memcpy(n + p.len + 6, &val2, 4);
        memcpy(n + p.len + 10, &val3, 4);
        p.len = p.len + 2 + 12;
        p.indexes++;
        return *this;
    }

    GamePacketBuilder &appendInt(int val)
    {
        BYTE *n = new BYTE[p.len + 2 + 4];
        memcpy(n, p.data, p.len);
        delete[] p.data;
        p.data = n;
        n[p.len] = p.indexes;
        n[p.len + 1] = 9;
        memcpy(n + p.len + 2, &val, 4);
        p.len = p.len + 2 + 4;
        p.indexes++;
        return *this;
    }

    GamePacketBuilder &appendIntx(int val)
    {
        BYTE *n = new BYTE[p.len + 2 + 4];
        memcpy(n, p.data, p.len);
        delete[] p.data;
        p.data = n;
        n[p.len] = p.indexes;
        n[p.len + 1] = 5;
        memcpy(n + p.len + 2, &val, 4);
        p.len = p.len + 2 + 4;
        p.indexes++;
        return *this;
    }

    GamePacketBuilder &appendString(string str)
    {
        BYTE *n = new BYTE[p.len + 2 + str.length() + 4];
        memcpy(n, p.data, p.len);
        delete[] p.data;
        p.data = n;
        n[p.len] = p.indexes;
        n[p.len + 1] = 2;
        int sLen = str.length();
        memcpy(n + p.len + 2, &sLen, 4);
        memcpy(n + p.len + 6, str.c_str(), sLen);
        p.len = p.len + 2 + str.length() + 4;
        p.indexes++;
        return *this;
    }

    GamePacket &&build()
    {
        BYTE *n = new BYTE[p.len + 1];
        memcpy(n, p.data, p.len);
        delete[] p.data;
        p.data = n;
        char zero = 0;
        memcpy(p.data + p.len, &zero, 1);
        p.len += 1;
        *(int *)(p.data + 56) = p.indexes;
        *(BYTE *)(p.data + 60) = p.indexes;
        return std::move(this->p);
    }
};

namespace Packets
{
    void onsupermain(ENetPeer *peer, int hash, string a, string b, int hash2)
    {
        GamePacketBuilder()
            .appendString("OnSuperMainStartAcceptLogonHrdxs47254722215a")
            .appendInt(hash)
            .appendString(a)
            .appendString(b)
            .appendString("cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster")
            .appendString("proto=96|choosemusic=audio/mp3/about_theme.mp3|active_holiday=0|server_tick=102144241|clash_active=1|drop_lavacheck_faster=1|isPayingUser=0|usingStoreNavigation=1|enableInventoryTab=1|bigBackpack=1|")
            .appendInt(hash2)
            .build()
            .send(peer);
    }
    void dialog(ENetPeer *peer, string message)
    {
        pinfo(peer)->usingDialog = true;
        GamePacketBuilder()
            .appendString("OnDialogRequest")
            .appendString(message)
            .build()
            .send(peer);
    }
    void consoleMessage(ENetPeer *peer, string message)
    {
        GamePacketBuilder()
            .appendString("OnConsoleMessage")
            .appendString(message)
            .build()
            .send(peer);
    }
    void sethasgrowid(ENetPeer *peer, bool inaccount, string tankidname, string tankidpass)
    {
        GamePacketBuilder()
            .appendString("SetHasGrowID")
            .appendInt(inaccount)
            .appendString(tankidname)
            .appendString(tankidpass)
            .build()
            .send(peer);
    }
    void refresh_data(ENetPeer *peer)
    {
        if (itemsDat != NULL)
        {
            ENetPacket *packet = enet_packet_create(itemsDat,
                                                    itemsDatSize + 60,
                                                    ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
            //pinfo(peer)->updatingdata = true;
            enet_peer_disconnect_later(peer, 0);
        }
    }
    void refresh_player(ENetPeer *peer)
    {
        if (playerTri != NULL)
        {
            ENetPacket *packet = enet_packet_create(playerTri,
                                                    playerSize + 60,
                                                    ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
            //pinfo(peer)->updatingdata = true;
            enet_peer_disconnect_later(peer, 0);
        }
    }
}