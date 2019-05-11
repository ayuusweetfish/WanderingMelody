#include "gig/Musician.h"

void Musician::startPlay()
{
    _curTick = 0;
    _isPlaying = true;
    _barlines.push_back(0);
    _barlinePtr = 0;
}

void Musician::tick(double dt)
{
    if (!_isPlaying) return;
    int32_t lastTick = (int32_t)_curTick;
    _curTick += dt * _tempoChanges.front().second;
    for (auto &mt : _musicTracks)
        mt.triggerAutoNotes(lastTick, (int32_t)_curTick);
}
