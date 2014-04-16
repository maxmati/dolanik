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

#include "mp3Song.h"

#include <dolanik/mp3Player.h>

Mp3Song::Mp3Song ( std::string path, Mp3Player* player )
{
  this->path = path;
  this->player = player;
}

void Mp3Song::play( MumbleClient::MumbleClient* mc )
{
  stoped = false;
  player->run(*this, mc);
}
void Mp3Song::stop()
{
  stoped = true;
}
bool Mp3Song::isStoped()
{
  return stoped;
}


std::string Mp3Song::getPath()
{
  return path;
}

int Mp3Song::getDuration()
{
  return 0;
}

std::string Mp3Song::getTitle()
{
  return std::string();
}

std::string Mp3Song::getAlbum()
{
  return std::string();
}

std::string Mp3Song::getArtist()
{
  return std::string();
}
