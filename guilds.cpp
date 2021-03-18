#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

using std::string;

struct Guilds
{
private:
    friend class boost::serialization::access;
    template <class Ar>
    void serialize(Ar &ar, unsigned)
    {
        ar &name &world &leader &level &xp &max &statement &Players &Elder &CoLeader;
    }

public:
    string name;
    string world;
    string leader;
    int level;
    int xp;
    int max;
    string statement;
    std::vector<string> Players;
    std::vector<string> Elder;
    std::vector<string> CoLeader;
};

std::stringstream serialize_guild(Guilds const &guild)
{
    std::stringstream str;
    {
        boost::archive::binary_oarchive oa(str);
        oa << guild;
    }
    return str;
}