#include "dolanI.h"
#include <dolanik/dolan.h>
#include <dolanik/server.h>
#include <dolanik/music.h>
#include <libmumbleclient/Settings.hpp>

ServerI::ServerI(Dolanik::Dolan* dolan):
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
  return this->dolan->connect(si.host,si.port,si.username,si.password);
}


MusicI::MusicI(Dolanik::Dolan* dolan):
dolan(dolan)
{
}
void MusicI::play(Ice::Int id ,const Dolan::Song& song, const Ice::Current&)
{
	this->dolan->getMusic(id)->play(song.path, song.title, song.album, song.artist);
}
Dolan::Song MusicI::getCurrentSong(Ice::Int id ,const Ice::Current&)
{
	Dolanik::Music::Song song = this->dolan->getMusic(id)->getCurrentSong();
	Dolan::Song tmp;
	tmp.path = song.path.native();
	tmp.title = song.title;
	tmp.album = song.album;
	tmp.artist = song.artist;
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
