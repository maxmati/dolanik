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
  size_t processedSamples = 0;

  switch(codecMsgType) {
    case MessageType::UDPVoiceCELTAlpha:
    case MessageType::UDPVoiceCELTBeta:
      while(processedSamples < len) {
        uint8_t compressedBuffer[512];
        int encoded;

        if(len - processedSamples < SAMPLES_IN_10MS) {
            int16_t pcmBuffer[SAMPLES_IN_10MS];

            memcpy(pcmBuffer, pcm + processedSamples, len - processedSamples);
            memset(pcmBuffer + len - processedSamples, 0, sizeof(pcmBuffer) - len - processedSamples);

            encoded = encodeCELTFrame(pcmBuffer, compressedBuffer);

            processedSamples += len - processedSamples;
        } else {
          encoded = encodeCELTFrame(pcm + processedSamples, compressedBuffer);

          processedSamples += SAMPLES_IN_10MS;
        }

        compressedFrameQueueMutex.lock();
        compressedFrameQueue.push(std::vector<uint8_t>(compressedBuffer, compressedBuffer + encoded));
        compressedFrameQueueMutex.unlock();
      }
      break;
    case MessageType::UDPVoiceOpus:
      break;
    default:
      std::cerr << "Unsupported codec type " << codecMsgType << std::endl;
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

      if(codecMsgType == MessageType::UDPVoiceOpus) {
        // TODO: Opus support
      } else {
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
      }
    } else {
      compressedFrameQueueMutex.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
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