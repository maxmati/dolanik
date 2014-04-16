#pragma once

#include <libspotify/api.h>
#include <boost/thread.hpp>
#include <atomic>

#include <queue>

#include <spotify/spotifySong.h>



class SpotifySong;

class Spotify {
public:
  Spotify(std::string username, std::string password);
  ~Spotify();
  
  boost::shared_ptr<SpotifySong> createSong(std::string uri);
  void play( SpotifySong::Ptr song );
  void stop( SpotifySong::Ptr song );
  
  boost::mutex spotifyApiMutex;
protected:
private:
  
  
  void loggedIn(sp_session* sess, sp_error error);
  void loggedOut(sp_session* sess);
  void notifyMainThread(sp_session* sess);
  int musicDelivery( sp_session* sess, const sp_audioformat* format, const void* _frames, int numFrames );
  void endOfTrack(sp_session* sess);
  void metadataUpdated(sp_session* sess);
  void playTokenLost(sp_session* sess);
  
  static void loggedInWrapper(sp_session* sess, sp_error error);
  static void loggedOutWrapper(sp_session* sess);
  static void notifyMainThreadWrapper(sp_session* sess);
  static int musicDeliveryWrapper(sp_session* sess, const sp_audioformat* format,
				  const void *frames, int numFrames);
  static void endOfTrackWrapper(sp_session* sess);
  static void metadataUpdatedWrapper(sp_session* sess);
  static void playTokenLostWrapper(sp_session* sess);
  
  void run();
  
  static Spotify* instance;
  
  std::queue<boost::shared_ptr<char>> framesBuffer;
  sp_audioformat framesBufferFormat;
  boost::mutex framesBufferLock;
  
  sp_session_callbacks sessionCallbacks;
  sp_session_config spconfig;
  sp_session* session;
  
  boost::mutex notifyMutex;
  boost::condition_variable notifyCond;
  bool notify;
  
  boost::mutex playbackNotifyMutex;
  boost::condition_variable playbackNotifyCond;
  bool playback;
    
  boost::mutex songsMutex;
  std::list<boost::weak_ptr<SpotifySong>> songs;
  boost::weak_ptr<SpotifySong> currentSong;
  
  std::atomic_bool isLoggedIn;
  
  

};