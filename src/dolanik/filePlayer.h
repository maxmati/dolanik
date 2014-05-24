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

#define __STDC_CONSTANT_MACROS 
extern "C"
{
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
  #include <libavutil/frame.h>
  #include <libavutil/audio_fifo.h>
  #include <libavutil/error.h>
}
#undef __STDC_CONSTANT_MACROS

#include <iostream>
#include <libmumbleclient/Client.hpp>
#include "fileSong.h"

class FilePlayer
{
public:
  FileSong::Ptr createSong( std::string path );
  void play(FileSong& song, MumbleClient::MumbleClient* mc);
  FilePlayer();
  ~FilePlayer();
private:
  int decodeAudioFrame( FileSong& song, AVFrame* frame, int* finished );
  void initPacket(AVPacket *packet);

};
