#include "gig/MusicTrack.h"
#include "audio/Soundbank.h"

void MusicTrack::addNote(const MusicNote &note)
{
    if (note.note != MusicNote::NOCHANGE || note.vel != MusicNote::NOCHANGE ||
        note.pan != MusicNote::NOCHANGE || note.dtune != MusicNote::NOCHANGE)
    {
        _notes.push_back(note);
        printf("%d %c %d %d %d %d\n",
            note.time, note.tag, note.note, note.vel, note.pan, note.dtune);
    }
}

void MusicTrack::triggerNote()
{
    _soundbank->sendNote(_notes.front());
}
