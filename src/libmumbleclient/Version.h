#ifndef _VERSION_H
#define _VERSION_H

#define MUMXTEXT(X) #X
#define MUMTEXT(X) MUMXTEXT(X)

#ifndef MUMBLE_VERSION
#define MUMBLE_VERSION 1.2.3
#endif
#ifndef MUMBLE_VERSION
#define MUMBLE_RELEASE "Compiled " __DATE__ " " __TIME__
#else
#define MUMBLE_RELEASE MUMTEXT(MUMBLE_VERSION)
#endif

#include <string>

class MumbleVersion {
  public:
    static unsigned int getRaw(const std::string &version = std::string(MUMTEXT(MUMBLE_VERSION)));
    static bool get(int *major, int *minor, int *patch, const std::string &version = std::string(MUMTEXT(MUMBLE_VERSION)));

    static unsigned int toRaw(int major, int minor, int patch);
    static void fromRaw(unsigned int version, int *major, int *minor, int *patch);
};

#endif
