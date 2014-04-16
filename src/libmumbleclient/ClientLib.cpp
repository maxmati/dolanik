#include "ClientLib.hpp"

#include <iostream>
#include <cassert>

#include <boost/iterator/iterator_concepts.hpp>

#include "Client.hpp"
#include "Logging.hpp"
#include "Settings.hpp"

namespace MumbleClient {

///////////////////////////////////////////////////////////////////////////////

// static
MumbleClientLib* MumbleClientLib::instance_ = NULL;

///////////////////////////////////////////////////////////////////////////////
// MumbleClientLib, private:

MumbleClientLib::MumbleClientLib() {
	CELTCodec *codec;
	
	codec = new CELTCodec070("0.0.0");
	if(codec->isValid())
		codecMap.insert(std::pair<int, CELTCodec*>(codec->getBitstreamVersion(), codec));
	else
		delete codec;

	codec = new CELTCodec011("2.0.0");
	if(codec->isValid())
		codecMap.insert(std::pair<int, CELTCodec*>(codec->getBitstreamVersion(), codec));
	else
		delete codec;
}

MumbleClientLib::~MumbleClientLib() {
	//TODO: memleak (cos sie ten destruktor kurwa nie wykonuje)
	//MAXMATI: Jak mam się kurwa wykonać jak jestem prywatny
	for(std::pair<int, CELTCodec*> codec : codecMap)
		delete codec.second;
	
    delete instance_;
}

///////////////////////////////////////////////////////////////////////////////
// MumbleClientLib, public:

MumbleClientLib* MumbleClientLib::instance() {
    if (instance_ == NULL) {
        instance_ = new MumbleClientLib();
        return instance_;
    }

    return instance_;
}

MumbleClient* MumbleClientLib::NewClient() {
    return new MumbleClient(&io_service_);
}

const std::map<int, CELTCodec*> &MumbleClientLib::getCodecMap() const {
	return codecMap;
}

void MumbleClientLib::Run() {
    io_service_.reset();
    io_service_.run();
}

void MumbleClientLib::Shutdown() {
    ::google::protobuf::ShutdownProtobufLibrary();
}

// static
int32_t MumbleClientLib::GetLogLevel() {
    return logging::GetLogLevel();
}

// static
void MumbleClientLib::SetLogLevel(int32_t level) {
    logging::SetLogLevel(level);
}

}  // namespace MumbleClient
