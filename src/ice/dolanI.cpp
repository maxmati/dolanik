#include "dolanI.h"

MusicI::MusicI(boost::shared_ptr<Dolanik::Music> music):
music(music)
{
}
void MusicI::play(const Dolan::Song& song, const Ice::Current&)
{
	this->music->play(song.path, song.title, song.album, song.artist);
}
Dolan::Song MusicI::getCurrentSong(const Ice::Current&)
{
	Dolanik::Music::Song song = this->music->getCurrentSong();
	Dolan::Song tmp;
	tmp.path = song.path.native();
	tmp.title = song.title;
	tmp.album = song.album;
	tmp.artist = song.artist;
	return tmp;
}
void MusicI::stop(const Ice::Current&)
{
	this->music->stop();
}
Ice::Double MusicI::adjustVolume(Ice::Double delta, const Ice::Current&)
{
	this->music->adjustVolume(delta);
	return this->music->getVolume();
}
void  MusicI::replay(const Ice::Current&)
{
	this->music->replay();
}
void  MusicI::setEqualizer(Ice::Int band, Ice::Double amp, const Ice::Current&)
{
	this->music->setEqualizer(band, amp);
}
void  MusicI::resetEqualizer(const Ice::Current&)
{
	this->music->resetEqualizer();
}
void  MusicI::clearQueue(const Ice::Current&)
{
	this->music->clearQueue();
}
Ice::Double  MusicI::getVolume(const Ice::Current&)
{
	return this->music->getVolume();
}
void  MusicI::setVolume(Ice::Double volume, const Ice::Current&)
{
	this->music->setVolume(volume);
}
