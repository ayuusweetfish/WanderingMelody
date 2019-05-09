#include "gig/Musician.h"

void Musician::startPlay()
{
    _curTick = 0;
}

void Musician::tick(double dt)
{
    _curTick += dt * 240;
}

void Musician::sendEvent(int message)
{
}
