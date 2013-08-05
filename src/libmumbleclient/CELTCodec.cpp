#include <climits>
#include <list>

#include "CELTCodec.hpp"

#define SAMPLE_RATE 48000

CELTCodec::CELTCodec(const std::string& version) {
	this->valid = false;
	this->mode = nullptr;
	this->version = version;
	this->bitstreamVersion = INT_MIN;
	
	std::list<std::string> filenames;
	filenames.push_back("/usr/lib64/libcelt0.so");
	filenames.push_back("/usr/lib64/libcelt.so");
	filenames.push_back("/usr/lib64/celt.so");
	
	for(std::list<std::string>::iterator it = filenames.begin();
	    it != filenames.end(); ++it) {
		celtLibrary.setFileName(*it + "." + version);
	
		if(celtLibrary.load()) {
			valid = true;
			
			celt_mode_destroy = reinterpret_cast<__typeof__(celt_mode_destroy)>(celtLibrary.resolve("celt_mode_destroy"));
			celt_mode_info = reinterpret_cast<__typeof__(celt_mode_info)>(celtLibrary.resolve("celt_mode_info"));
			celt_encoder_destroy = reinterpret_cast<__typeof__(celt_encoder_destroy)>(celtLibrary.resolve("celt_encoder_destroy"));
			celt_encoder_ctl = reinterpret_cast<__typeof__(celt_encoder_ctl)>(celtLibrary.resolve("celt_encoder_ctl"));
			break;
		}
	}
}

CELTCodec::~CELTCodec() {	
	if(celtLibrary.isLoaded())
		celtLibrary.unload();
}

bool CELTCodec::isValid() const {
	return valid;
}

int CELTCodec::getBitstreamVersion() const {
	if (mode && bitstreamVersion == INT_MIN)
		celt_mode_info(mode, CELT_GET_BITSTREAM_VERSION, reinterpret_cast<celt_int32 *>(&bitstreamVersion));
	
	return bitstreamVersion;
}

CELTCodec070::CELTCodec070(const std::string& version) : CELTCodec(version) {
	if (valid) {
		celt_mode_create = reinterpret_cast<__typeof__(celt_mode_create)>(celtLibrary.resolve("celt_mode_create"));
		celt_encoder_create = reinterpret_cast<__typeof__(celt_encoder_create)>(celtLibrary.resolve("celt_encoder_create"));
		celt_encode = reinterpret_cast<__typeof__(celt_encode)>(celtLibrary.resolve("celt_encode"));
		celt_strerror = reinterpret_cast<__typeof__(celt_strerror)>(celtLibrary.resolve("celt_strerror"));
	
		mode = celt_mode_create(SAMPLE_RATE, SAMPLE_RATE / 100, NULL);
	}
}

CELTEncoder* CELTCodec070::encoderCreate() {
	return celt_encoder_create(mode, 1, NULL);
}

int CELTCodec070::encode(CELTEncoder* st, const celt_int16* pcm, unsigned char* compressed, int nbCompressedBytes) {
	return celt_encode(st, pcm, NULL, compressed, nbCompressedBytes);
}

CELTCodec011::CELTCodec011(const std::string& version): CELTCodec(version) {
	if (valid) {
		celt_mode_create = reinterpret_cast<__typeof__(celt_mode_create)>(celtLibrary.resolve("celt_mode_create"));
		celt_encoder_create_custom = reinterpret_cast<__typeof__(celt_encoder_create_custom)>(celtLibrary.resolve("celt_encoder_create_custom"));
		celt_encode = reinterpret_cast<__typeof__(celt_encode)>(celtLibrary.resolve("celt_encode"));
		celt_strerror = reinterpret_cast<__typeof__(celt_strerror)>(celtLibrary.resolve("celt_strerror"));
	
		mode = celt_mode_create(SAMPLE_RATE, SAMPLE_RATE / 100, NULL);
	}
}

CELTEncoder* CELTCodec011::encoderCreate() {
	return celt_encoder_create_custom(mode, 1, NULL);
}

int CELTCodec011::encode(CELTEncoder* st, const celt_int16* pcm, unsigned char* compressed, int nbCompressedBytes) {
	return celt_encode(st, pcm, SAMPLE_RATE / 100, compressed, nbCompressedBytes);
}
