#include <iostream>
#include <sstream>

#include "players.h"
#include "enet/enet.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include <boost/algorithm/string.hpp>


Players *pinfo(ENetPeer *peer)
{
    return (Players *)(peer->data);
}

Players *Desrialize(std::istream *blobdata)
{
    Players *ply;
    {
        boost::archive::binary_iarchive ia(*blobdata);
        ia >> ply;
    }
    return ply;
}

std::stringstream serialize_player(Players *ply)
{
    std::stringstream str;
    {
        boost::archive::binary_oarchive oa(str);
        oa << ply;
    }
    return str;
}