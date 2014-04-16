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
#include <boost/function.hpp>

namespace MumbleClient
{
class MumbleClient;
}

class Mp3Player;
class Mp3Song : public Dolanik::Song
{
public:
  typedef boost::shared_ptr<Mp3Song> Ptr;
  
  Mp3Song( std::string path, Mp3Player* player );
  virtual void play( MumbleClient::MumbleClient* mc);
  virtual void stop();
  virtual int getDuration();
  virtual std::string getTitle();
  virtual std::string getAlbum();
  virtual std::string getArtist();
  std::string getPath();
  bool isStoped();
private:
  int duration;
  std::string title;
  std::string album;
  std::string artist;
  std::string path;
  Mp3Player* player;
  bool stoped;
};