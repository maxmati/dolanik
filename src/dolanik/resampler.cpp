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
#include <misc.h>
#include <cassert>
#include <iostream>

#include <cstdio>
#include <chrono>

Resampler::Resampler( int64_t dstChannelLayout, uint dstRate, AVSampleFormat dstSampleFmt ):
  graph(NULL),
  srcFilter(NULL),
  volumeFilter(NULL),
  formatFilter(NULL),
  sinkFilter(NULL),
  volume(0.2),
  dstRate(dstRate),
  dstFormat(dstSampleFmt),
  dstChannelLayout(dstChannelLayout),
  dstChannelNb(av_get_channel_layout_nb_channels(dstChannelLayout)),
  dstSampleSize(av_get_bytes_per_sample(dstFormat)),
  srcRate(0),
  srcFormat(AV_SAMPLE_FMT_NONE),
  srcChannelLayout(0),
  srcChannelsNb(0),
  srcIsPlanar(false),
  _srcSampleSize(0),
  srcSampleSize(0),
  srcPlanes(0),
  FRAME_SIZE(1024)
{
}
Resampler::~Resampler()
{
  if(graph)
    avfilter_graph_free(&graph);
  
}
void Resampler::setInputFormat ( int64_t srcChannelLayout, uint srcRate, AVSampleFormat srcSampleFmt )
{
  flush();
  
  this->srcRate = srcRate;
  this->srcFormat = srcSampleFmt;
  this->srcChannelLayout = srcChannelLayout;
  this->srcChannelsNb = av_get_channel_layout_nb_channels(srcChannelLayout);
  this->srcIsPlanar = av_sample_fmt_is_planar(srcFormat);
  this->_srcSampleSize = av_get_bytes_per_sample(srcFormat);
  this->srcSampleSize = srcIsPlanar ?  _srcSampleSize : _srcSampleSize * srcChannelsNb;
  this->srcPlanes = srcIsPlanar ? srcChannelsNb : 1;
  
  assert(initFilterGraph() >= 0);
  
  inputBuffer.resize(srcChannelsNb);
  for(uint i = 0; i < srcChannelsNb; ++i)
    inputBuffer[i].resize(0);
  
  char chLayout[64];
  char format[64];
  av_get_sample_fmt_string (format, sizeof( format ), srcFormat);
  av_get_channel_layout_string( chLayout, sizeof( chLayout ), 0, srcChannelLayout);
  std::cout<<"Resampler change input format to: "<<format<<':'<<chLayout<<" "<<srcRate<<"hz"<<std::endl;
  
}

int Resampler::flush()
{
  int err;
  if(inputBuffer.size() == 0 || inputBuffer[0].size() == 0)
    return 0;
  
  uint inputBufferSamples = inputBuffer[0].size()/srcSampleSize;
  
  AVFrame *frame;
  frame = av_frame_alloc();
  frame->sample_rate = srcRate;
  frame->format = srcFormat;
  frame->channel_layout = srcChannelLayout;
  frame->nb_samples = inputBufferSamples;
  err = av_frame_get_buffer(frame, 0);
  if (err < 0)
    return err;

  for(uint j = 0; j < srcPlanes; ++j)//2 is number of channels
  {
    memcpy(
      frame->extended_data[j],
      inputBuffer[j].data(),
      sizeof(char)*srcSampleSize*inputBufferSamples
    );
  }
  err = av_buffersrc_add_frame(srcFilter, frame);
  if (err < 0) {
    av_frame_unref(frame);
    std::cerr<<"Error submitting the frame to the filtergraph:"<<std::endl;
    return err;
  }
  while ((err = av_buffersink_get_frame(sinkFilter, frame)) >= 0) 
  {
    uint outBufferSize = outputBuffer.size();
    outputBuffer.resize(dstSampleSize * frame->nb_samples + outBufferSize);
    mempcpy(
      &(outputBuffer.data()[outBufferSize]),
      frame->extended_data[0],
      sizeof(char)*dstSampleSize * frame->nb_samples 
    );
    av_frame_unref(frame);
  }
  return 0;
}

int Resampler::initFilterGraph()
{
  AVFilter        *abuffer;
  AVFilter        *volume;
  AVFilter        *aformat;
  AVFilter        *abuffersink;
  char ch_layout[64];
  char options_str[1024];
  int err;
  
  avfilter_register_all();
  
  if(graph)
    avfilter_graph_free(&graph);
  
  graph = avfilter_graph_alloc();
  if (!graph) {
    std::cerr<<"Unable to create filter graph."<<std::endl;
    return AVERROR(ENOMEM);
  }
  
  /* Create the abuffer filter;
   * it will be used for feeding the data into the graph. */
  abuffer = avfilter_get_by_name("abuffer");
  if (!abuffer) {
    std::cerr<<"Could not find the abuffer filter."<<std::endl;
    return AVERROR_FILTER_NOT_FOUND;
  }
  srcFilter = avfilter_graph_alloc_filter( graph, abuffer, "src");
  if (!srcFilter ) {
    std::cerr<<"Could not allocate the abuffer instance."<<std::endl;
    return AVERROR(ENOMEM);
  }

  /* Set the filter options through the AVOptions API. */
  av_get_channel_layout_string(ch_layout, sizeof(ch_layout), 0, srcChannelLayout);
  av_opt_set( srcFilter, "channel_layout", ch_layout, AV_OPT_SEARCH_CHILDREN);
  av_opt_set( srcFilter, "sample_fmt", av_get_sample_fmt_name(srcFormat), AV_OPT_SEARCH_CHILDREN);
  av_opt_set_q( sinkFilter, "time_base", (AVRational){ 1, srcRate },  AV_OPT_SEARCH_CHILDREN);
  av_opt_set_int( srcFilter, "sample_rate", srcRate, AV_OPT_SEARCH_CHILDREN);
  /* Now initialize the filter; we pass NULL options, since we have already
   * set all the options above. */
  err = avfilter_init_str( srcFilter, NULL);
  if (err < 0) {
    std::cerr<<"Could not initialize the abuffer filter."<<std::endl;
    return err;
  }
  /* Create volume filter. */
  volume = avfilter_get_by_name("volume");
  if (!volume) {
    std::cerr<<"Could not find the volume filter."<<std::endl;
    return AVERROR_FILTER_NOT_FOUND;
  }

  volumeFilter = avfilter_graph_alloc_filter( graph, volume, "volume");
  if (!volumeFilter ) {
    std::cerr<<"Could not allocate the volume instance."<<std::endl;
    return AVERROR(ENOMEM);
  }

  /* A different way of passing the options is as key/value pairs in a
   * dictionary. */
  AVDictionary *options_dict = NULL;
  av_dict_set(&options_dict, "volume", Anal::toStr(this->volume).c_str(), 0);
  av_dict_set(&options_dict, "precision", "fixed", 0);
  err = avfilter_init_dict(volumeFilter, &options_dict);
  av_dict_free(&options_dict);

  if (err < 0) {
    std::cerr<<"Could not initialize the volume filter."<<std::endl;
    return err;
  }
  
  /* Create the aformat filter;
   * it ensures that the output is of the format we want. */
  aformat = avfilter_get_by_name("aformat");
  if (!aformat) {
    std::cerr<<"Could not find the aformat filter."<<std::endl;
    return AVERROR_FILTER_NOT_FOUND;
  }
  formatFilter = avfilter_graph_alloc_filter( graph, aformat, "aformat");
  if (!formatFilter ) {
    std::cerr<<"Could not allocate the aformat instance."<<std::endl;
    return AVERROR(ENOMEM);
  }

  /* A third way of passing the options is in a string of the form
   * key1=value1:key2=value2.... */
  
  av_get_channel_layout_string(ch_layout, sizeof(ch_layout), 0, dstChannelLayout);
  snprintf(options_str, sizeof(options_str),
           "sample_fmts=%s:sample_rates=%d:channel_layouts=%s",
           av_get_sample_fmt_name(dstFormat), dstRate,ch_layout);
  err = avfilter_init_str(formatFilter, options_str);

  if (err < 0) {
    av_log(NULL, AV_LOG_ERROR, "Could not initialize the aformat filter.\n");
    return err;
  }
  /* Finally create the abuffersink filter;
   * it will be used to get the filtered data out of the graph. */
  abuffersink = avfilter_get_by_name("abuffersink");
  if (!abuffersink) {
    std::cerr<<"Could not find the abuffersink filter."<<std::endl;
    return AVERROR_FILTER_NOT_FOUND;
  }
  sinkFilter = avfilter_graph_alloc_filter( graph, abuffersink, "sink");
  if (!sinkFilter ) {
    std::cerr<<"Could not allocate the abuffersink instance."<<std::endl;
    return AVERROR(ENOMEM);
  }
  
  /* This filter takes no options. */
  err = avfilter_init_str( sinkFilter, NULL);
  if (err < 0) {
    std::cerr<<"Could not initialize the abuffersink instance."<<std::endl;
    return err;
  }
  /* Connect the filters;
   * in this simple case the filters just form a linear chain. */
  err = avfilter_link( srcFilter, 0, volumeFilter, 0);
  if (err >= 0)
    err = avfilter_link( volumeFilter, 0, formatFilter, 0);
  if (err >= 0)
    err = avfilter_link( formatFilter, 0, sinkFilter, 0);
  if (err < 0) {
    std::cerr<<"Error connecting filters"<<std::endl;
    return err;
  }
  /* Configure the graph. */
  err = avfilter_graph_config( graph, NULL);
  if (err < 0) {
    av_log(NULL, AV_LOG_ERROR, "Error configuring the filter graph\n");
    return err;
  }

  std::cout<<avfilter_graph_dump(graph,"")<<std::endl;
  return 0;
}

void Resampler::setVolume ( float volume )
{
  std::cout<<"setVolume("<<Anal::toStr(volume)<<")"<<std::endl;
  this->volume = volume;
  int err = avfilter_process_command(volumeFilter,"volume",Anal::toStr(volume).c_str(), NULL, 0, 0);
  if (err < 0)
  {
    char errorstr[1024];
    av_strerror(err, errorstr, sizeof(errorstr));
    std::cout<<"error changing volume "<<err<<" "<<errorstr<<std::endl;
  }
}

int Resampler::calculateDstSamplesNumber ( int srcSamplesNumber )
{
  uint realInputSamples = ((inputBuffer[0].size()/srcSampleSize + srcSamplesNumber)/FRAME_SIZE)*FRAME_SIZE;
  uint outputSamples = av_rescale_rnd(realInputSamples,
                                      dstRate,srcRate, AV_ROUND_UP);
  return outputSamples + outputBuffer.size()/dstSampleSize;
}

int Resampler::resample ( const char** srcData, int srcSamples, char* dstData, int dstSamples )
{
  static long long int totalSamples = 0;
  static std::chrono::microseconds totalTime(0);
  
  auto start = std::chrono::steady_clock::now();
  int err;
  AVFrame *frame;
  frame  = av_frame_alloc();
  if (!frame) {
    std::cerr<<"Error allocating the frame"<<std::endl;
    return -1;
  }

  uint consumedSamples = 0;
  
  while(srcSamples + inputBuffer[0].size()/srcSampleSize >= FRAME_SIZE)
  {
    av_frame_unref(frame);
    frame->sample_rate = srcRate;
    frame->format = srcFormat;
    frame->channel_layout = srcChannelLayout;
    frame->nb_samples = FRAME_SIZE;
    err = av_frame_get_buffer(frame, 0);
    if (err < 0)
      return err;
    
    uint inputBufferSamples = inputBuffer[0].size()/srcSampleSize;
    uint currentConsumedSamples = 0;
    
    for(uint j = 0; j < srcPlanes; ++j)//2 is number of channels
    {
      if(inputBufferSamples > 0)
      {
        memcpy(
          frame->extended_data[j],
          inputBuffer[j].data(),
          sizeof(char)*srcSampleSize*inputBufferSamples
        );
        
        memcpy(
          &(frame->extended_data[j][srcSampleSize*inputBufferSamples]),
          &(srcData[j][srcSampleSize*consumedSamples]),
          sizeof(char)*srcSampleSize*(FRAME_SIZE - inputBufferSamples)
        );
        
        inputBuffer[j].resize(0);
        currentConsumedSamples = (FRAME_SIZE - inputBufferSamples);
      } else {
         memcpy(
           frame->extended_data[j],
           &(srcData[j][srcSampleSize*consumedSamples]),
           sizeof(char)*srcSampleSize*FRAME_SIZE
         );
         
         currentConsumedSamples = FRAME_SIZE;
      }
    }
    
    err = av_buffersrc_add_frame(srcFilter, frame);
    if (err < 0) {
      av_frame_unref(frame);
      std::cerr<<"Error submitting the frame to the filtergraph:"<<std::endl;
      return err;
    }

    consumedSamples += currentConsumedSamples;
    srcSamples -= currentConsumedSamples;
  }
  if(srcSamples > 0)
  {
    for(uint j = 0; j < srcPlanes; ++j)//2 is number of channels
    {
      uint currentBufferSize = inputBuffer[j].size();
      inputBuffer[j].resize(srcSamples * srcSampleSize + currentBufferSize);
      memcpy(
        inputBuffer[j].data() + currentBufferSize,
        &(srcData[j][srcSampleSize*consumedSamples]),
        sizeof(char)*srcSampleSize*srcSamples  
      );
    }
  }
  
  /* Get all the filtered output that is available. */
  uint providedFrames = 0;
  
  if(outputBuffer.size() > 0)
  {
    memcpy(
      &(dstData[dstSampleSize*providedFrames]),
      outputBuffer.data(),
      sizeof(char)*outputBuffer.size()
    );
    
    providedFrames += outputBuffer.size()/dstSampleSize;
    outputBuffer.resize(0);
  }
  
  //av_frame_unref(frame);
  while (dstSamples > 0 && ((err = av_buffersink_get_frame(sinkFilter, frame)) >= 0)) {
    if(dstSamples >= frame->nb_samples)
    {
      memcpy(
        &(dstData[dstSampleSize*providedFrames]),
        frame->extended_data[0],
        sizeof(char)*dstSampleSize*frame->nb_samples
      );
      
      providedFrames += frame->nb_samples; 
      dstSamples -= frame->nb_samples;
    } else {
      memcpy(
        &(dstData[dstSampleSize*providedFrames]),
        frame->extended_data[0],
        sizeof(char)*dstSampleSize*dstSamples
      );
      
      outputBuffer.resize(dstSampleSize * (frame->nb_samples - dstSamples));
      mempcpy(
        outputBuffer.data(),
        &(frame->extended_data[0][dstSampleSize*dstSamples]),
        sizeof(char)*dstSampleSize * (frame->nb_samples - dstSamples)
      );
      providedFrames += dstSamples;
      dstSamples -= dstSamples;
    }
    av_frame_unref(frame);
  }
  
  auto end = std::chrono::steady_clock::now();  
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds >(end - start);
  totalTime += elapsed;
  totalSamples += providedFrames;
  if(totalSamples > 131072)
  {
    std::cout<<"resampling last "<<totalSamples<<" samples took "<< totalTime.count()<<"µs."<<std::endl;
    totalTime = std::chrono::microseconds(0);
    totalSamples = 0;
  }
    
  return providedFrames;
}
