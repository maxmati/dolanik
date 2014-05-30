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
  #include <libavutil/opt.h>
  #include <libavutil/channel_layout.h>
  #include <libavutil/samplefmt.h>
  #include <libswresample/swresample.h>
}
#undef __STDC_CONSTANT_MACROS

class Resampler
{
public:
  Resampler(int64_t dstChannelLayout, uint dstRate, AVSampleFormat dstSampleFmt);
  void setInputFormat(int64_t srcChannelLayout, uint srcRate, AVSampleFormat srcSampleFmt);
  int calculateDstSamplesNumber(int srcSamplesNumber);
  int resample( const char** srcData, int srcSamples, char* dstData, int dstSamples );
  ~Resampler();
private:
  uint dstRate;
  int srcRate;
  SwrContext *swrCtx;
};
