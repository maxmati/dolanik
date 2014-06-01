#ifndef _LIBMUMBLECLIENT_AUDIO_H
#define _LIBMUMBLECLIENT_AUDIO_H

#include <queue>
#include <thread>
#include <mutex>
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

  void run();
  void stop();

  void enqueue(const int16_t *pcm, size_t len);

  bool selectCodec(int alpha, int beta, bool preferAlpha);
  void setMaxBandwidth(unsigned bitrate, unsigned frames);

protected:
	int encodeCELTFrame(const short int *pcm, unsigned char *buffer);
	// TODO: void encodeOpusFrame();

	MumbleClient *mumbleClient;

	CELTCodec *celtCodec;
	CELTEncoder *celtEncoder;
	MessageType codecMsgType; // do private

	unsigned audioBitrate;
	unsigned audioFrames;
	
	unsigned totalFrames;
	unsigned queuedFrames;

  std::queue<std::vector<uint16_t>> pcmFrameQueue;
  std::queue<std::vector<uint8_t>> compressedFrameQueue;

private:
  const size_t SAMPLES_IN_10MS = 480;

  bool running;

  std::mutex compressedFrameQueueMutex;
  std::thread *queueThread;

  void processQueue();
};

}

#endif
