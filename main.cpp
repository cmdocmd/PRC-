#include <iostream>
#include "player.cpp"
#include "enet/include/enet/enet.h"
#include "events.cpp"
#include "MySql.cpp"

using std::cout;
using std::endl;
using std::string;

ENetHost *server;

typedef unsigned char BYTE;
typedef unsigned char byte;
typedef unsigned char __int8;
typedef unsigned short __int16;
typedef unsigned int DWORD;

void LOAD_WORLDS()
{
    GET_WORLD(server, "CMD").info;
    GET_WORLD(server, "IMBUSY").info;
    GET_WORLD(server, "HADI").info;
    GET_WORLD(server, "SECRET").info;
}

void Start_ENet()
{
    printf("[ENet]  Starting...\n");
    enet_initialize();
    ENetAddress address;
    address.port = 17091;
    server = enet_host_create(&address, 1024, 10, 0, 0);
    if (server == NULL)
    {
        printf("Could not start ENet..");
        return;
    }
    server->checksum = enet_crc32;
    enet_host_compress_with_range_coder(server);
    printf("[ENet] Started successfly\n");
}

void Run()
{
    ENetEvent event;
    while (true)
    {
        while (enet_host_service(server, &event, 1000) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
            {
                cout << "connecting...\n";
                Events::Connect(event.peer, server);
                continue;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                cout << "receiving...\n";
                Events::Recieve(server, event.packet, event.peer, GetTextPointerFromPacket(event.packet));
                enet_packet_destroy(event.packet);
                continue;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                cout << "disconnecting...\n";
                Events::Disconnect(event.peer);
                continue;
            }
            case ENET_EVENT_TYPE_NONE:
            {
                continue;
            }
            }
        }
    }
}

int main()
{
    LOAD_WORLDS();
    cout << "RebornTM\n";
    Start_ENet();
    itemsbuild();
    Run();
}