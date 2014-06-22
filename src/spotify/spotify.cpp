#include "spotify.h"
#include "spotifySong.h"
#include <cassert>

#include <iostream>
#include <chrono>
#include <thread>
#include <boost/bind.hpp>

#include <dolanik/music.h>



#include <stdint.h>
#include <stdlib.h>
const uint8_t g_appkey[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00,
};
const size_t g_appkey_size = sizeof(g_appkey);

static void metadata_updated(sp_session *sess)
{
  std::cout<<"metadata_updated()"<<std::endl;
}

static void play_token_lost(sp_session *sess)
{
  std::cout<<"play_token_lost()"<<std::endl;
}

Spotify* Spotify::instance = nullptr;
Spotify::Spotify(std::string username, std::string password):
  sessionCallbacks(),
  spconfig(),
  session(nullptr),
  notify(false),
  playback(false),
  isLoggedIn(false)
{
  assert(this->instance == nullptr);
  this->instance = this;
  
  sessionCallbacks.logged_in = &Spotify::loggedInWrapper;
  sessionCallbacks.notify_main_thread = &Spotify::notifyMainThreadWrapper;
  sessionCallbacks.music_delivery = &Spotify::musicDeliveryWrapper;
  sessionCallbacks.end_of_track = &Spotify::endOfTrackWrapper;
  sessionCallbacks.metadata_updated = &Spotify::metadataUpdatedWrapper;
  sessionCallbacks.play_token_lost = &Spotify::playTokenLostWrapper;
  
  spconfig.api_version = SPOTIFY_API_VERSION;
  spconfig.cache_location = "tmp";//TODO
  spconfig.settings_location = "tmp";//TODO
  spconfig.application_key = g_appkey;//TODO
  spconfig.application_key_size = g_appkey_size;//TODO
  spconfig.user_agent = "maxmati-dolanik";
  spconfig.callbacks = &sessionCallbacks;
  
  {
    boost::mutex::scoped_lock lock(this->spotifyApiMutex);
    sp_error error;
    error = sp_session_create(&(this->spconfig), &(this->session));
    
    if (error != SP_ERROR_OK) {
      throw std::exception();
    }
    
    error = sp_session_login(this->session, username.c_str(),
			     password.c_str(), 0, NULL);
    if (error != SP_ERROR_OK) {
      throw std::exception();
    }
  }
  
  boost::thread(boost::bind(&Spotify::run, this));

  
}
Spotify::~Spotify()
{
  assert(this->instance != nullptr);
  this->instance = nullptr;
}

boost::shared_ptr< SpotifySong > Spotify::createSong(std::string uri)
{
  sp_track* track = nullptr;
  {
    boost::mutex::scoped_lock apiLock(this->spotifyApiMutex);
    sp_link* link = sp_link_create_from_string(uri.c_str());
    track = sp_link_as_track(link);
  }
  boost::shared_ptr<SpotifySong> song(new SpotifySong(track,this));

  {
    boost::lock_guard<boost::mutex> lock(this->songsMutex);
    this->songs.push_back(song);
  }
  
  return song;
}
uint Spotify::getSampleSize ( sp_sampletype& type )
{
  if(type == SP_SAMPLETYPE_INT16_NATIVE_ENDIAN)
    return 2;
  else
    assert(false);
}

void Spotify::play ( SpotifySong::Ptr song, Dolanik::Music& music )
{
  {
    sp_error error;//FIXME: wait until loaded
    boost::lock_guard<boost::mutex> apiLock(this->spotifyApiMutex);
    error = sp_session_player_load(this->session,song->track);
    error = sp_session_player_play(this->session, true);
  }
  bool initialized = false;
  uint sampleSize;
  this->currentSong = song;
 
  playback = true;
  while(playback)
  {
    
    std::chrono::microseconds sleepTime(0);
    {
      boost::mutex::scoped_lock lock(framesBufferLock);
      if(framesBuffer.size() > 0 && !initialized)
      {
        int64_t channelLayout;
        if(framesBufferFormat.channels == 1)
          channelLayout = AV_CH_LAYOUT_MONO;
        else if(framesBufferFormat.channels == 2)
          channelLayout = AV_CH_LAYOUT_STEREO;
        else
          assert(false);
        
        AVSampleFormat format;
        if(framesBufferFormat.sample_type == SP_SAMPLETYPE_INT16_NATIVE_ENDIAN)
          format = AV_SAMPLE_FMT_S16;
        else
          assert(false);
        
        music.setInputFormat(channelLayout,framesBufferFormat.sample_rate,
                             format);
        
        sampleSize = getSampleSize(framesBufferFormat.sample_type);
        initialized = true;
      }
      if(framesBuffer.size() > 480)
      {
        //FIXME
        char* buffer = new char[480*sampleSize*framesBufferFormat.channels];//480 samples at 48khz gives 10ms
        for(int i = 0; i < 480; ++i)
        {
          boost::shared_ptr<char> frame = framesBuffer.front();
          framesBuffer.pop();
          memcpy(buffer+i*sampleSize*framesBufferFormat.channels, frame.get(),
                 sizeof(char)*sampleSize*framesBufferFormat.channels);
        }
        sleepTime = music.send((const char**)&buffer,480);
        delete[] buffer;
      } else {
        lock.unlock();
        boost::unique_lock<boost::mutex> notifyLock(playbackNotifyMutex);
        playbackNotifyCond.wait(notifyLock);
      }
    }
    std::this_thread::sleep_for(sleepTime);
  }
  currentSong.reset();
  
  
}

void Spotify::stop ( SpotifySong::Ptr song )
{
  assert((currentSong.lock()) == song);
  sp_session_player_play(session, false);
  sp_session_player_unload(session);
  currentSong.reset();
  
  boost::mutex::scoped_lock playbackLock(playbackNotifyMutex);
  playback = false;
  playbackNotifyCond.notify_one();
  
  boost::mutex::scoped_lock lock(framesBufferLock);
  framesBuffer =  std::queue<boost::shared_ptr<char>>();
  

}

void Spotify::endOfTrack(sp_session* sess)
{
  std::cout<<"endOfTrack()"<<std::endl;
  assert(session == sess);
  sp_session_player_unload(session);
}
void Spotify::loggedIn(sp_session* sess, sp_error error)
{
  std::cout<<"loggedIn()"<<std::endl;
  
  if(error == SP_ERROR_OK)
    this->isLoggedIn.store(true);
  else
    throw std::exception();
}
void Spotify::loggedOut(sp_session* sess)
{
  std::cout<<"loggedOut()"<<std::endl;
  this->isLoggedIn.store(false);
}

void Spotify::metadataUpdated(sp_session* sess) //TODO: remove invalid and with full metadata
{
  std::cout<<"metadataUpdated()"<<std::endl;
  boost::lock_guard<boost::mutex> lock(this->songsMutex);
  for(auto &weekSong : this->songs){
    if(boost::shared_ptr<SpotifySong> song = weekSong.lock())
    {
      song->updateMetadata();
    }
  }
}
int Spotify::musicDelivery(sp_session* sess, const sp_audioformat* format, 
			   const void* _frames, int numFrames)
{
  const char* frames = reinterpret_cast<const char*>(_frames);
  
  boost::mutex::scoped_lock lock(framesBufferLock);
  if(framesBuffer.size() > 32768)
    return 0;
  
  if(framesBuffer.empty())
  {
    framesBufferFormat = *format;
    std::cout<<"musicDelivery() creating new framesBuffer format: channels="
      <<format->channels<<" rate="<<format->sample_rate
      <<" type="<<format->sample_type<<std::endl;
  }
  else //FIXME
  {
    assert(framesBufferFormat.channels == format->channels);
    assert(framesBufferFormat.sample_rate == format->sample_rate);
    assert(framesBufferFormat.sample_type == format->sample_type);
  }
  
  uint sampleSize = getSampleSize(framesBufferFormat.sample_type);
  
  for(int i = 0; i<numFrames; ++i)
  {
    boost::shared_ptr<char> frame (new char[format->channels * sampleSize]);
    memcpy(frame.get(), frames, sizeof(char)*format->channels * sampleSize);
    framesBuffer.push(frame);
    frames += format->channels * sampleSize;
  }
//   std::cerr<<"musicDelivery() loaded "<<numFrames<<" frames. Have "
//     <<framesBuffer.size()<<" in buffer."<<std::endl;
  
  playbackNotifyCond.notify_one();
  return numFrames;
}
void Spotify::notifyMainThread(sp_session* sess)
{
  {
    boost::lock_guard<boost::mutex> lock(this->notifyMutex);
    this->notify = true;
  }
  this->notifyCond.notify_all();
}
void Spotify::playTokenLost(sp_session* sess)
{
  std::cout<<"playTokenLost()"<<std::endl;
  assert(session == sess);
  sp_session_player_unload(session);
}

void Spotify::run()
{
  int nextTimeout = 0;
  
  boost::unique_lock< boost::mutex > lock(this->notifyMutex);
  for(;;){
    if(nextTimeout == 0)
      while(!this->notify)
	this->notifyCond.wait(lock);
    else
	this->notifyCond.timed_wait(
	  lock,
	  boost::posix_time::microseconds(nextTimeout)
	);
      
    this->notify = false;
    
    lock.unlock();
    do
    {
      boost::unique_lock< boost::mutex > apiLock(this->spotifyApiMutex);
      sp_session_process_events(this->session, &nextTimeout);
    } while (nextTimeout == 0);
    lock.lock();
    
        
  }
}

void Spotify::endOfTrackWrapper(sp_session* sess)
{
  return Spotify::instance->endOfTrack(sess);
}
void Spotify::loggedInWrapper(sp_session* sess, sp_error error)
{
  return Spotify::instance->loggedIn(sess, error);
}
void Spotify::loggedOutWrapper(sp_session* sess)
{
  return Spotify::instance->loggedOutWrapper(sess);
}
void Spotify::metadataUpdatedWrapper(sp_session* sess)
{
  return Spotify::instance->metadataUpdated(sess);
}
int Spotify::musicDeliveryWrapper(sp_session* sess, const sp_audioformat* format,
				  const void* frames, int numFrames)
{
  return Spotify::instance->musicDelivery(sess, format, frames, numFrames);
}
void Spotify::notifyMainThreadWrapper(sp_session* sess)
{
  return Spotify::instance->notifyMainThread(sess);
}
void Spotify::playTokenLostWrapper(sp_session* sess)
{
  return Spotify::instance->playTokenLost(sess);
}
