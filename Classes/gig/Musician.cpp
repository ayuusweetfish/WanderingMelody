#include "gig/Musician.h"
#include "gig/Gig.h"

#include <algorithm>

void Musician::startPlay()
{
    _isAutoscroll |= _isAutoplay;

    _curTime = _curTick = 0;
    _isPlaying = true;
    _barlines.push_back(0);
    _barlinePtr = 0;
    _isInBreak = false;

    _beater = Beater(8, this->getOrigTempo());
}

void Musician::tick(double dt, double lcap, double hcap)
{
    if (!_isPlaying) return;
    int32_t lastTick = (int32_t)_curTick;

    _curTime += dt;
    _rawTick = _beater.getY(_curTime);
    _curTick = std::min(std::max(_rawTick, lcap), hcap);

    for (auto &mt : _musicTracks) {
        MusicTrack::flags_t f =
            mt.triggerAutoNotes(lastTick, (int32_t)_curTick, _isAutoplay);
        this->processFlags(f);
    }
}

void Musician::addHit(double time, int32_t noteTick, bool propagateUp)
{
    if (propagateUp && _isAutoscroll) return;
    _beater.update({time, (double)noteTick}, _beater.getK(_curTime));
    if (propagateUp && _gig) _gig->dispatchHit(_tag, _curTime, noteTick);
}

void Musician::processFlags(MusicTrack::flags_t flags)
{
    if (flags & MusicTrack::FLAGS_BREAK)
        _isInBreak ^= 1;
}

Musician::~Musician()
{
    for (auto &mt : _musicTracks) mt.releaseSoundbank();
}
