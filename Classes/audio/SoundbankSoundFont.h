#ifndef __WanderingMelody__SoundbankSoundFont_h__
#define __WanderingMelody__SoundbankSoundFont_h__

#include "audio/Soundbank.h"
#include "audio/tsf.h"

#include <cstdint>
#include <mutex>
#include <utility>

class SoundbankSoundFont : public Soundbank {
public:
    SoundbankSoundFont(
        const std::string &s,
        std::unordered_map<std::string, std::string> args);
    virtual ~SoundbankSoundFont() override;

    virtual void sendNote(const MusicNote &note) override;

protected:
    typedef std::pair<tsf *, std::mutex *> tsfMutex;

    static void render(tsfMutex f, float *output, uint32_t nframe);

    // tuple<tsf, reference count, callback ID>
    typedef std::unordered_map<std::string, std::tuple<tsfMutex, int, int>> cache_t;
    static cache_t _cache;

    tsfMutex _f;
    cache_t::iterator _cacheItr;
    int _channelNum;
    MusicNote _lastNote;
};

#endif
