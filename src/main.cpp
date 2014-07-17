#include <config.h>

#include <iostream>
#include <Ice/Ice.h>

#ifdef USE_SPOTIFY
  #include "spotify/spotify.h"
#endif

#include "ice/dolanI.h"

#include "dolanik/dolanik.h"
#include "dolanik/music.h"
#include "dolanik/filePlayer.h"
#include "dolanik/config.h"


#include <jsoncpp/json/json.h>


int main()
{
  Dolanik::Config* config = Dolanik::Config::getInstance();
  config->parseConfig("/etc/dolanik/config");
  
  boost::shared_ptr<Dolanik::Dolanik> dolan(new Dolanik::Dolanik);
  dolan->init();
  
#ifdef USE_SPOTIFY
  Spotify spotify(
    config->getAsString("spotifyUser"),
    config->getAsString("spotifyPassword"),
    config->getAsString("spotifyKeyFile", "/etc/dolanik/spotify.key")
  );
#endif

  Dolanik::FilePlayer filePlayer;

  int status = 0;
  Ice::CommunicatorPtr ic;
  try {
    ic = Ice::initialize();
    
    Ice::ObjectAdapterPtr adapter =
      ic->createObjectAdapterWithEndpoints("DolanAdapter", "default -p 10000");
      
    Ice::ObjectPtr musicI = new MusicI(dolan.get());
    adapter->add( musicI, ic->stringToIdentity("Music"));
    
    Ice::ObjectPtr serverI = new ServerI(dolan.get());
    adapter->add( serverI, ic->stringToIdentity("Server"));
    
    Ice::ObjectPtr filePlayerI = new FilePlayerI(*(dolan.get()), filePlayer);
    adapter->add( filePlayerI, ic->stringToIdentity("FilePlayer"));
    
#ifdef USE_SPOTIFY
    Ice::ObjectPtr spotifyPlayerI = new SpotifyPlayerI(*(dolan.get()),spotify);
    adapter->add( spotifyPlayerI, ic->stringToIdentity("SpotifyPlayer"));
#endif
    
    adapter->activate();
    
    ic->waitForShutdown();
    
  } catch (const Ice::Exception& e) {
    
    std::cerr << e << std::endl;
    status = 1;
    
  } catch (const char* msg) {
    
    std::cerr << msg << std::endl;
    status = 1;
    
  }
  if (ic) {
    try {
      
      ic->destroy();
      
    } catch (const Ice::Exception& e) {
      
      std::cerr << e << std::endl;
      status = 1;
      
    }
  }

  return status;
}
