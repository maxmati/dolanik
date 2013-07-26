#pragma once

#include "dolan.h"
#include <boost/shared_ptr.hpp>
#include <dolanik/music.h>

class MusicI : public Dolan::Music {
public:
    MusicI(boost::shared_ptr<Dolanik::Music> music);
    virtual void stop(const Ice::Current&);
    virtual Ice::Double adjustVolume(Ice::Double, const Ice::Current&);
    virtual void replay(const Ice::Current&);
    virtual void setEqualizer(Ice::Int, Ice::Double, const Ice::Current&);
    virtual void resetEqualizer(const Ice::Current&);
    virtual void clearQueue(const Ice::Current&);
    virtual Ice::Double getVolume(const Ice::Current&);
    virtual void setVolume(Ice::Double, const Ice::Current&);
    virtual void play(const Dolan::Song&, const Ice::Current&);
    virtual Dolan::Song getCurrentSong(const Ice::Current&);
private:
	boost::shared_ptr<Dolanik::Music> music;
};

