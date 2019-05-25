#include "gig/Musician.h"

#include <algorithm>

void Musician::startPlay()
{
    _curTime = _curTick = 0;
    _isPlaying = true;
    _barlines.push_back(0);
    _barlinePtr = 0;

    _beater = Beater(8, this->getOrigTempo());
}

void Musician::tick(double dt)
{
    if (_display) _display->refresh();

    if (!_isPlaying) return;
    int32_t lastTick = (int32_t)_curTick;

    _curTime += dt;
    _curTick = _beater.getY(_curTime);

    for (auto &mt : _musicTracks)
        mt.triggerAutoNotes(lastTick, (int32_t)_curTick);
}

void Musician::addHit(double noteTick)
{
    _beater.update({_curTime, noteTick}, _beater.getK(_curTime));
}
