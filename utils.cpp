#include <iostream>
#include "enet/include/enet/enet.h"
#include <string.h>


int GetMessageTypeFromPacket(ENetPacket *packet)
{
    int result;

    if (packet->dataLength > 3u)
    {
        result = *(packet->data);
    }
    else
    {
        std::cout << "Bad packet length, ignoring message" << std::endl;
        result = 0;
    }
    return result;
}

int count_players(ENetHost *server, ENetPeer *peer)
{
    ENetPeer *currentPeer;
    int count = 0;
    for (currentPeer = server->peers;
         currentPeer < &server->peers[server->peerCount];
         ++currentPeer)
    {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
        {
            continue;
        }
        if (currentPeer->address.host == peer->address.host)
        {
            count++;
        }
    }
    return count;
}

void sendData(ENetHost *server, ENetPeer *peer, int num = 1, char *data = 0, int len = 0)
{
    ENetPacket *packet = enet_packet_create(0, len + 5, ENET_PACKET_FLAG_RELIABLE);
    memcpy(packet->data, &num, 4);
    if (data != NULL)
    {
        memcpy(packet->data + 4, data, len);
    }
    char zero = 0;
    memcpy(packet->data + 4 + len, &zero, 1);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(server);
}

char *GetTextPointerFromPacket(ENetPacket *packet)
{
    char zero = 0;
    memcpy(packet->data + packet->dataLength - 1, &zero, 1);
    return (char *)(packet->data + 4);
}

std::vector<string> explode(const string &delimiter, const string &str)
{
	std::vector<string> arr;

	int strleng = str.length();
	int delleng = delimiter.length();
	if (delleng == 0)
		return arr;//no change

	int i = 0;
	int k = 0;
	while (i < strleng)
	{
		int j = 0;
		while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
			j++;
		if (j == delleng)//found delimiter
		{
			arr.push_back(str.substr(k, i - k));
			i += delleng;
			k = i;
		}
		else
		{
			i++;
		}
	}
	arr.push_back(str.substr(k, i - k));
	return arr;
}