#include "gig/MusicianNKeys.h"

#include <cctype>

template <int N> void MusicianNKeys<N>::parseGrid(int32_t time, const char *grid)
{
    for (int i = 0; i < N; i++) if (!isspace(grid[i])) {
        KeyNote n;
        n.time = time;
        n.tag = grid[i];
        n.track = i;
        n.triggered = -1;
        _keyNotes.push_back(n);
        printf("%d %c %d\n", n.time, n.tag, n.track);
    }
}

template <int N> void MusicianNKeys<N>::sendEvent(int message)
{
    if (message == 149) triggerNote(0);
    else if (message == 147) triggerNote(1);
    else if (message == 126) triggerNote(2);
    else if (message == 145) triggerNote(3);
}

template <int N> void MusicianNKeys<N>::triggerNote(uint8_t trackIdx)
{
    for (KeyNote &n : _keyNotes) if (n.track == trackIdx) {
        if (n.time >= _curTick - 120 && n.time <= _curTick + 120)
            n.triggered = _curTick;
    }
}
