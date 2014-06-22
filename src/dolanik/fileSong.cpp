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

#include "fileSong.h"
#include "filePlayer.h"

#define __STDC_CONSTANT_MACROS 
extern "C"
{
  #include <libavformat/avformat.h>
}
#undef __STDC_CONSTANT_MACROS

namespace Dolanik {
FileSong::FileSong ( FilePlayer& player )
  :codecContext(NULL),
  formatContext(NULL),
  player(player),
  playback(true)
{

}
FileSong::~FileSong()
{
  if(codecContext)
    avformat_close_input(&formatContext);
}

void FileSong::stop()
{
  playback = false;
}

void FileSong::play ( Music& music )
{
  playback = true;
  player.play(*this, music);
}

int FileSong::getDuration()
{
  return duration;
}

std::string FileSong::getTitle()
{
  return title;
}

std::string FileSong::getAlbum()
{
  return album;
}

std::string FileSong::getArtist()
{
  return artist;
}
}