#include "Audio.hpp"

#include "ClientLib.hpp"
#include "PacketDataStream.hpp"

namespace MumbleClient {

Audio::Audio(MumbleClient *mumbleClient) :
  mumbleClient(mumbleClient),
  celtCodec(nullptr),
  celtEncoder(nullptr),
  totalFrames(0) {
}

void Audio::run() {
  running = true;
  queueThread = new std::thread(&Audio::processQueue, this);
}

void Audio::stop() {
  running = false;
  queueThread->detach();
  delete queueThread;
}

void Audio::enqueue(const int16_t *pcm, size_t len) {
  const unsigned int REQ_SAMPLES = SAMPLES_IN_10MS * audioFrames; // FIXME: audioFrames is not thread safe

  pcmFrameQueue.insert(pcmFrameQueue.end(), pcm, pcm + len);

  if(pcmFrameQueue.size() >= REQ_SAMPLES) {
    size_t offset = 0;

    while(offset < pcmFrameQueue.size() - (pcmFrameQueue.size() % REQ_SAMPLES)) {
      std::vector<uint8_t> celtBuf(127);

      int size = encodeCELTFrame(pcmFrameQueue.data() + offset, celtBuf.data());
      if(size < 0) {
        std::cerr << "Enqueue CELT compression failed!" << std::endl;
        return;
      }

      celtBuf.resize(size);

      compressedFrameQueueMutex.lock();
      compressedFrameQueue.emplace(celtBuf);
      compressedFrameQueueMutex.unlock();

      offset += SAMPLES_IN_10MS;
    }

    std::move(pcmFrameQueue.begin() + offset, pcmFrameQueue.end(), pcmFrameQueue.begin());
    pcmFrameQueue.resize(pcmFrameQueue.size() - offset);
  }
}

void Audio::processQueue() {
  while(true) {
    compressedFrameQueueMutex.lock();
    if(compressedFrameQueue.size() > 0) {
      uint8_t data[1024];
      int flags = 0 | (codecMsgType << 5);
      PacketDataStream pds(data + 1, 1023);

      data[0] = static_cast<uint8_t>(flags);
      pds << totalFrames - audioFrames; // seq

      size_t readyFrames = std::min(static_cast<size_t>(audioFrames), compressedFrameQueue.size());

      for(size_t frame = 0; frame < readyFrames; ++frame) {
        const std::vector<uint8_t> &processedFrame = compressedFrameQueue.front();

        uint8_t head = static_cast<uint8_t>(processedFrame.size());
        if(frame < readyFrames - 1)
          head |= 0x80;
        pds.append(head);
        pds.append(reinterpret_cast<const char*>(processedFrame.data()), processedFrame.size());

        compressedFrameQueue.pop();
      }
      compressedFrameQueueMutex.unlock();

      mumbleClient->SendUdpMessage(reinterpret_cast<const char *>(data), pds.size() + 1);

      totalFrames += readyFrames;

      std::this_thread::sleep_for(std::chrono::milliseconds(10 * readyFrames));
    } else {
      compressedFrameQueueMutex.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

int Audio::encodeCELTFrame(const int16_t *pcm, uint8_t *buf) {
  if(!celtCodec && !celtEncoder) {
    std::cerr << "Couldn\'t encode frame: CELT codec not available" << std::endl;
    return 0;
  }

  celtCodec->celt_encoder_ctl(celtEncoder, CELT_SET_PREDICTION(0));
  celtCodec->celt_encoder_ctl(celtEncoder, CELT_SET_BITRATE(audioBitrate));

  return celtCodec->encode(celtEncoder, pcm, buf, std::min(static_cast<int>(audioBitrate) / (8 * 100), 127));
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