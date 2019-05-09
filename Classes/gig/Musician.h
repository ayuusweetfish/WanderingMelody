#ifndef __WanderingMelody__Musician_h__
#define __WanderingMelody__Musician_h__

#include "gig/MusicTrack.h"

#include <utility>
#include <vector>

class Musician {
public:
    Musician() { }

    inline void allocateMusicTrack(int idx) {
        if (_musicTracks.size() <= idx)
            _musicTracks.resize(idx + 1);
    }
    inline void addTempoChange(uint32_t time, uint16_t tempo) {
        _tempoChanges.push_back({time, tempo});
    }
    inline MusicTrack &getMusicTrack(int idx) { return _musicTracks[idx]; }

    void startPlay();
    void tick(double dt);
    void sendEvent(int message);
    inline const std::vector<MusicNote *> &getRecentTriggers() {
        return _recentTriggers;
    }
    uint32_t getTimePositionInTrack();

protected:
    std::vector<std::pair<uint32_t, uint16_t>> _tempoChanges;
    std::vector<MusicTrack> _musicTracks;

    // Fields used during playback
    int _tempoChangePtr;
    double _curTick;
    std::vector<MusicNote *> _recentTriggers;
};

#endif
