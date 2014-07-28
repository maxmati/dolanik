#include "Audio.hpp"

#include <chrono>

#include "Logging.hpp"
#include "ClientLib.hpp"
#include "PacketDataStream.hpp"

namespace MumbleClient {

Audio::Audio(MumbleClient *mumbleClient) :
  mumbleClient(mumbleClient),
  celtCodec(nullptr),
  celtEncoder(nullptr),
  totalFrames(0) {
    opusEncoder = opus_encoder_create(SAMPLES_IN_10MS * 100, 1, OPUS_APPLICATION_AUDIO, nullptr);
    opus_encoder_ctl(opusEncoder, OPUS_SET_VBR(0));
}

Audio::~Audio() {
  queueCv.notify_all();
}

void Audio::run() {
  running = true;
  queueThread = new std::thread(&Audio::processQueue, this);
}

void Audio::stop() {
  running = false;
  queueCv.notify_all();
  queueThread->join();
  delete queueThread;
}

int Audio::encodeOpusFrame(const int16_t *pcm, uint8_t *buf, size_t len) {
  return opus_encode(opusEncoder, pcm, len, buf, 1024); // FIXME: examine buffer size by bitrate
}

int Audio::encodeCELTFrame(const int16_t *pcm, uint8_t *buf) {
  if(!celtCodec && !celtEncoder) {
    LOG(ERROR) << "CELT encode failed: codec not available";
    return 0;
  }

  celtCodec->celt_encoder_ctl(celtEncoder, CELT_SET_PREDICTION(0));
  celtCodec->celt_encoder_ctl(celtEncoder, CELT_SET_BITRATE(audioBitrate));

  return celtCodec->encode(celtEncoder, pcm, buf, std::min(static_cast<int>(audioBitrate) / (8 * 100), 127));
}

std::chrono::milliseconds Audio::enqueue(const int16_t *pcm, size_t len) {
  const unsigned int REQ_SAMPLES = SAMPLES_IN_10MS * audioFrames;

  pcmQueue.insert(pcmQueue.end(), pcm, pcm + len);

  if(pcmQueue.size() >= REQ_SAMPLES) {
    size_t offset = 0;
    size_t queueSize = 0;

    while(offset < pcmQueue.size() - (pcmQueue.size() % REQ_SAMPLES)) {
      int size;
      std::vector<uint8_t> compressedBuf;
      std::lock_guard<std::mutex> lock(queueMtx);

      if(codecMsgType == MessageType::UDPVoiceOpus) {
        compressedBuf.resize(1024);
        size = encodeOpusFrame(pcmQueue.data() + offset, compressedBuf.data(), REQ_SAMPLES);
        offset += REQ_SAMPLES;
      } else {
        compressedBuf.resize(127);
        size = encodeCELTFrame(pcmQueue.data() + offset, compressedBuf.data());
        offset += SAMPLES_IN_10MS;
      }

      if(size < 0) {
        LOG(ERROR) << "Enqueue failed: encoder returned error";
        return std::chrono::milliseconds();
      }

      // resize to fit the buffer (use size returned from the compressor)
      compressedBuf.resize(size);
      compressedQueue.emplace(compressedBuf);

      queueSize = compressedQueue.size();
    }

    std::move(pcmQueue.begin() + offset, pcmQueue.end(), pcmQueue.begin());
    pcmQueue.resize(pcmQueue.size() - offset);

    queueCv.notify_one();

    if(codecMsgType == MessageType::UDPVoiceOpus)
      queueSize *= audioFrames;

    // returns enqueued data length + 2 frames (for jitter safety)
    return std::chrono::milliseconds(std::max(static_cast<size_t>(0), queueSize * 10 - 2 * (10 * audioFrames)));
  }
  return std::chrono::milliseconds();
}

bool Audio::selectCodec(bool opus, int alpha, int beta, bool preferAlpha) {
  CELTCodec *newCodec = nullptr;
  const std::map<int, CELTCodec*> &codecMap = MumbleClientLib::instance()->getCodecMap();

  if(opus) {
    if(celtCodec) {
      celtCodec->celt_encoder_destroy(celtEncoder);
      celtEncoder = nullptr;
    }

    codecMsgType = MessageType::UDPVoiceOpus;
    return true;
  }

  std::map<int, CELTCodec*>::const_iterator codecIt = preferAlpha ? codecMap.find(alpha) : codecMap.find(beta);
  if(codecIt == codecMap.end()) {
    if(celtCodec && celtEncoder) {
      celtCodec->celt_encoder_destroy(celtEncoder);
      celtEncoder = nullptr;
      celtCodec = nullptr;
    }

    LOG(ERROR) << "Audio: Codec selection failed";
    LOG(INFO) << "(requested bitstream: " << (preferAlpha ? alpha : beta) << ")";;
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

  opus_encoder_ctl(opusEncoder, OPUS_SET_BITRATE(bitrate));
}

void Audio::processQueue() {
  std::unique_lock<std::mutex> lock(queueMtx, std::defer_lock);

  while(running) {
    lock.lock();

    while(running && compressedQueue.size() == 0) {
      LOG(INFO) << "Audio: Queue processor going sleep";
      queueCv.wait(lock);
      LOG(INFO) << "Audio: Queue processor woken up";
    }

    auto start = std::chrono::steady_clock::now();

    if(compressedQueue.size() > 0) {
      uint8_t data[1024];
      int flags = 0 | (codecMsgType << 5);
      PacketDataStream pds(data + 1, 1023);
      size_t frames = 0;

      data[0] = static_cast<uint8_t>(flags);
      pds << totalFrames;

      if(codecMsgType == MessageType::UDPVoiceOpus) {
        const std::vector<uint8_t> &processed = compressedQueue.front();

        pds << processed.size();
        pds.append(reinterpret_cast<const char*>(processed.data()), processed.size());

        compressedQueue.pop();

        frames = audioFrames;
      } else {
        size_t readyFrames = std::min(static_cast<size_t>(audioFrames), compressedQueue.size());

        for(size_t frame = 0; frame < readyFrames; ++frame) {
          const std::vector<uint8_t> &processed = compressedQueue.front();

          uint8_t head = static_cast<uint8_t>(processed.size());
          if(frame < readyFrames - 1)
            head |= 0x80;
          pds.append(head);
          pds.append(reinterpret_cast<const char*>(processed.data()), processed.size());

          compressedQueue.pop();
        }
        frames = readyFrames;
      }
      lock.unlock();

      mumbleClient->SendUdpMessage(reinterpret_cast<const char *>(data), pds.size() + 1);

      totalFrames += frames;

      std::this_thread::sleep_until(start + std::chrono::milliseconds(10 * frames));
    }
  }
}

}