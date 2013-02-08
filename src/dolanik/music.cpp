/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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


#include "music.h"

#include <boost/thread.hpp>

#include "../libmumbleclient/PacketDataStream.hpp"
#include "../misc.h"

namespace Dolan {

Music::Music(MumbleClient::MumbleClient* _mc)
    :playback(false),
     replayCurrentSongFlag(false),
     volume(0.2),
     mh(NULL),
     mc(_mc),
     kSampleRate(48000)
{
    mc->SetTextMessageCallback(boost::bind(&Music::onTxtMsg,this, _1));
}

Music::~Music()
{

}

double Music::getVolume()
{
  return volume;
}
MusicI::Song Music::getCurrentSong()
{
  return currentSong;
}
uint Music::getCurrentSongLength()
{
  if(mh)
  {
    int length = mpg123_length(mh);
    if(length != MPG123_ERR)
      return length/this->orginalSampleRate;
  }
  return 0;
}


void Music::stop()
{
    playback = false;
    statusComment();
}
void Music::setVolume(double volume)
{
  volume = std::max(volume,0.0);
  volume = std::min(volume,2.0);
  this->volume = volume;
  statusComment();
}

void Music::adjustVolume(double delta)
{
    volume += delta;
    volume = std::max(volume,0.0);
    volume = std::min(volume,2.0);
    statusComment();
}
void Music::replay()
{
    replayCurrentSongFlag = true;
    if(!playback)
    {
        boost::thread(boost::bind(&Music::run,this));
    }
}
void Music::setEqualizer(int band, double amp)
{
    if(band < 0 || band > 31 || amp < 0)
        return;

    if(mh)
        mpg123_eq(mh, MPG123_LR, band, amp);
}
void Music::resetEqualizer()
{
    if(mh)
        mpg123_reset_eq(mh);
}

void Music::clearQueue()
{
  queuedSongs.clear();
}

void Music::play(std::string path, std::string title, std::string album, std::string artist)
{
    Song song;
    song.path = path;
    song.title = title;
    song.album = album;
    song.artist = artist;
    queuedSongs.push_back(song);
    if(!playback)
    {
        boost::thread(boost::bind(&Music::run,this));
    }
    statusComment();
}

std::string Music::genPlaylistString()
{
  std::string result = "";
  for(uint i = 0;i< queuedSongs.size() ;++i)
  {
    Song song = queuedSongs.at(i);
    result += Anal::toStr(i+1) + ") " + song.title + " : " + song.artist + "<br />";
    
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
            msg += "I'm currently playing song : "+ currentSong.title + "(Length: "+ Anal::toStr(getCurrentSongLength()) +"s ) <br />";
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
    if(playback)
        msg += "I'm currently playing song : <br />"
             + currentSong.title +" : " + currentSong.artist  + "(Length: "+ Anal::toStr(getCurrentSongLength()) +"s ) <br />";
    if(!queuedSongs.empty())
      msg += "Playlist: <br />" 
	  + genPlaylistString();
    mc->SetComment(msg);
}

void Music::playMp3(const char* path)
{
    //int celt_version; //MAXMATI: unused

    std::cout << "<< play mp3 thread" << std::endl;

    struct sched_param param;
    param.sched_priority = 1;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    // FIXME(pcgod): 1-6 to match Mumble client
    uint frames = 6;
    int audio_quality = 24000;

    int err = mpg123_init();
    mh = mpg123_new(NULL, &err);
    mpg123_param(mh, MPG123_VERBOSE, 255, 0);
    mpg123_param(mh, MPG123_RVA, MPG123_RVA_MIX, 0);
    mpg123_param(mh, MPG123_ADD_FLAGS, MPG123_MONO_MIX, 0);
    mpg123_param(mh, MPG123_FORCE_RATE, kSampleRate, 0);
    mpg123_open(mh, path);

    long rate = 0;
    int channels = 0, encoding = 0;
    mpg123_getformat(mh, &rate, &channels, &encoding);
    mpg123_format_none(mh);

    this->orginalSampleRate = rate;
    
    if(mpg123_scan(mh) == MPG123_ERR) {
        std::cerr << "Malformed audio frame found; exiting!" << std::endl;
    }
    
    statusComment();

    mpg123_id3v2* id3v2;
    if(mpg123_meta_check(mh) & MPG123_ID3)
        mpg123_id3(mh, NULL, &id3v2);

    /*
      if(id3v2 && id3v2->title) {
        if(id3v2->artist) {
          currentSongTittle = id3v2->artist->p;
          currentSongTittle += " - ";
        }

        currentSongTittle += id3v2->title->p;
        statusComment();//TODO
      }
    */

    rate = kSampleRate;
    channels = MPG123_MONO;
    err = mpg123_format(mh, rate, channels, encoding);

    // FIXME(pcgod): maybe broken for mono MP3s
    int frame_size = kSampleRate / 100;

    CELTCodec *cc = mc->getAudio()->getCeltCodec();
    CELTEncoder *ce = mc->getAudio()->getCeltEncoder();

    cc->celt_encoder_ctl(ce, CELT_SET_PREDICTION(0));
    cc->celt_encoder_ctl(ce, CELT_SET_BITRATE(audio_quality));

    std::cout << "decoding..." << std::endl;

    size_t buffer_size = frame_size * 2;
    unsigned char* buffer = static_cast<unsigned char *>(malloc(buffer_size));

    unsigned char **celtBuffer = new unsigned char*[frames];
    for(uint i = 0; i < frames; ++i)
        celtBuffer[i] = new unsigned char[128];

    int32_t seq = 0;

    do {
        mpg123_volume(mh, volume);

        int encodedFrames = 0;
        int frameLength = std::min(audio_quality / (100 * 8), 127);

        for(size_t i = 0; i < frames && err == MPG123_OK; ++i) {
            err = mpg123_read(mh, buffer, buffer_size, NULL);
            cc->encode(ce, reinterpret_cast<short *>(buffer), celtBuffer[i], frameLength);

            ++encodedFrames;
        }

        char packet[1024];
        char flags = 0;
        flags |= (mc->getAudio()->getMessageType() << 5);
        packet[0] = static_cast<unsigned char>(flags);

        MumbleClient::PacketDataStream pds(packet + 1, 1023);
        seq += frames;
        pds << seq;

        for(int i = 0; i < encodedFrames; ++i) {
            std::string s(reinterpret_cast<const char*>(celtBuffer[i]), frameLength);
            unsigned char head = frameLength;

            if(i < encodedFrames - 1)
                head |= 0x80;

            pds.append(head);
            pds.append(s);
        }

#define TCP 0
#if TCP
        mc->SendRawUdpTunnel(packet, pds.size() + 1);
#else
        mc->SendUdpMessage(packet, pds.size() + 1);
#endif

        boost::this_thread::sleep(boost::posix_time::milliseconds((frames) * 10));
    } while (err == MPG123_OK && playback);

    if (err != MPG123_DONE)
        std::cerr << "Warning: Decoding ended prematurely because: " << (err == MPG123_ERR ? mpg123_strerror(mh) : mpg123_plain_strerror(err)) << std::endl;

    std::cout << "finished decoding" << std::endl;

    free(buffer);
    mpg123_close(mh);
    mpg123_delete(mh);
    mh = NULL;
    mpg123_exit();

    playback = false;
    std::cout << ">> play mp3 thread" << std::endl;
}

void Music::run()
{

    while(!queuedSongs.empty())
    {
        playback = true;

	if(!replayCurrentSongFlag)
	{
	  lastSong = currentSong;
	  currentSong = queuedSongs.front();
	  queuedSongs.pop_front();
	}else
	  replayCurrentSongFlag = false;

        statusComment();
        playMp3(currentSong.path.native().c_str());
        statusComment();
    }
    playback = false;

}

}
