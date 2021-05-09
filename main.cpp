#include <iostream>
#include "enet/enet.h"
#include "server.h"
#include "worlds.h"

int main()
{
    Worlds world = world.GenerateWorld("CMD");
    Server * server = new Server(17091);
    server->m_worlds.push_back(world);
    server->Run(server);
    return 0;
}