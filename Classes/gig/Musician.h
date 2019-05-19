#ifndef __WanderingMelody__Musician_h__
#define __WanderingMelody__Musician_h__

#include "gig/MusicTrack.h"
#include "gig/Beater.h"

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
    Musician() : _isPlaying(0) { }

    inline void allocateMusicTrack(int idx) {
        if (_musicTracks.size() <= idx)
            _musicTracks.resize(idx + 1);
    }
    inline void addTempoChange(int32_t time, uint16_t tempo) {
        _tempoChanges.push_back({time, tempo});
    }
    inline void addBarline(int32_t time) { _barlines.push_back(time); }
    inline MusicTrack &getMusicTrack(int idx) { return _musicTracks[idx]; }

    virtual void parseGrid(int32_t time, const char *grid) = 0;
    virtual int getWidth() = 0;

    class Display : public cocos2d::Node {
    public:
        Display() { }
    };

    virtual Display *createDisplay() = 0;

    virtual void startPlay();
    virtual void tick(double dt);
    virtual void sendEvent(int message) = 0;
    inline const std::vector<MusicNote *> &getRecentTriggers() {
        return _recentTriggers;
    }
    double getCurTick() { return _curTick; }
    double getOrigTempo() { return _tempoChanges.front().second; }

protected:
    std::vector<KeyNote> _keyNotes;
    std::vector<std::pair<int32_t, uint16_t>> _tempoChanges;
    std::vector<int32_t> _barlines;
    std::vector<MusicTrack> _musicTracks;

    // Fields used during playback
    bool _isPlaying;
    int _tempoChangePtr;
    int _barlinePtr;
    double _curTime, _curTick;
    std::vector<MusicNote *> _recentTriggers;

    Beater _beater;
    void addHit(double noteTick);
};

#endif
