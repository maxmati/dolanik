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

#include "filePlayer.h"
#include "fileSong.h"

#include <thread>

#include <dolanik/music.h>

#include <boost/thread.hpp>

namespace Dolanik {
static char *const get_error_text(const int error)
{
  static char error_buffer[255];
  av_strerror(error, error_buffer, sizeof(error_buffer) );
  return error_buffer;
}

FilePlayer::FilePlayer()
{
  av_register_all();
}
FilePlayer::~FilePlayer()
{

}
void FilePlayer::initPacket ( AVPacket* packet )
{
  av_init_packet(packet);
  packet->data = NULL;
  packet->size = 0;
}
FileSong::Ptr FilePlayer::createSong ( std::string path)
{
  FileSong::Ptr song(new FileSong(*this));
  AVCodec *input_codec = NULL;
  int error;
  int streamId = -1;
  if ((error = avformat_open_input(&(song->formatContext), path.c_str(), NULL, NULL)) < 0) 
  {
    std::cerr<<"Could not open input file '"<<path
      <<"' (error '"<<get_error_text(error)<<"')"<<std::endl;
    return FileSong::Ptr();
  }
  if ((error = avformat_find_stream_info(song->formatContext, NULL)) < 0) 
  {
    std::cerr<<"Could not open find stream info (error '"<<get_error_text(error)
      <<"')"<<std::endl;
    return FileSong::Ptr();
  }
  for(unsigned int i = 0; i<song->formatContext->nb_streams; ++i )
  {
    if(song->formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
    {
      streamId = i;
      break;
    }
  }
  if(streamId <0)
  {
    std::cerr<<"Could not find audio stream"<<std::endl;
    return FileSong::Ptr();
  }
  input_codec = avcodec_find_decoder(song->formatContext->streams[streamId]->codec->codec_id);
  if (!input_codec) 
  {
    std::cerr<<"Could not find input codec"<<std::endl;
    return FileSong::Ptr();
  }
  if ((error = avcodec_open2(song->formatContext->streams[streamId]->codec,
    input_codec, NULL)) < 0) 
  {
    std::cerr<<"Could not open input codec (error '"<<get_error_text(error)
      <<"')"<<std::endl;
    return FileSong::Ptr();
  }
  song->codecContext = song->formatContext->streams[streamId]->codec;
  
  AVDictionaryEntry *tag = NULL;
  if((tag = av_dict_get(song->formatContext->metadata, "album", NULL, 0)))
    song->album = tag->value;
  if((tag = av_dict_get(song->formatContext->metadata, "title", NULL, 0)))
    song->title = tag->value;
  if((tag = av_dict_get(song->formatContext->metadata, "artist", NULL, 0)))
    song->artist = tag->value;

  song->duration = song->formatContext->duration/AV_TIME_BASE;

  return song;
}
void FilePlayer::play ( FileSong& song, Dolanik::Music& music)
{
  int finished = 0;
  bool initialized = false;
  AVFrame* frame = av_frame_alloc();
  
  do{
    
    decodeAudioFrame(song, frame, &finished);
    
    if(finished)
      break;
    
    if(!initialized)
    {
      music.setInputFormat(frame->channel_layout, frame->sample_rate,
                           (AVSampleFormat)frame->format);
      initialized = true;
    }
    
    std::this_thread::sleep_for(music.send(
      (const char**)frame->extended_data,
      frame->nb_samples
    ));
    
  }while(!finished && song.playback);
  av_frame_free(&frame);
  
}


int FilePlayer::decodeAudioFrame(FileSong& song,AVFrame* frame, int* finished)
{
  /** Packet used for temporary storage. */
  AVPacket inputPacket;
  int error;
  int dataPresent;
    initPacket (&inputPacket );
  av_frame_unref(frame);
  /** Read one audio frame from the input file into a temporary packet. */
  if ((error = av_read_frame(song.formatContext, &inputPacket )) < 0) {
    /** If we are the the end of the file, flush the decoder below. */
    if (error == AVERROR_EOF)
      *finished = 1;
    else {
      std::cerr<<"Could not read frame (error '"<<get_error_text(error)
      <<"')"<<std::endl;
      return error;
    }
  }
  /**
   * Decode the audio frame stored in the temporary packet.
   * The input audio stream decoder is used to do this.
   * If we are at the end of the file, pass an empty packet to the decoder
   * to flush it.
   */
  if ((error = avcodec_decode_audio4(song.codecContext, frame,
    &dataPresent, &inputPacket )) < 0) 
  {
    std::cerr << "Could not decode frame (error '"<<get_error_text(error)<<"')"
    <<std::endl;
    av_free_packet(&inputPacket );
    return error;
  }
  /**
   * If the decoder has not been flushed completely, we are not finished,
   * so that this function has to be called again.
   */
  if (*finished && dataPresent)
    *finished = 0;
  av_free_packet(&inputPacket );
  return 0;
}
}