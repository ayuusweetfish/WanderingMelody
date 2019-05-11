#ifndef __WanderingMelody__SoundbankSoundFont_h__
#define __WanderingMelody__SoundbankSoundFont_h__

#include "audio/Soundbank.h"
#include "audio/tsf.h"

#include <map>
#include <string>
#include <cstdint>

class SoundbankSoundFont : public Soundbank {
public:
    SoundbankSoundFont(const std::string &s);
    ~SoundbankSoundFont();

    virtual void sendNote(const MusicNote &note) override;

protected:
    void render(float *output, uint32_t nframe);

    static std::map<std::string, std::pair<tsf *, int>> _cache;

    tsf *_f;
    int _channelNum;
    MusicNote _lastNote;
};

#endif
