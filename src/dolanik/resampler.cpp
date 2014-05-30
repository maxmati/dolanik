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

#include "resampler.h"
#include <cassert>

Resampler::Resampler( int64_t dstChannelLayout, uint dstRate, AVSampleFormat dstSampleFmt ):
  dstRate(dstRate),
  srcRate(0),
  swrCtx(nullptr)
{
  //const int64_t srcChLayout = AV_CH_LAYOUT_MONO;
  //const int64_t dstChLayout = AV_CH_LAYOUT_MONO;
  //const uint srcRate = 44100;
  //const AVSampleFormat srcSampleFmt = AV_SAMPLE_FMT_S16;
  //const AVSampleFormat dstSampleFmt = AV_SAMPLE_FMT_S16;
  //uint srcNbChannels;
  swrCtx = swr_alloc();
  assert(swrCtx);
  
  /* set options */
  /*av_opt_set_int(swrCtx, "in_channel_layout",    srcChLayout, 0);
  av_opt_set_int(swrCtx, "in_sample_rate",       srcRate, 0);
  av_opt_set_sample_fmt(swrCtx, "in_sample_fmt", srcSampleFmt, 0);*/
  av_opt_set_int(swrCtx, "out_channel_layout",    dstChannelLayout, 0);
  av_opt_set_int(swrCtx, "out_sample_rate",       dstRate, 0);
  av_opt_set_sample_fmt(swrCtx, "out_sample_fmt", dstSampleFmt, 0);
   
  //srcNbChannels = av_get_channel_layout_nb_channels(srcChLayout);
}

void Resampler::setInputFormat ( int64_t srcChannelLayout, uint srcRate, AVSampleFormat srcSampleFmt )
{
  this->srcRate = srcRate;
  av_opt_set_int(swrCtx, "in_channel_layout",    srcChannelLayout, 0);
  av_opt_set_int(swrCtx, "in_sample_rate",       srcRate, 0);
  av_opt_set_sample_fmt(swrCtx, "in_sample_fmt", srcSampleFmt, 0);
  
  assert(swr_init(swrCtx) >= 0);
  
}


Resampler::~Resampler()
{
  if(swrCtx)
    swr_free(&swrCtx);
}

int Resampler::calculateDstSamplesNumber ( int srcSamplesNumber )
{
  return av_rescale_rnd(swr_get_delay(swrCtx, srcRate) + srcSamplesNumber,
			srcRate,dstRate, AV_ROUND_UP);
}

int Resampler::resample ( const char** srcData, int srcSamples, char* dstData, int dstSamples )
{
  return swr_convert(swrCtx, (uint8_t**)&dstData, dstSamples, (const uint8_t**)srcData, srcSamples);
}
