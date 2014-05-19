#ifndef _LIBMUMBLECLIENT_AUDIO_H
#define _LIBMUMBLECLIENT_AUDIO_H

#include <queue>
#include <boost/concept_check.hpp>

#include "CELTCodec.hpp"
#include "PacketDataStream.hpp"
#include "Client.hpp"

namespace MumbleClient {

class MumbleClient;

class Audio {
public:
	Audio(MumbleClient *mumbleClient);
	
	enum MessageType {
		UDPVoiceCELTAlpha,
		UDPPing,
		UDPVoiceSpeex,
		UDPVoiceCELTBeta,
		UDPVoiceOpus
	};

	void encodeAudioFrame(const short *pcm, bool forceFlush);
	bool selectCodec(int alpha, int beta, bool preferAlpha);
	void setMaxBandwidth(unsigned bitrate, unsigned frames);

	//tymczasowo
	CELTCodec *getCeltCodec() const { return celtCodec; };
	CELTEncoder *getCeltEncoder() const { return celtEncoder; };
	MessageType getMessageType() const { return codecMsgType; };
	
protected:
	int encodeCELTFrame(const short int *pcm, unsigned char *buffer);
	// TODO: void encodeOpusFrame();
	void flushCheck(const unsigned char buffer[], bool forceFlush);

	MumbleClient *mumbleClient;

	CELTCodec *celtCodec;
	CELTEncoder *celtEncoder;
	MessageType codecMsgType; // do private

	unsigned audioBitrate;
	unsigned audioFrames;
	
	unsigned totalFrames;
	unsigned queuedFrames;

	std::queue<std::string> frameQueue; // do private
};

}

#endif