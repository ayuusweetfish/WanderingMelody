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
    if (_isAutoplay) return;

    int tolTicks = 480;
    for (KeyNote &n : _keyNotes) if (n.track == trackIdx && n.triggered == -1) {
        if (n.time >= _curTick - tolTicks && n.time <= _curTick + tolTicks) {
            n.triggered = _curTick;
            // Handle multiple keys of the same tag
            // TODO: Optimize with data structures
            bool allTriggered = true;
            for (KeyNote &nn : _keyNotes)
                if (nn.time == n.time && nn.triggered == -1 && nn.tag == n.tag) {
                    allTriggered = false;
                    break;
                }
            if (allTriggered)
                for (auto &mt : _musicTracks) {
                    MusicTrack::flags_t f = mt.triggerNote(n.time, n.tag);
                    this->processFlags(f);
                }
            addHit(_curTime, n.time);
            break;
        }
    }
}

template class MusicianNKeys<2>;
template class MusicianNKeys<4>;
