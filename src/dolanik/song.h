#pragma once
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace MumbleClient
{
class MumbleClient;
}
namespace Dolanik{
class Musik;
  
class Song
{
public:
  typedef boost::shared_ptr<Song> Ptr;
  virtual std::string getArtist() = 0;
  virtual std::string getAlbum() = 0;
  virtual std::string getTitle() = 0;
  virtual int getDuration() = 0;
  virtual void play(MumbleClient::MumbleClient* mc) = 0;
  virtual void stop() = 0;
protected:
private:
};
}