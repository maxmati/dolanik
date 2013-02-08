/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once

#include <string>
#include <boost/filesystem/path.hpp>
#if 0
#include "../network/dolanMusicSignal.h"
#include "../network/connection.h"
#endif

namespace Dolan {

class MusicI
{
public:
    struct Song{
      boost::filesystem::path path;
      std::string artist;
      std::string title;
      std::string album;
    };
public:
    virtual void play(std::string path, std::string title, std::string album, std::string artist)=0;
    virtual void stop()=0;
    virtual void adjustVolume(double delta)=0;
    virtual void replay()=0;
    virtual void setEqualizer(int band, double amp)=0;
    virtual void resetEqualizer()=0;
    virtual void clearQueue()=0;
    virtual double getVolume()=0;
    virtual void setVolume(double volume)=0;
    virtual Song getCurrentSong()=0;
    virtual void parseMsg(){};

    
    MusicI(){};
    virtual ~MusicI(){};
public:
   /* void parseMsg(Network::DolanMusicSignal& signal,boost::shared_ptr<Network::Connection > connection){
      if(signal.action == Network::DolanMusicSignal::play)
	this->play(signal.sArgs.at(0),signal.sArgs.at(1),signal.sArgs.at(2),signal.sArgs.at(3));
      else if(signal.action == Network::DolanMusicSignal::stop)
	this->stop();
      else if(signal.action == Network::DolanMusicSignal::adjustVolume)
	this->adjustVolume(signal.dArgs.at(0));
      else if(signal.action == Network::DolanMusicSignal::replay)
	this->replay();
      else if(signal.action == Network::DolanMusicSignal::setEqualizer)
	this->setEqualizer(signal.iArgs.at(0), signal.dArgs.at(0));
      else if(signal.action == Network::DolanMusicSignal::resetEqualizer)
	this->resetEqualizer();
      else if(signal.action == Network::DolanMusicSignal::clearQueue)
	this->clearQueue();
      else if(signal.action == Network::DolanMusicSignal::setVolume)
	this->setVolume(signal.dArgs.at(0));
      else if(signal.action == Network::DolanMusicSignal::getVolume)
      {
	Network::DolanMusicSignal response;
	response.action = Network::DolanMusicSignal::currentVolume;
	response.dArgs.push_back(this->getVolume());
	response.response = true;
	response.sesionId = signal.sesionId;
	connection->send(response,Network::DolanMusicSignalMsgType);
      }
      else if(signal.action == Network::DolanMusicSignal::getCurrentSong)
      {
	Network::DolanMusicSignal response;
	response.action = Network::DolanMusicSignal::currentSong;
	Song song = this->getCurrentSong();
	response.sArgs.push_back(song.path.native());
	response.sArgs.push_back(song.title);
	response.sArgs.push_back(song.album);
	response.sArgs.push_back(song.artist);
	response.response = true;
	response.sesionId = signal.sesionId;
	connection->send(response,Network::DolanMusicSignalMsgType);
      }
    }; */
};
}
