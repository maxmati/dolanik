/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  Mateusz "MaxMati" Nowotynski <maxmati4@gmail.com>

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
#include <deque>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <mpg123.h>

#include <libmumbleclient/Client.hpp>

namespace Dolanik{
 
  
  class Music
  {
  public:
	  
	  struct Song{
		  boost::filesystem::path path;
		  std::string artist;
		  std::string title;
		  std::string album;
	  };

  public:
    Music(MumbleClient::MumbleClient* mc);
    virtual void play(std::string path, std::string title, std::string album, std::string artist);
    virtual void stop();
    virtual void adjustVolume(double delta);
    virtual void replay();
    virtual void setEqualizer(int band, double amp);
    virtual void resetEqualizer();
    virtual void clearQueue();
    virtual double getVolume();
    virtual void setVolume(double volume);
    Song getCurrentSong();
    uint getCurrentSongLength();
    
    virtual void run();
    virtual ~Music();
  protected:

  private:
    void onTxtMsg(const std::string& text);
    void statusComment();
    std::string genPlaylistString();
    void playMp3(const char* path);
    
    bool playback;
    bool replayCurrentSongFlag;
    double volume;

    mpg123_handle *mh;

    MumbleClient::MumbleClient* mc;
    
    const int32_t kSampleRate;
    long orginalSampleRate;

    Song currentSong;
    Song lastSong;
    std::deque<Song> queuedSongs;
  };

}
