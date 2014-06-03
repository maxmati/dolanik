/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  Mateusz "MaxMati" Nowotynski <maxmati4@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once

#include <string>
#include <deque>
#include <atomic>
#include <chrono>
#include <queue>

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>


#include <dolanik/song.h>
#include <dolanik/resampler.h>

namespace Dolanik{
  class Song;
  
  class Music
  {
  public:
    typedef boost::function<void()> EndOfTrackCallbackType;
    
    Music(MumbleClient::MumbleClient* mc);
    virtual void play(Song::Ptr);///Thread safe
    virtual void stop();///Thread safe
    virtual void adjustVolume(double delta);
    virtual void replay();///Thread safe
    virtual void setEqualizer(int band, double amp);
    virtual void resetEqualizer();
    virtual void clearQueue();///Thread safe
    virtual double getVolume();
    virtual void setVolume(double volume);
    Song::Ptr getCurrentSong();///Thread safe
    
    void setInputFormat(int64_t srcChannelLayout, uint srcRate, AVSampleFormat srcSampleFmt);
    std::chrono::microseconds send(const char** pcm, uint nbSamples);
    
    
    virtual void run();
    virtual ~Music();
  protected:

  private:
    void onTxtMsg(const std::string& text);
    void statusComment();
    std::string genPlaylistString();
    void notifyPlaybackThread();    
    
    std::atomic<bool> playback;
    std::atomic<bool> replayCurrentSongFlag;
    double volume;

    MumbleClient::MumbleClient* mc;
    
    boost::mutex playbackThreadMutex;
    boost::condition playbackThreadCond;
    boost::mutex songsMutex;
    Song::Ptr currentSong;
    std::list<Song::Ptr> history;
    std::deque<Song::Ptr> queue;
    
    const uint sampleRate;
    const size_t sampleSize;
    
    Resampler resampler;
 
  };

}
