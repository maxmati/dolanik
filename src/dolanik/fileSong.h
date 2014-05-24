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
#include <queue>

class FilePlayer;
class AVFormatContext;
struct AVCodecContext;
class FileSong : public Dolanik::Song
{
  friend class FilePlayer;
public:
  typedef boost::shared_ptr<FileSong> Ptr;
  FileSong(FilePlayer& player);
  ~FileSong();
  virtual void stop();
  virtual void play ( MumbleClient::MumbleClient* mc );
  virtual int getDuration();
  virtual std::string getTitle();
  virtual std::string getAlbum();
  virtual std::string getArtist();
private:
  AVCodecContext* codecContext;
  AVFormatContext* formatContext;
  std::queue<boost::shared_ptr<char>> framesBuffer;
  FilePlayer& player;
  bool playback;
};
