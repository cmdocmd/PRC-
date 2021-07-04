#include <iostream>
#include <signal.h>
#include "enet/enet.h"
#include "server.h"
#include "worlds.h"

void exitHandler(int s)
{
    exits = true;
}

int main()
{
    signal(SIGINT, exitHandler);
    Server *server = new Server(17091);
    Worlds world = world.GenerateWorld("ADMIN");
    server->m_worlds.push_back(world);
    server->Run(server);
    return 0;
}