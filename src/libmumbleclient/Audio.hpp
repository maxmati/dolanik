#ifndef _LIBMUMBLECLIENT_AUDIO_H
#define _LIBMUMBLECLIENT_AUDIO_H

#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>

extern "C" {
  #include <opus/opus.h>
}

#include "CELTCodec.hpp"
#include "PacketDataStream.hpp"
#include "Client.hpp"

namespace MumbleClient {

class MumbleClient;

class Audio {
public:
  Audio(MumbleClient *mumbleClient);
  ~Audio();

  enum MessageType {
    UDPVoiceCELTAlpha,
    UDPPing,
    UDPVoiceSpeex,
    UDPVoiceCELTBeta,
    UDPVoiceOpus
  };

  void run();
  void stop();

  std::chrono::milliseconds enqueue(const int16_t *pcm, size_t len);

  bool selectCodec(bool opus, int alpha, int beta, bool preferAlpha);
  void setMaxBandwidth(unsigned bitrate, unsigned frames);

protected:
  int encodeOpusFrame(const int16_t *pcm, uint8_t *buf, size_t len);
  int encodeCELTFrame(const int16_t *pcm, uint8_t *buf);

  MumbleClient *mumbleClient;

  OpusEncoder *opusEncoder;

  CELTCodec *celtCodec;
  CELTEncoder *celtEncoder;
  MessageType codecMsgType;

  unsigned audioBitrate;
  unsigned audioFrames;
  unsigned totalFrames;

  std::vector<int16_t> pcmQueue;
  std::queue<std::vector<uint8_t>> compressedQueue;

private:
  const size_t SAMPLES_IN_10MS = 480;

  bool running;
  std::thread *queueThread;

  std::mutex queueMtx;

  std::condition_variable queueCv;

  void processQueue();
};

}

#endif
