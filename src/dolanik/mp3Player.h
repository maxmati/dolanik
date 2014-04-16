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

#include <string>
#include <dolanik/mp3Song.h>

namespace MumbleClient
{
class MumbleClient;
}

namespace Dolanik
{
class Dolanik;
}

class Mp3Song;
class Mp3Player
{
  friend class Mp3Song;
  public:
    
    Mp3Player();
    Mp3Song::Ptr createSong(std::string path, std::string title,
			    std::string album, std::string artist);
  private:
    void run(Mp3Song& song, MumbleClient::MumbleClient* mc );
    void playMp3(Mp3Song& song, MumbleClient::MumbleClient* mc);
    

};

