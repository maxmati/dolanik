#include "Audio.hpp"

#include "ClientLib.hpp"
#include "PacketDataStream.hpp"

Audio::Audio() :
	celtCodec(nullptr),
	celtEncoder(nullptr),
	totalFrames(0) {
}

void Audio::flushCheck(const unsigned char buffer[], bool forceFlush) {
	/*
	 * WARNING W CHUJ: zmusic do wyslania ramki, kiedy jestesmy pewni,
	 * ze nie bedziemy juz nic wysylac (zapobieganie, przed ucieciem ostatniej
	 * paczki ramek) [CHYBA DONE <SEX>] */
	
	frameQueue.push(reinterpret_cast<const char*>(buffer));
	
	if(!forceFlush && queuedFrames < audioFrames)
		return;

	unsigned char data[1024];

	int flags = 0; // TODO; send targets
	flags |= (codecMsgType << 5);
	
	data[0] = static_cast<unsigned char>(flags);

	int frames = queuedFrames;
	queuedFrames = 0;
	
	MumbleClient::PacketDataStream pds(data + 1, 1023);
	pds << totalFrames - frames; // seq
	
	for(int i = 0; i < frames; ++i) {
		const std::string &frame = frameQueue.front();
		unsigned char head = static_cast<unsigned char>(frame.size());
		if(i < frames - 1)
			head |= 0x80;
		pds.append(head);
		pds.append(frame.data(), frame.size());
	}
	
	//sendAudioFrame(data, pds);
}

void Audio::encodeAudioFrame(const short *pcm, bool forceFlush) {
	int len;
	unsigned char buffer[512];
	
	if(codecMsgType == MessageType::UDPVoiceCELTAlpha || codecMsgType == MessageType::UDPVoiceCELTBeta)
		len = encodeCELTFrame(pcm, buffer);
	
	if(!len)
		return;
	
	++queuedFrames;
	++totalFrames;
	
	flushCheck(buffer, forceFlush);
}

int Audio::encodeCELTFrame(const short int *pcm, unsigned char *buffer) {
	if(!celtCodec && !celtEncoder) {
		std::cerr << "Couldn\'t encode frame: CELT codec not available" << std::endl;
		return 0;
	}

	celtCodec->celt_encoder_ctl(celtEncoder, CELT_SET_PREDICTION(0));
	celtCodec->celt_encoder_ctl(celtEncoder, CELT_SET_BITRATE(audioBitrate));

	return celtCodec->encode(celtEncoder, pcm, buffer, std::min(static_cast<int>(audioBitrate) / (8 * 100), 127));
}

bool Audio::selectCodec(int alpha, int beta, bool preferAlpha) {
	CELTCodec *newCodec = nullptr;
	const std::map<int, CELTCodec*> &codecMap = MumbleClient::MumbleClientLib::instance()->getCodecMap();

	std::map<int, CELTCodec*>::const_iterator codecIt = preferAlpha ? codecMap.find(alpha) : codecMap.find(beta);
	if(codecIt == codecMap.end()) {
		if(celtCodec && celtEncoder) {
			celtCodec->celt_encoder_destroy(celtEncoder);
			celtEncoder = nullptr;
			celtCodec = nullptr;
		}
		
		std::cerr << "Couldn\'t select codec! (requested bitstream: " << (preferAlpha ? alpha : beta) << ")" << std::endl;

		return false;
	}
	
	newCodec = codecIt->second;
	
	if(newCodec != celtCodec) {
		if(celtCodec && celtEncoder) {
			celtCodec->celt_encoder_destroy(celtEncoder);
			celtEncoder = nullptr;
		}
		
		celtCodec = newCodec;
		codecMsgType = preferAlpha ? MessageType::UDPVoiceCELTAlpha : MessageType::UDPVoiceCELTBeta;
		celtEncoder = newCodec->encoderCreate();
	}
	
	return true;
}

void Audio::setMaxBandwidth(unsigned int bitrate, unsigned int frames) {
	// TODO: max server bandwidth
	audioBitrate = bitrate;
	audioFrames = frames;
}
