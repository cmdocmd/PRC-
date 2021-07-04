#include <iostream>
#include "packets.h"
#include "players.h"
#include "items.h"

void Packets::consoleMessage(ENetPeer *peer, std::string message)
{
    GamePacketBuilder()
        .appendString("OnConsoleMessage")
        .appendString(message)
        .build()
        .send(peer);
}
void Packets::dialog(ENetPeer *peer, std::string message)
{
    pinfo(peer)->usingDialog = true;
    GamePacketBuilder()
        .appendString("OnDialogRequest")
        .appendString(message)
        .build()
        .send(peer);
}
void Packets::onSpawn(ENetPeer *peer, std::string message)
{
    GamePacketBuilder()
        .appendString("OnSpawn")
        .appendString(message)
        .build()
        .send(peer);
}
void Packets::requestWorldSelectMenu(ENetPeer *peer, std::string message)
{
    GamePacketBuilder()
        .appendString("OnRequestWorldSelectMenu")
        .appendString(message)
        .build()
        .send(peer);
}

void Packets::storeRequest(ENetPeer *peer, std::string message)
{
    GamePacketBuilder()
        .appendString("OnStoreRequest")
        .appendString(message)
        .build()
        .send(peer);
}

void Packets::storeResult(ENetPeer *peer, std::string message)
{
    GamePacketBuilder()
        .appendString("OnStorePurchaseResult")
        .appendString(message)
        .build()
        .send(peer);
}

void Packets::onsupermain(ENetPeer *peer, int hash, std::string a, std::string b, int hash2)
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

void Packets::sethasgrowid(ENetPeer *peer, bool inaccount, std::string tankidname, std::string tankidpass)
{
    GamePacketBuilder()
        .appendString("SetHasGrowID")
        .appendInt(inaccount)
        .appendString(tankidname)
        .appendString(tankidpass)
        .build()
        .send(peer);
}

void Packets::onsetcloth(ENetPeer *peer)
{

    GamePacket p3 = GamePacketBuilder()
                        .appendString("OnSetClothing")
                        .appendFloat(pinfo(peer)->hair, pinfo(peer)->shirt, pinfo(peer)->pants)
                        .appendFloat(pinfo(peer)->feet, pinfo(peer)->face, pinfo(peer)->hand)
                        .appendFloat(pinfo(peer)->back, pinfo(peer)->mask, (pinfo(peer))->neck)
                        .appendIntx(pinfo(peer)->skinColor)
                        .appendFloat(pinfo(peer)->ances, 0.0f, 0.0f)
                        .build();
    memcpy(p3.data + 8, &((pinfo(peer))->netID), 4);
    p3.send(peer);
}

void Packets::onoverlay(ENetPeer *peer, std::string message)
{
    GamePacketBuilder()
        .appendString("OnTextOverlay")
        .appendString(message)
        .build()
        .send(peer);
}

void Packets::onsetpos(ENetPeer *peer, int x, int y)
{
    pinfo(peer)->usingDoor = true;
    GamePacket p2 = GamePacketBuilder()
                        .appendString("OnSetPos")
                        .appendFloat(x, y)
                        .build();
    memcpy(p2.data + 8, &(pinfo(peer)->netID), 4);
    p2.send(peer);
}

void Packets::onsetbux(ENetPeer *peer)
{
    GamePacketBuilder()
        .appendString("OnSetBux")
        .appendInt(pinfo(peer)->gem)
        .build()
        .send(peer);
}

void Packets::onaddnotification(ENetPeer *peer, std::string message, std::string music)
{
    GamePacketBuilder()
        .appendString("OnAddNotification")
        .appendString("interface/atomic_button.rttex")
        .appendString(message)
        .appendString(music)
        .appendInt(0)
        .build()
        .send(peer);
    //"audio/hub_open.wav"
}

void Packets::ontalkbubble(ENetPeer *peer, int netid, std::string message, int id)
{
    GamePacketBuilder()
        .appendString("OnTalkBubble")
        .appendIntx(netid)
        .appendString(message)
        .appendIntx(id)
        .build()
        .send(peer);
}

void Packets::onfreezestate(ENetPeer *peer, int state)
{
    GamePacket p3 = GamePacketBuilder()
                        .appendString("OnSetFreezeState")
                        .appendIntx(state)
                        .build();
    memcpy(p3.data + 8, &(pinfo(peer)->netID), 4);
    p3.send(peer);
}

void Packets::onforcetradeend(ENetPeer *peer)
{
    GamePacketBuilder()
        .appendString("OnForceTradeEnd")
        .build()
        .send(peer);
}

void Packets::onzoomcamera(ENetPeer *peer, int zoom)
{
    GamePacketBuilder()
        .appendString("OnZoomCamera")
        .appendInt(zoom)
        .build()
        .send(peer);
}

void Packets::onsetcurrentweather(ENetPeer *peer, int weather)
{
    GamePacketBuilder()
        .appendString("OnSetCurrentWeather")
        .appendInt(weather)
        .build()
        .send(peer);
}

void Packets::onfailedtoenterworld(ENetPeer *peer)
{
    GamePacketBuilder()
        .appendString("OnFailedToEnterWorld")
        .appendIntx(1)
        .build()
        .send(peer);
}

void Packets::setrespawnpos(ENetPeer *peer, int x, int y)
{
    GamePacket p3 = GamePacketBuilder()
                        .appendString("SetRespawnPos")
                        .appendInt(x + (y * 100))
                        .build();
    memcpy(p3.data + 8, &(pinfo(peer)->netID), 4);
    p3.send(peer);
}

void Packets::onkill(ENetPeer *peer)
{
    GamePacket p3 = GamePacketBuilder()
                        .appendString("OnKilled")
                        .build();
    memcpy(p3.data + 8, &(pinfo(peer)->netID), 4);
    p3.send(peer);
}

void Packets::onparticleeffect(ENetPeer *peer, int effect, int x, int y)
{
    GamePacketBuilder()
        .appendString("OnParticleEffect")
        .appendIntx(effect)
        .appendFloat(x, y)
        .build()
        .send(peer);
}

void Packets::ontradestatus(ENetPeer *peer, int netid, std::string s2, std::string offername, std::string box, bool trade)
{
    if (trade)
    {
        GamePacketBuilder()
            .appendString("OnTradeStatus")
            .appendInt(netid)
            .appendString(s2)
            .appendString(offername + "`o's offer.")
            .appendString(box)
            .build()
            .send(peer);
    }
    else
    {
        GamePacketBuilder()
            .appendString("OnTradeStatus")
            .appendInt(netid)
            .appendString(s2)
            .appendString("`wWaiting for " + offername)
            .appendString(box)
            .build()
            .send(peer);
    }
}

void Packets::onstarttrade(ENetPeer *peer, ENetPeer *peer2)
{
    GamePacketBuilder()
        .appendString("OnStartTrade")
        .appendString(pinfo(peer2)->username)
        .appendInt(pinfo(peer2)->netID)
        .build()
        .send(peer);
}

void Packets::onnamechanged(ENetPeer *peer, std::string name)
{
    GamePacket p3 = GamePacketBuilder()
                        .appendString("OnNameChanged")
                        .appendString("```0" + name)
                        .build();
    memcpy(p3.data + 8, &(pinfo(peer)->netID), 4);
    p3.send(peer);
}

void Packets::oninvis(ENetPeer *peer, int invis)
{
    GamePacket p0 = GamePacketBuilder()
                        .appendString("OnInvis")
                        .appendInt(invis)
                        .build();
    memcpy(p0.data + 8, &(pinfo(peer)->netID), 4);
    p0.send(peer);
}

void Packets::onremove(ENetPeer *peer, Players *data)
{
    GamePacketBuilder()
        .appendString("OnRemove")
        .appendString("netID|" + std::to_string(data->netID) + "\n")
        .build()
        .send(peer);
}

void Packets::sendaction(ENetPeer *peer, int netid, std::string action)
{
    GamePacket p2 = GamePacketBuilder()
                        .appendString("OnAction")
                        .appendString(action)
                        .build();
    memcpy(p2.data + 8, &netid, 4);
    p2.send(peer);
}

void Packets::refresh_data(ENetPeer *peer)
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
