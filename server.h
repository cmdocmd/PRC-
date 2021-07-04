#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include <signal.h>

#include "worlds.h"

extern bool exits;

class Server
{
private:
public:
    std::vector<Worlds> m_worlds;
    Server(int port);
    void Run(Server *server);
    void Flush_Worlds(Worlds world, Server *server);
    void Save_Worlds(Server *server);
    std::stringstream serialize_world(Worlds const &world);
    Worlds deserialize(std::string world);
    void sendGlobalMessage(std::string message, std::string music, bool Global = false);
    void sendModsLogs(std::string message, std::string music, bool log = false);
};
#endif