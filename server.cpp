#include <iostream>

#include "server.h"
#include "enet/enet.h"
#include "utils.h"
#include "events.h"
#include "mysql.h"
#include "packets.h"
#include "items.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include <boost/algorithm/string.hpp>


ENetHost *Host;
time_t old = time(NULL);

Server::Server(int port)
{
    printf("[ENet]  Starting...\n");
    enet_initialize();
    ENetAddress address;
    enet_address_set_host(&address, "0.0.0.0");
    address.port = port;
    Host = enet_host_create(&address, 1024, 10, 0, 0);
    if (Host == NULL)
    {
        printf("Could not start ENet..");
        return;
    }
    Host->checksum = enet_crc32;
    enet_host_compress_with_range_coder(Host);
    printf("[ENet] Started successfly\n");
    itemsbuild();
}

std::stringstream Server::serialize_world(Worlds const &world)
{
    std::stringstream str;
    {
        boost::archive::binary_oarchive oa(str);
        oa << world;
    }
    return str;
}

void Server::Flush_Worlds(Worlds world, Server *server)
{
    if (WORLD_EXIST(world.Name))
    {
        UPDATE_WORLD(server->serialize_world(world), world.Name);
    }
    else
    {
        INSERT_WORLD(world.Name, server->serialize_world(world));
    }
}

void Server::Save_Worlds(Server *server)
{
    for (int i = 0; i < static_cast<int>(server->m_worlds.size()); i++)
    {
        Flush_Worlds(server->m_worlds[i], server);
        server->m_worlds.erase(server->m_worlds.begin() + i);
        i--;
    }
}

void Server::Run(Server *server)
{
    ENetEvent event;
    time_t to = 30 * 60;
    while (true)
    {
        time_t now = time(NULL);
        if (now - old > to)
        {
            old = now;
            Save_Worlds(server);
        }
        while (enet_host_service(Host, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
            {
                std::cout << "Connecting" << "\n";
                Connect(event.peer, Host);
                continue;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                std::cout << "Reciving" << "\n";
                Recieve(server, Host, event.packet, event.peer, get_text(event.packet));
                enet_packet_destroy(event.packet);
                continue;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                std::cout << "disconnecting" << "\n";
                Disconnect(Host, event.peer);
                continue;
            }
            }
        }
    }
}

Worlds Server::deserialize(std::string world)
{
    Worlds wld;
    std::istream *blobdata = WORLD_DATA(world);
    {
        boost::archive::binary_iarchive ia(*blobdata);
        ia >> wld;
    }
    return wld;
}

void Server::sendGlobalMessage(std::string message, bool Global /* false */)
{
    Packets p;
    std::string str = "";
    if (Global)
    {
        str = "`4Global System Message`o: " + message;
    }
    else
    {
        str = message;
    }
    std::vector<ENetPeer *> all = All(Host);
    for (ENetPeer *currentPeer : all)
    {
        p.consoleMessage(currentPeer, str);
    }
}

void Server::sendModsLogs(std::string message, bool log /* false */)
{
    Packets p;
    std::string str = "";
    if (log)
    {
        str = "`r[Mods Logs] `o" + message;
    }
    else
    {
        str = message;
    }
    std::vector<ENetPeer *> mod = Mods(Host);
    for (ENetPeer *currentPeer : mod)
    {
        p.consoleMessage(currentPeer, str);
    }
}