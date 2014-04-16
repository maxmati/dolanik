/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <dolanik/song.h>
#include <boost/enable_shared_from_this.hpp>

namespace MumbleClient
{
class MumbleClient;
}

class sp_album;
class sp_artist;
class Spotify;
class sp_track;
class SpotifySong : 
  public Dolanik::Song, 
  public boost::enable_shared_from_this<SpotifySong>
{
  friend class Spotify;
public:
  typedef boost::shared_ptr<SpotifySong> Ptr;
  SpotifySong(sp_track* track, Spotify* spotify);
  ~SpotifySong();
  bool loaded();
  virtual int getDuration();
  virtual std::string getTitle();
  virtual std::string getAlbum();
  virtual std::string getArtist();
  virtual void stop();
  virtual void play( MumbleClient::MumbleClient* mc);
private:
  bool updateMetadata();//< Use only from callback no locking
  
  int duration;
  std::string title;
  std::string albumName;
  std::string artistName;
  
  sp_track* track;
  sp_artist* artist;
  sp_album* album;
  
  Spotify* spotify;
  
  MumbleClient::MumbleClient* mc;
};

