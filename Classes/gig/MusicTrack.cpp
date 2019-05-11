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

void MusicTrack::triggerNote(int32_t time, char tag)
{
    if (!_soundbank) return;
    for (MusicNote &n : _notes)
        if (n.time == time && n.tag == tag)
            _soundbank->sendNote(n);
}

void MusicTrack::triggerAutoNotes(int32_t start, int32_t end)
{
    if (!_soundbank) return;
    for (MusicNote &n : _notes)
        if (n.time >= start && n.time < end && (n.tag == '.' || n.tag == ' '))
            _soundbank->sendNote(n);
}
