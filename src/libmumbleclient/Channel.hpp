#ifndef _LIBMUMBLECLIENT_CHANNEL_H_
#define _LIBMUMBLECLIENT_CHANNEL_H_

#include <boost/weak_ptr.hpp>
#include "Logging.hpp"

namespace MumbleClient {

class Channel {
public:
    Channel(int32_t id_) : id(id_) { }
    ~Channel() { LOG(INFO) << "Channel " << name << " destroyed"; }
    int32_t id;
    boost::weak_ptr<Channel> parent;
    int32_t position;
    bool temporary;
    std::string name;
    std::string description;

private:
    Channel(const Channel&);
    void operator=(const Channel&);
};

}  // namespace MumbleClient

#endif  // CHANNEL_H_
