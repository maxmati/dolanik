#include <iostream>
#include <Ice/Ice.h>
#include <spotify/spotify.h>
#include "spotify/spotifySong.h"
#include <ice/dolanI.h>

#include <dolanik/dolanik.h>
#include <dolanik/music.h>
#include "dolanik/filePlayer.h"

int main()
{

	boost::shared_ptr<Dolanik::Dolanik> dolan(new Dolanik::Dolanik);
	dolan->init();
	uint id = dolan->connect("mumble.maxmati.pl", "64738", "dolanik", "", "./Cert.pem");
	boost::shared_ptr<Dolanik::Music> music = dolan->getMusic(id);
	//Spotify spotify("maxmati", "xxx");
	FilePlayer filePlayer;
	sleep(5);
	FileSong::Ptr ss = filePlayer.createSong("/home/maxmati/Downloads/da.mp3");
	music->play(ss);
	//ss->play(music->mc);
	sleep(1);
	//ss->stop();
	/*
	SpotifySong::Ptr sSong =
	spotify.createSong("spotify:track:1iu2z6r4LLOqyK2auNIrv6");
	
	music->play( sSong );*/
	sleep(1000);
	
	
	//music->play("/home/maxmati/Downloads/da.mp3","a","n","c");
	/*
	int status = 0;
	Ice::CommunicatorPtr ic;
	try {
		ic = Ice::initialize();
		Ice::ObjectAdapterPtr adapter =
			ic->createObjectAdapterWithEndpoints("DolanAdapter", "default -p 10000");
		Ice::ObjectPtr music = new MusicI(dolan.get());
		adapter->add(music, ic->stringToIdentity("Music"));
		Ice::ObjectPtr server = new ServerI(dolan.get());
		adapter->add(server, ic->stringToIdentity("Server"));
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

	*/
	char a;
	std::cin >> a;
	std::cout << a;
	return 0;
}
