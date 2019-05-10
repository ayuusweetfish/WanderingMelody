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

template <int N> void MusicianNKeys<N>::setKeyMapping(const int keyCodes[N])
{
    for (int i = 0; i < N; i++) _keyMapping[i] = keyCodes[i];
}

template <int N> void MusicianNKeys<N>::startPlay()
{
    Musician::startPlay();
    for (int i = 0; i < N; i++) _isKeyDown[i] = false;
}

template <int N> void MusicianNKeys<N>::sendEvent(int message)
{
    for (int i = 0; i < N; i++)
        if (_keyMapping[i] == message) {
            _isKeyDown[i] = true;
            triggerNote(i);
        } else if (_keyMapping[i] + 1024 == message) {
            _isKeyDown[i] = false;
        }
}

template <int N> void MusicianNKeys<N>::triggerNote(uint8_t trackIdx)
{
    for (KeyNote &n : _keyNotes) if (n.track == trackIdx) {
        if (n.time >= _curTick - 120 && n.time <= _curTick + 120) {
            n.triggered = _curTick;
            _musicTracks[0].triggerNote();
        }
    }
}

template class MusicianNKeys<2>;
template class MusicianNKeys<4>;
