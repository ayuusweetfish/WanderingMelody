#ifndef __WanderingMelody__SoundbankSoundFont_h__
#define __WanderingMelody__SoundbankSoundFont_h__

#include "audio/Soundbank.h"

class SoundbankSoundFont : public Soundbank {
public:
    SoundbankSoundFont(const char *s) { }

    virtual void sendNote(const MusicNote &note) override;
};

#endif
