#include "Audio.hpp"

#include "ClientLib.hpp"
#include "PacketDataStream.hpp"

namespace MumbleClient {

Audio::Audio(MumbleClient *mumbleClient) :
	mumbleClient(mumbleClient),
	celtCodec(nullptr),
	celtEncoder(nullptr),
	totalFrames(0),
	queuedFrames(0) {
}

void Audio::flushCheck(const unsigned char buffer[], bool forceFlush) {
	// FIXME: calculate frame size once
	frameQueue.push(std::string(reinterpret_cast<const char*>(buffer), std::min(static_cast<int>(audioBitrate) / (8 * 100), 127)));
	
	if(!forceFlush && queuedFrames < audioFrames)
		return;

	unsigned char data[1024];

	int flags = 0; // TODO; send targets
	flags |= (codecMsgType << 5);
	
	data[0] = static_cast<unsigned char>(flags);

	PacketDataStream pds(data + 1, 1023);
	pds << totalFrames - audioFrames; // seq

	for(unsigned int i = 0; i < audioFrames; ++i) {
  	std::string &frame = frameQueue.front();

		unsigned char head = static_cast<unsigned char>(frame.size());
		if(i < audioFrames - 1)
			head |= 0x80;
		pds.append(head);
		pds.append(frame.data(), frame.size());

		frameQueue.pop();
	}

	mumbleClient->SendUdpMessage(reinterpret_cast<const char *>(data), pds.size() + 1);

	queuedFrames -= audioFrames;
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
	const std::map<int, CELTCodec*> &codecMap = MumbleClientLib::instance()->getCodecMap();

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

}