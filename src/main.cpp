#include <config.h>

#include <iostream>
#include <Ice/Ice.h>

#ifdef USE_SPOTIFY
  #include <spotify/spotify.h>
  #include "spotify/spotifySong.h"
#endif

#include <ice/dolanI.h>

#include <dolanik/dolanik.h>
#include <dolanik/music.h>
#include "dolanik/filePlayer.h"



int main()
{

	boost::shared_ptr<Dolanik::Dolanik> dolan(new Dolanik::Dolanik);
	dolan->init();
	uint id = dolan->connect("mumble.maxmati.pl", "64738", "dolanik", "", "./cert.pem");
	boost::shared_ptr<Dolanik::Music> music = dolan->getMusic(id);
	sleep(5);
#ifdef USE_SPOTIFY
  Spotify spotify("maxmati", "xxx");
  SpotifySong::Ptr sSong =
    spotify.createSong("spotify:track:7DFNE7NO0raLIUbgzY2rzm");
  music->play( sSong );
#endif
    
  Dolanik::FilePlayer filePlayer;
  Dolanik::FileSong::Ptr ss = filePlayer.createSong("./sample.mp3");
	music->play(ss);
	//ss->play(music->mc);
	//sleep(1);
	//ss->stop();


	//sleep(2);
	//sleep(1000);
	
	
	//music->play("/home/maxmati/Downloads/da.mp3","a","n","c");
	
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

	
	char a;
	std::cin >> a;
	std::cout << a;
	return 0;
}
