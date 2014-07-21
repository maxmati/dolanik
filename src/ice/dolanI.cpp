#include "dolanI.h"

#ifdef USE_SPOTIFY
  #include <spotify/spotify.h>
#endif

#include <dolanik/dolanik.h>
#include <dolanik/server.h>
#include <dolanik/music.h>
#include <dolanik/filePlayer.h>
#include <libmumbleclient/Settings.hpp>

ServerI::ServerI(Dolanik::Dolanik* dolan):
dolan(dolan)
{}

Dolan::ServersInfo ServerI::getServers(const Ice::Current& )
{
  std::map< uint, boost::shared_ptr< Dolanik::Server > > servers = this->dolan->getServers();
  std::vector<Dolan::ServerInfo> serversOut;
  for( auto server: servers)
  {
    Dolan::ServerInfo serverOut;
    serverOut.id = server.first;
    serverOut.host = server.second->getHost();
    serverOut.port =  server.second->getPort();
    serverOut.username =  server.second->getUsername();
    serverOut.password =  server.second->getPassword();
    serversOut.push_back(serverOut);
  }
  return serversOut;
}

void ServerI::disconnect(Ice::Int id , const Ice::Current& )
{
  this->dolan->disconnect(id);
}

Ice::Int ServerI::connect(const Dolan::ServerInfo& si, const Ice::Current& )
{
  return this->dolan->connect(si.host,si.port,si.username,si.password, si.certFile);
}


MusicI::MusicI(Dolanik::Dolanik* dolan):
dolan(dolan)
{
}

Dolan::Song MusicI::getCurrentSong(Ice::Int id ,const Ice::Current&)
{
	Dolanik::Song::Ptr song = this->dolan->getMusic(id)->getCurrentSong();
	Dolan::Song tmp;
	tmp.title = song->getTitle();
	tmp.album = song->getAlbum();
	tmp.artist = song->getArtist();
  tmp.time = song->getDuration();
	return tmp;
}
void MusicI::stop(Ice::Int id ,const Ice::Current&)
{
	this->dolan->getMusic(id)->stop();
}
Ice::Double MusicI::adjustVolume(Ice::Int id ,Ice::Double delta, const Ice::Current&)
{
	this->dolan->getMusic(id)->adjustVolume(delta);
	return this->dolan->getMusic(id)->getVolume();
}
void  MusicI::replay(Ice::Int id ,const Ice::Current&)
{
	this->dolan->getMusic(id)->replay();
}
void  MusicI::setEqualizer(Ice::Int id ,Ice::Int band, Ice::Double amp, const Ice::Current&)
{
	this->dolan->getMusic(id)->setEqualizer(band, amp);
}
void  MusicI::resetEqualizer(Ice::Int id ,const Ice::Current&)
{
	this->dolan->getMusic(id)->resetEqualizer();
}
void  MusicI::clearQueue(Ice::Int id ,const Ice::Current&)
{
	this->dolan->getMusic(id)->clearQueue();
}
Ice::Double  MusicI::getVolume(Ice::Int id ,const Ice::Current&)
{
	return this->dolan->getMusic(id)->getVolume();
}
void  MusicI::setVolume(Ice::Int id ,Ice::Double volume, const Ice::Current&)
{
	this->dolan->getMusic(id)->setVolume(volume);
}

#ifdef USE_SPOTIFY
SpotifyPlayerI::SpotifyPlayerI ( Dolanik::Dolanik& dolan, Spotify& spotify )
:dolan(dolan),
spotify(spotify)
{}
void SpotifyPlayerI::play ( const std::string& uri, Ice::Int id, const Ice::Current& )
{
  Dolanik::Song::Ptr song = spotify.createSong(uri);
  dolan.getMusic(id)->play(song);
}
#endif

FilePlayerI::FilePlayerI ( Dolanik::Dolanik& dolan, Dolanik::FilePlayer& player )
:dolan(dolan),
player(player)
{}
void FilePlayerI::play ( const std::string& path, Ice::Int id, const Ice::Current& )
{
  Dolanik::Song::Ptr song = player.createSong(path);
  dolan.getMusic(id)->play(song);
}

