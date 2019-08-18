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

class Gig;

class Musician {
public:
    Musician()
      : _isAutoplay(false), _isAutoscroll(false),
        _curTime(0), _curTick(0), _isPlaying(false),
        _isRehearsal(false), _display(nullptr), _gig(nullptr) { }
    virtual ~Musician();

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
        virtual void refresh() = 0;
    };

    virtual Display *createDisplay() = 0;
    inline Display *getDisplay() { return _display; }

    inline void setGig(Gig *gig, int tag) { _gig = gig; _tag = tag; }
    inline Gig *getGig() { return _gig; }
    inline void setIsAutoplay(bool isAutoplay) { _isAutoplay = isAutoplay; }
    inline void setIsAutoscroll(bool isAutoscroll) { _isAutoscroll = isAutoscroll; }
    inline void setIsCooperative(bool isCooperative) { _isCooperative = isCooperative; }
    inline bool isCooperative() { return _isCooperative; }

    virtual void startPlay();
    virtual void stopPlay();
    virtual void resumePlay();
    inline void refresh() { if (_display) _display->refresh(); }
    virtual void tick(double dt, double lcap = 0, double hcap = INFINITY);
    void jump(double tick);
    void clearTriggered();
    virtual void sendEvent(int message) = 0;
    double getCurTick() { return _curTick; }
    double getRawTick() { return _rawTick; }
    double getOrigTempo() { return _tempoChanges.front().second; }
    double tickToTime(double tick);
    double timeToTick(double time);
    bool isInBreak() { return _isInBreak || _isAutoscroll || _isAutoplay; }

    inline void setIsRehearsal(bool isRehearsal) { _isRehearsal = isRehearsal; }
    inline bool isRehearsal() { return _isRehearsal; }

protected:
    std::vector<KeyNote> _keyNotes;
    std::vector<std::pair<int32_t, uint16_t>> _tempoChanges;
    std::vector<int32_t> _barlines;
    std::vector<MusicTrack> _musicTracks;

    bool _isAutoplay;
    bool _isAutoscroll;
    bool _isCooperative;

    // Fields used during playback
    bool _isPlaying;
    int _tempoChangePtr;
    int _barlinePtr;
    double _curTime, _curTick, _rawTick;    // _curTick is the adjusted value
    bool _isInBreak;
    void processFlags(MusicTrack::flags_t flags);

    Beater _beater;
    void addHit(double time, int32_t noteTick, bool propagateUp = true);

    bool _isRehearsal;

    Display *_display;
    Gig *_gig;
    int _tag;

    friend class Gig;
};

#endif
