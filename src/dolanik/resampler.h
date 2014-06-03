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

#include <vector>

#define __STDC_CONSTANT_MACROS
extern "C"
{
  #include <libavutil/opt.h>
  #include <libavutil/channel_layout.h>
  #include <libavutil/samplefmt.h>
  #include <libavfilter/avfiltergraph.h>
  #include <libavfilter/buffersink.h>
  #include <libavfilter/buffersrc.h>
  #include <libswresample/swresample.h>
}
#undef __STDC_CONSTANT_MACROS

class Resampler
{
public:
  Resampler(int64_t dstChannelLayout, uint dstRate, AVSampleFormat dstSampleFmt);//FIXME: support only mono as output
  void setInputFormat(int64_t srcChannelLayout, uint srcRate, AVSampleFormat srcSampleFmt);
  void setVolume(float volume);
  int calculateDstSamplesNumber(int srcSamplesNumber);
  int resample( const char** srcData, int srcSamples, char* dstData, int dstSamples );
  ~Resampler();
private:
  int initFilterGraph();
    int flush();
  
  
  AVFilterGraph* graph;
  AVFilterContext* srcFilter;
  AVFilterContext* volumeFilter;
  AVFilterContext* formatFilter;
  AVFilterContext* sinkFilter;
  
  float volume;
  
  const uint dstRate;
  const AVSampleFormat dstFormat;
  const int64_t dstChannelLayout; 
  const uint dstChannelNb;
  size_t dstSampleSize;
  
  uint srcRate;
  AVSampleFormat srcFormat;
  int64_t srcChannelLayout;
  uint srcChannelsNb;
  bool srcIsPlanar;
  size_t _srcSampleSize;//FIXME:rename
  size_t srcSampleSize;//FIXME: rename
  size_t srcPlanes;
    
  const uint FRAME_SIZE;
  std::vector<std::vector<char>> inputBuffer;
  std::vector<char> outputBuffer;
  
};
