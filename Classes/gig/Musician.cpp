#include "gig/Musician.h"

void Musician::startPlay()
{
    _curTick = 0;
    _isPlaying = true;
    _speed = _tempoChanges.front().second;
    _accel = 0;
    _barlines.push_back(0);
    _barlinePtr = 0;
}

void Musician::tick(double dt)
{
    if (!_isPlaying) return;
    int32_t lastTick = (int32_t)_curTick;

    _curTick += dt * (_speed + 0.5 * dt * _accel);
    _speed += dt * _accel;

    for (auto &mt : _musicTracks)
        mt.triggerAutoNotes(lastTick, (int32_t)_curTick);
}

void Musician::addHitOffset(double delta)
{
    _accel = delta * 200;
}
