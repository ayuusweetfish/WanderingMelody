#include "gig/Musician.h"

#include <algorithm>

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
    // TODO: Use better intergration method
    int sign = (_accel > 0 ? +1 : -1);
    _accel = std::max(0.0, _accel * sign - 100 * dt) * sign;

    for (auto &mt : _musicTracks)
        mt.triggerAutoNotes(lastTick, (int32_t)_curTick);
}

void Musician::addHitOffset(double delta)
{
    double weight = 0;
    for (double x : _hitOffsets)
        weight += std::max(0.0, 1 - std::abs(x - delta) / 0.5);
    weight = std::min(weight, 4.0) / 4;
    weight *= weight;

    _accel += delta * 100 * weight;
    printf("%.4lf\n", weight);

    if (_hitOffsets.size() >= 5) _hitOffsets.pop_front();
    _hitOffsets.push_back(delta);
}
