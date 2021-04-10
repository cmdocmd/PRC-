#pragma once
#include <iostream>
#include <vector>
#include "enet/include/enet/enet.h"
#include <string.h>
#include "GamePacketBuilder.cpp"

using std::string;

typedef unsigned char BYTE;

void updatesign(ENetPeer *peer, int fg, int bg, int x, int y, string text)
{
    int hmm = 8, wot = text.length(), lol = 0, wut = 5;
    int yeh = hmm + 3 + 1, idk = 15 + wot, lmao = -1, yey = 2; //idk = text_len + 15, wut = type(?), wot = text_len, yey = len of text_len
    int ok = 52 + idk;
    int kek = ok + 4, yup = ok - 8 - idk;
    int thonk = 4, magic = 56, wew = ok + 5 + 4;
    int wow = magic + 4 + 5;
    std::vector<BYTE> data(kek);
    ENetPacket *p = enet_packet_create(0, wew, ENET_PACKET_FLAG_RELIABLE);
    memcpy(&data[0], &wut, thonk);
    memcpy(&data[yeh], &hmm, thonk); //read discord
    memcpy(&data[yup], &x, 4);
    memcpy(&data[yup + 4], &y, 4);
    memcpy(&data[4 + yup + 4], &idk, thonk);
    memcpy(&data[magic], &fg, yey);
    memcpy(&data[magic + 2], &bg, yey); //p100 fix by the one and only lapada
    memcpy(&data[thonk + magic], &lol, thonk);
    memcpy(&data[magic + 4 + thonk], &yey, 1);
    memcpy(&data[wow], &wot, yey);               //data + wow = text_len
    memcpy(&data[yey + wow], text.c_str(), wot); //data + text_len_len + text_len_offs = text
    memcpy(&data[ok], &lmao, thonk);             //end ?
    memcpy(p->data, &thonk, thonk);
    memcpy((char *)p->data + thonk, &data[0], kek); //kek = data_len

    enet_peer_send(peer, 0, p);
}

	void updatedoor(ENetPeer* peer, int foreground, int background, int x, int y, string text, bool islocked)
	{
		int hmm = 8;
		int text_len = text.length();
		int lol = 0;
		int wut = 5;
		int yeh = hmm + 3 + 1;
		int idk = 15 + text_len;
		int is_locked = 0;
		if (islocked) is_locked = -1;
		int bubble_type = 1;
		int ok = 52 + idk;
		int kek = ok + 4;
		int yup = ok - 8 - idk;
		int four = 4;
		int magic = 56;
		int wew = ok + 5 + 4;
		int wow = magic + 4 + 5;

        std::vector<BYTE> data(kek);
		ENetPacket* p = enet_packet_create(0, wew, ENET_PACKET_FLAG_RELIABLE);
		for (int i = 0; i < kek; i++) data[i] = 0;
		memcpy(&data[0], &wut, four); //4
		memcpy(&data[yeh], &hmm, four); //8
		memcpy(&data[yup], &x, 4); //12
		memcpy(&data[yup + 4], &y, 4); //16
		memcpy(&data[4 + yup + 4], &idk, four); //20
		memcpy(&data[magic], &foreground, 2); //22
		memcpy(&data[magic + 2], &background, 2);
		memcpy(&data[four + magic], &lol, four); //26
		memcpy(&data[magic + 4 + four], &bubble_type, 1); //27
		memcpy(&data[wow], &text_len, 2); //data + wow = text_len, pos 29
		memcpy(&data[2 + wow], text.c_str(), text_len); //data + text_len_len + text_len_offs = text, pos 94
		memcpy(&data[ok], &is_locked, four); //98
		memcpy(p->data, &four, four); //4
		memcpy((char*)p->data + four, &data[0], kek); //kek = data_len
		enet_peer_send(peer, 0, p);
	}