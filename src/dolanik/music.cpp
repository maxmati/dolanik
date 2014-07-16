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

#include <boost/thread.hpp>

#include "music.h"

#include <misc.h>

#include <libmumbleclient/PacketDataStream.hpp>
#include <libmumbleclient/Client.hpp>



namespace Dolanik {

Music::Music(MumbleClient::MumbleClient* _mc)
  :playback(false),
  replayCurrentSongFlag(false),
  volume(0.2),
  mc(_mc),
  sampleRate(48000),
  sampleSize(2),
  resampler(AV_CH_LAYOUT_MONO,sampleRate, AV_SAMPLE_FMT_S16)
{
    mc->SetTextMessageCallback(boost::bind(&Music::onTxtMsg,this, _1));
    mc->getAudio()->setMaxBandwidth(24000, 6);//FIXME
    boost::thread(boost::bind(&Music::run, this));
}

Music::~Music()
{

}

std::chrono::microseconds Music::send ( const char** pcm, uint nbSamples )
{
  uint dstNbSamples = resampler.calculateDstSamplesNumber(nbSamples);
  std::unique_ptr<char[]> data(new char[dstNbSamples*sampleSize]);
  uint nbSamplesAfterResample = resampler.resample(pcm, nbSamples,
                                                   data.get(), dstNbSamples);

  if(nbSamplesAfterResample > 0)
    mc->getAudio()->enqueue(reinterpret_cast<const short int*>(data.get()),
                            nbSamplesAfterResample);

  return std::chrono::microseconds(nbSamplesAfterResample*1000*1000/sampleRate);
}
void Music::setInputFormat ( int64_t srcChannelLayout, uint srcRate, AVSampleFormat srcSampleFmt )
{
  resampler.setInputFormat(srcChannelLayout,srcRate,srcSampleFmt);
}


double Music::getVolume()
{
  return volume;
}
Song::Ptr Music::getCurrentSong()
{
  boost::mutex::scoped_lock lock(songsMutex);
  return currentSong;
}

void Music::stop()
{
    playback = false;
    boost::mutex::scoped_lock lock(songsMutex);
    if(currentSong)
    {
      currentSong->stop();
      history.push_back(currentSong);
      currentSong.reset();
    }
    statusComment();
}
void Music::setVolume(double volume)
{
  volume = std::max(volume,0.01);
  volume = std::min(volume,2.0);
  this->volume = volume;
  statusComment();
  resampler.setVolume(volume);
}

void Music::adjustVolume(double delta)
{
    volume += delta;
    volume = std::max(volume,0.01);
    volume = std::min(volume,2.0);
    statusComment();
    resampler.setVolume(volume);
}
void Music::replay()
{
    replayCurrentSongFlag = true;
    notifyPlaybackThread();
}
void Music::setEqualizer(int band, double amp)//TODO
{
    if(band < 0 || band > 31 || amp < 0)
        return;

    //if(mh)
    //    mpg123_eq(mh, MPG123_LR, band, amp);
}
void Music::resetEqualizer()//TODO
{
    //if(mh)
    //    mpg123_reset_eq(mh);
}

void Music::clearQueue()
{
  boost::mutex::scoped_lock lock(songsMutex);
  queue.clear();
}

void Music::play( Dolanik::Song::Ptr song)
{
  boost::mutex::scoped_lock lock(songsMutex);
  queue.push_back(song);
  notifyPlaybackThread();
  statusComment();
}
void Music::notifyPlaybackThread()
{
  if(!playback)
  {
    boost::mutex::scoped_lock lock(playbackThreadMutex);
    playback = true;
    playbackThreadCond.notify_one();
  }
}

std::string Music::genPlaylistString()
{
  std::string result = "";
  for(uint i = 0;i< queue.size() ;++i)
  {
    boost::shared_ptr<Song> song = queue.at(i);
    result += /*Anal::toStr(i+1) + */ ") " + song->getTitle() + " : " +
      song->getArtist() + "<br />";//FIXME
    
  }
  return result;
}



void Music::onTxtMsg(const std::string& text)
{
    if(text=="stop")
    {
        stop();
    } else if(text=="replay")
    {
        replay();
    } else if(text=="+")
    {
        adjustVolume(+0.1);
    } else if(text=="-")
    {
        adjustVolume(-0.1);
    } else if(text == "status")
    {
        std::ostringstream strs;
        strs << volume *100;
        std::string msg = "Dolanik status: <br />";
        msg += "Current volume = " + strs.str() + "%<br />";
        if(playback)
            msg += "I'm currently playing song : "+ currentSong->getTitle() 
              + "(Length:" /*"+ Anal::toStr(getCurrentSongLength()) +" */ "s ) <br />"; //FIXME
        mc->SendTextMessage("music",msg);
    } else if(text == "playlist")
    {
      mc->SendTextMessage("music", genPlaylistString());
    } else if(text.substr(0, 2) == "eq") {
        int band = -1;
        double amp = -1;

        if(text.substr(3, 5) == "reset") {
            resetEqualizer();
            return;
        }

        std::stringstream ss;
        ss << text.substr(2);
        ss >> band;
        ss >> amp;

        setEqualizer(band,amp);
    } else if(text == "help")
    {
        std::string msg = "Welcome on Dolanik music chanel! <br />";
        msg += "You can upload next song on <a href=\"http://dupa.maxmati.tk/\"> http://dupa.maxmati.tk/ </a> <br />";
        msg += "Commands: <br />";
        msg += "\'+\' - change volume up <br />";
        msg += "\'-\' - change volume down<br />";
        msg += "next - toggle between play or not next song<br />";
        msg += "stop - stop current song<br />";
        msg += "replay - play again last song <br />";
        mc->SendTextMessage("music",msg);
    }
    statusComment();
}

void Music::statusComment()
{
    std::ostringstream strs;
    strs << volume;
    std::string msg = "Dolanik status: <br />";
    msg += "Current volume = " + strs.str() + "<br />";
    if(playback && currentSong)
        msg += "I'm currently playing song : <br />"
          + currentSong->getTitle() +" : " + currentSong->getArtist()
          + "(Length: "+ Anal::toStr(currentSong->getDuration()) +"s ) <br />";
    if(!queue.empty())
      msg += "Playlist: <br />" 
          + genPlaylistString();
    mc->SetComment(msg);
}

void Music::run()
{
  for(;;)
  {
    while((!queue.empty() || replayCurrentSongFlag) && playback)
    {
      if(!replayCurrentSongFlag)
      {
        boost::mutex::scoped_lock lock(songsMutex);
        currentSong = queue.front();
        queue.pop_front();
      }else
        replayCurrentSongFlag = false;
      
      statusComment();
      if(currentSong)
      {
        currentSong->play(*this);
        {
          boost::mutex::scoped_lock lock(songsMutex);
          history.push_back(currentSong);
          currentSong.reset();
        }
      }
      statusComment();
    }
    boost::unique_lock<boost::mutex> lock(playbackThreadMutex);
    playback = false;
    playbackThreadCond.wait(lock);
  }

}

}
