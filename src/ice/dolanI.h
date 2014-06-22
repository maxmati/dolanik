#pragma once

#include <dolan.h>
#include <boost/shared_ptr.hpp>

#include <config.h>

#ifdef USE_SPOTIFY
class Spotify;
#endif
namespace Dolanik {
class Dolanik;
class Music;
class FilePlayer;
}

class MusicI : public Dolan::Music {
public:
    MusicI(Dolanik::Dolanik* dolan);
    virtual void stop(Ice::Int, const Ice::Current&);
    virtual Ice::Double adjustVolume(Ice::Int, Ice::Double, const Ice::Current&);
    virtual void replay(Ice::Int, const Ice::Current&);
    virtual void setEqualizer(Ice::Int, Ice::Int, Ice::Double, const Ice::Current&);
    virtual void resetEqualizer(Ice::Int, const Ice::Current&);
    virtual void clearQueue(Ice::Int, const Ice::Current&);
    virtual Ice::Double getVolume(Ice::Int, const Ice::Current&);
    virtual void setVolume(Ice::Int, Ice::Double, const Ice::Current&);
    virtual Dolan::Song getCurrentSong(Ice::Int, const Ice::Current&);
private:
    Dolanik::Dolanik* dolan;
};

class ServerI : public Dolan::Server {
public:
    ServerI(Dolanik::Dolanik* dolan);
    virtual Ice::Int connect(const Dolan::ServerInfo& , const Ice::Current&  = ::Ice::Current());
    virtual Dolan::ServersInfo getServers(const Ice::Current&  = ::Ice::Current());
    virtual void disconnect(Ice::Int , const Ice::Current&  = ::Ice::Current());
private:
    Dolanik::Dolanik* dolan;
};
#ifdef USE_SPOTIFY
class SpotifyPlayerI: public Dolan::SpotifyPlayer {
  public:
    SpotifyPlayerI(Dolanik::Dolanik& dolan, Spotify& spotify);
    virtual void play( const std::string& uri, Ice::Int id, const Ice::Current& );
  private:
    Dolanik::Dolanik& dolan;
    Spotify& spotify;
};
#endif
class FilePlayerI: public Dolan::FilePlayer {
  public:
    FilePlayerI( Dolanik::Dolanik& dolan, Dolanik::FilePlayer& player );
    virtual void play( const std::string& path, Ice::Int id, const Ice::Current& );
  private:
    Dolanik::Dolanik& dolan;
    Dolanik::FilePlayer& player;
};