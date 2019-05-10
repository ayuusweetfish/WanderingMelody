#ifndef __WanderingMelody__Soundbank_h__
#define __WanderingMelody__Soundbank_h__

#include "gig/MusicTrack.h"

class Soundbank {
public:
    Soundbank() { }

    virtual void sendNote(const MusicNote &note) = 0;
};

#endif
