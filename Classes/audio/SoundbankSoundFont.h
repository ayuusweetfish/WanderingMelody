#ifndef __WanderingMelody__SoundbankSoundFont_h__
#define __WanderingMelody__SoundbankSoundFont_h__

#include "audio/Soundbank.h"
#include "audio/tsf.h"

#include <cstdint>

class SoundbankSoundFont : public Soundbank {
public:
    SoundbankSoundFont(const char *s);
    ~SoundbankSoundFont();

    virtual void sendNote(const MusicNote &note) override;

protected:
    void render(float *output, uint32_t nframe);

    tsf *_f;
    MusicNote _lastNote;
};

#endif
