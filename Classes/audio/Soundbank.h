#ifndef __WanderingMelody__Soundbank_h__
#define __WanderingMelody__Soundbank_h__

#include "gig/MusicTrack.h"

#include <string>
#include <unordered_map>

class Soundbank {
public:
    Soundbank() : _valid(true) { }
    virtual ~Soundbank() { }

    bool isValid() { return _valid; }
    virtual void sendNote(const MusicNote &note) = 0;

protected:
    static inline double getArgNumber(
        const std::unordered_map<std::string, std::string> args,
        std::string key, double dfault)
    {
        auto it = args.find(key);
        return (it != args.end() ? std::stod(it->second) : dfault);
    }

    bool _valid;
};

#endif
