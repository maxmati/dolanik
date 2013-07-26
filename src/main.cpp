#include <iostream>
#include <Ice/Ice.h>
#include "ice/dolanI.h"

#include "dolanik/dolan.h"




int  main()
{
	Dolanik::Dolan dolan;
	dolan.init();
	
	int status = 0;
	Ice::CommunicatorPtr ic;
	try {
		ic = Ice::initialize();
		Ice::ObjectAdapterPtr adapter =
			ic->createObjectAdapterWithEndpoints("DolanAdapter", "default -p 10000");
		Ice::ObjectPtr object = new MusicI(dolan.getMusic());
		adapter->add(object, ic->stringToIdentity("Music"));
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
	return status;
}
