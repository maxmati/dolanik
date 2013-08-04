#pragma once

#include "dolan.h"
#include <boost/shared_ptr.hpp>

namespace Dolanik {
class Dolan;
class Music;
}

class MusicI : public Dolan::Music {
public:
    MusicI(Dolanik::Dolan* dolan);
    virtual void stop(Ice::Int, const Ice::Current&);
    virtual Ice::Double adjustVolume(Ice::Int, Ice::Double, const Ice::Current&);
    virtual void replay(Ice::Int, const Ice::Current&);
    virtual void setEqualizer(Ice::Int, Ice::Int, Ice::Double, const Ice::Current&);
    virtual void resetEqualizer(Ice::Int, const Ice::Current&);
    virtual void clearQueue(Ice::Int, const Ice::Current&);
    virtual Ice::Double getVolume(Ice::Int, const Ice::Current&);
    virtual void setVolume(Ice::Int, Ice::Double, const Ice::Current&);
    virtual void play(Ice::Int, const Dolan::Song&, const Ice::Current&);
    virtual Dolan::Song getCurrentSong(Ice::Int, const Ice::Current&);
private:
    Dolanik::Dolan* dolan;
};

class ServerI : public Dolan::Server {
public:
    ServerI(Dolanik::Dolan* dolan);
    virtual Ice::Int connect(const Dolan::ServerInfo& , const Ice::Current&  = ::Ice::Current());
    virtual Dolan::ServersInfo getServers(const Ice::Current&  = ::Ice::Current());
    virtual void disconnect(Ice::Int , const Ice::Current&  = ::Ice::Current());
private:
    Dolanik::Dolan* dolan;
};
