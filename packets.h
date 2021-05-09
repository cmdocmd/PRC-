#pragma once

#include <iostream>
#include <string.h>

#include "enet/enet.h"
#include "players.h"

typedef unsigned char BYTE;

struct GamePacket
{
private:
	GamePacket(GamePacket const &) = delete;
	GamePacket &operator=(GamePacket const &) = delete;

	GamePacket()
	{
		int len = 61;
		BYTE *packetData = new BYTE[len];
		int one = 1;
		int four = 4;
		uint32_t a = 4294967295;
		int eight = 8;
		memset(packetData, 0, len);
		memcpy(packetData, &four, 4);
		memcpy(packetData + 4, &one, 4);
		memcpy(packetData + 8, &a, 4);
		memcpy(packetData + 16, &eight, 4);
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

	GamePacketBuilder &appendString(std::string str)
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

class Packets
{
public:
	void consoleMessage(ENetPeer *peer, std::string message);
	void dialog(ENetPeer *peer, std::string message);
	void onSpawn(ENetPeer *peer, std::string message);
	void requestWorldSelectMenu(ENetPeer *peer, std::string message);
	void storeRequest(ENetPeer *peer, std::string message);
	void storeResult(ENetPeer *peer, std::string message);
	void onsupermain(ENetPeer *peer, int hash, std::string a, std::string b, int hash2);
	void sethasgrowid(ENetPeer *peer, bool inaccount, std::string tankidname, std::string tankidpass);
	void onsetcloth(ENetPeer *peer);
	void onoverlay(ENetPeer *peer, std::string message);
	void onsetpos(ENetPeer *peer, int x, int y);
	void onsetbux(ENetPeer *peer);
	void onaddnotification(ENetPeer *peer, std::string message);
	void ontalkbubble(ENetPeer *peer, int netid, std::string message);
	void onfreezestate(ENetPeer *peer, int state);
	void onforcetradeend(ENetPeer *peer);
	void onzoomcamera(ENetPeer *peer, int zoom);
	void onsetcurrentweather(ENetPeer *peer, int weather);
	void onfailedtoenterworld(ENetPeer *peer);
	void setrespawnpos(ENetPeer *peer, int x, int y);
	void onkill(ENetPeer *peer);
	void ontradestatus(ENetPeer *peer, int netid, std::string s2, std::string offername, std::string box, bool trade);
	void onstarttrade(ENetPeer *peer, ENetPeer *peer2);
	void onnamechanged(ENetPeer *peer, std::string name);
	void oninvis(ENetPeer *peer, int invis);
	void onremove(ENetPeer *peer, Players *data);
	void sendaction(ENetPeer *peer, int netid, std::string action);
	void refresh_data(ENetPeer *peer);
};