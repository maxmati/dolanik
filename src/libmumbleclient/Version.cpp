#include "Version.h"

#include <iostream>

#include <boost/regex.hpp>

unsigned int MumbleVersion::getRaw(const std::string &version) {
    int major, minor, patch;

    if (get(&major, &minor, &patch, version))
        return toRaw(major, minor, patch);

    return 0;
}

bool MumbleVersion::get(int *major, int *minor, int *patch, const std::string &version) {
  const boost::regex rx("(\\d+)\\.(\\d+)\\.(\\d+)");
  boost::match_results<std::string::const_iterator> what;

    if (boost::regex_match(version, what, rx)) {
        if (major)
            *major = what[0].str();
        if (minor)
            *minor = what[1].str();
        if (patch)
            *patch = what[2].str();

        return true;
    }
    return false;
}

unsigned int MumbleVersion::toRaw(int major, int minor, int patch) {
    return (major << 16) | (minor << 8) | patch;
}

void MumbleVersion::fromRaw(unsigned int version, int *major, int *minor, int *patch) {
    *major = (version & 0xFFFF0000) >> 16;
    *minor = (version & 0xFF00) >> 8;
    *patch = (version & 0xFF);
}
