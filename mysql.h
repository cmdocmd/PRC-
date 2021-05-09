#pragma once

#include <iostream>
#include <sstream>

void INSERT_ACCOUNT(std::string growid, std::string pass, std::stringstream blob);
void UPDATE(std::stringstream str, std::string growid);
bool ACCOUNT_EXIST(std::string growid);
bool CHECK_LOGIN(std::string growid, std::string pass);
std::istream* PLAYER_DATA(std::string growid);
int PLAYER_ID(std::string growid);
void INSERT_WORLD(std::string name, std::stringstream blob);
void UPDATE_WORLD(std::stringstream str, std::string name);
bool WORLD_EXIST(std::string world);
std::istream* WORLD_DATA(std::string world);
void INSERT_GUILD(std::string name, std::stringstream blob);
bool GUILD_EXIST(std::string guild);
