#ifndef __WanderingMelody__SoundbankSoundFont_h__
#define __WanderingMelody__SoundbankSoundFont_h__

#include "audio/Soundbank.h"
#include "audio/tsf.h"

#include <cstdint>

class SoundbankSoundFont : public Soundbank {
public:
    SoundbankSoundFont(
        const std::string &s,
        std::unordered_map<std::string, std::string> args);
    virtual ~SoundbankSoundFont() override;

    virtual void sendNote(const MusicNote &note) override;

protected:
    static void render(tsf *f, float *output, uint32_t nframe);

    // tuple<tsf, reference count, callback ID>
    typedef std::unordered_map<std::string, std::tuple<tsf *, int, int>> cache_t;
    static cache_t _cache;

    tsf *_f;
    cache_t::iterator _cacheItr;
    int _channelNum;
    MusicNote _lastNote;
};

#endif
