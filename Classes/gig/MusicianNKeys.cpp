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
