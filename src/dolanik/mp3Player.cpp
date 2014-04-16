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

#include "mp3Player.h"

#include <boost/shared_ptr.hpp>

#include <libmumbleclient/Client.hpp>

#include <mpg123.h>

#include <dolanik/mp3Song.h>
#include <dolanik/dolanik.h>
#include <dolanik/music.h>




Mp3Player::Mp3Player ()
{
}

Mp3Song::Ptr Mp3Player::createSong (std::string path, std::string title, 
				    std::string album, std::string artist)
{
  return Mp3Song::Ptr(new Mp3Song(path, this));
}
void Mp3Player::run ( Mp3Song& song, MumbleClient::MumbleClient* mc )
{
  playMp3(song, mc);
}

void Mp3Player::playMp3(Mp3Song& song, MumbleClient::MumbleClient* mc)
{
  std::cout << "<< play mp3 thread" << std::endl;
  
  struct sched_param param;
  param.sched_priority = 1;
  pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
  
  // FIXME(pcgod): 1-6 to match Mumble client
  uint frames = 6;
  mc->getAudio()->setMaxBandwidth(24000, frames);
  
  int err = mpg123_init();
  mpg123_handle* mh = mpg123_new(NULL, &err);
  mpg123_param(mh, MPG123_VERBOSE, 255, 0);
  mpg123_param(mh, MPG123_RVA, MPG123_RVA_MIX, 0);
  mpg123_param(mh, MPG123_ADD_FLAGS, MPG123_MONO_MIX, 0);
  mpg123_param(mh, MPG123_FORCE_RATE, 48000, 0);
  mpg123_open(mh, song.getPath().c_str());
  
  long rate = 0;
  int channels = 0, encoding = 0;
  mpg123_getformat(mh, &rate, &channels, &encoding);
  mpg123_format_none(mh);
  
  //this->orginalSampleRate = rate;
  
  if(mpg123_scan(mh) == MPG123_ERR) {
    std::cerr << "Malformed audio frame found; exiting!" << std::endl;
  }
  
  //statusComment();
  
  mpg123_id3v2* id3v2;
  if(mpg123_meta_check(mh) & MPG123_ID3)
    mpg123_id3(mh, NULL, &id3v2);
  
  rate = 48000;
  channels = MPG123_MONO;
  err = mpg123_format(mh, rate, channels, encoding);
  
  int frame_size = 48000 / 100;
  
  std::cout << "decoding..." << std::endl;
  
  size_t buffer_size = frame_size * 2;
  unsigned char* buffer = static_cast<unsigned char *>(malloc(buffer_size));
  
  do {
    //mpg123_volume(mh, volume);
    
    int encodedFrames = 0;
    
    for(size_t i = 0; i < frames && err == MPG123_OK; ++i) {
      err = mpg123_read(mh, buffer, buffer_size, NULL);
      mc->getAudio()->encodeAudioFrame(reinterpret_cast<const short int*>(buffer), false);
      
      
      ++encodedFrames;
    }
    
    // FIXME: packet queue timing must be handled internally
    boost::this_thread::sleep(boost::posix_time::milliseconds((frames) * 
    10));
  } while (err == MPG123_OK && !song.isStoped());
  
  if (err != MPG123_DONE)
    std::cerr << "Warning: Decoding ended prematurely because: " << (err == 
    MPG123_ERR ? mpg123_strerror(mh) : mpg123_plain_strerror(err)) << std::endl;
  
  std::cout << "finished decoding" << std::endl;
  
  free(buffer);
  mpg123_close(mh);
  mpg123_delete(mh);
  mh = NULL;
  mpg123_exit();

  //playback = false;
  std::cout << ">> play mp3 thread" << std::endl;
}

