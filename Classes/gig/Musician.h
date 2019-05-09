#ifndef __WanderingMelody__Musician_h__
#define __WanderingMelody__Musician_h__

#include "gig/MusicTrack.h"

#include "cocos2d.h"
#include <utility>
#include <vector>

class KeyNote {
public:
    uint32_t time;  // in ticks
    char tag;
    uint8_t track;
    int32_t triggered;
};

class Musician {
public:
    Musician() { }

    inline void allocateMusicTrack(int idx) {
        if (_musicTracks.size() <= idx)
            _musicTracks.resize(idx + 1);
    }
    inline void addTempoChange(int32_t time, uint16_t tempo) {
        _tempoChanges.push_back({time, tempo});
    }
    inline MusicTrack &getMusicTrack(int idx) { return _musicTracks[idx]; }

    virtual void parseGrid(int32_t time, const char *grid) = 0;
    virtual int getWidth() = 0;

    class Display : public cocos2d::Node {
    public:
        Display() { }
    };

    virtual Display *createDisplay() = 0;

    void startPlay();
    void tick(double dt);
    void sendEvent(int message);
    inline const std::vector<MusicNote *> &getRecentTriggers() {
        return _recentTriggers;
    }
    double getCurTick() { return _curTick; }

protected:
    std::vector<KeyNote> _keyNotes;
    std::vector<std::pair<int32_t, uint16_t>> _tempoChanges;
    std::vector<MusicTrack> _musicTracks;

    // Fields used during playback
    int _tempoChangePtr;
    double _curTick;
    std::vector<MusicNote *> _recentTriggers;
};

#endif
