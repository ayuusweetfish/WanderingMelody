#include "gig/MusicTrack.h"
#include "audio/Soundbank.h"

using flags_t = MusicTrack::flags_t;

void MusicTrack::addNote(const MusicNote &note)
{
    if (note.note != MusicNote::NOCHANGE || note.vel != MusicNote::NOCHANGE ||
        note.pan != MusicNote::NOCHANGE || note.dtune != MusicNote::NOCHANGE)
    {
        if (note.tag == '.' && !_notes.empty())
            _notes.back().chained = true;
        _notes.push_back(note);
        //printf("%d %c %d %d %d %d\n",
        //    note.time, note.tag, note.note, note.vel, note.pan, note.dtune);
    }
}

static inline flags_t updateFlags(flags_t f, const MusicNote &n)
{
    if (n.note == MusicNote::NOTE_BREAK)
        f ^= MusicTrack::FLAGS_BREAK;
    return f;
}

void MusicTrack::releaseSoundbank()
{
    if (_soundbank) delete _soundbank;
}

void MusicTrack::updateChainedNote(int32_t actualTime, int index)
{
    if (index + 1 >= _notes.size()) return; // Maybe use an assertion
    _chainedNote = index + 1;
    _chainedNoteTime = _notes[index + 1].time + (actualTime - _notes[index].time);
}

flags_t MusicTrack::triggerNote(int32_t time, int32_t actualTime, char tag)
{
    if (!_soundbank) return 0;
    flags_t flags = 0;
    for (MusicNote &n : _notes)
        if (n.time == time && n.tag == tag) {
            _soundbank->sendNote(n);
            flags = updateFlags(flags, n);
            if (n.chained) updateChainedNote(actualTime, &n - _notes.data());
            else _chainedNote = -1;
        }
    return flags;
}

flags_t MusicTrack::triggerAutoNotes(int32_t start, int32_t end, bool disregardAuto)
{
    if (!_soundbank) return 0;
    flags_t flags = 0;
    for (MusicNote &n : _notes)
        if (n.time >= start && n.time < end &&
            (disregardAuto || n.tag == ' '))
        {
            _soundbank->sendNote(n);
            flags = updateFlags(flags, n);
        }
    if (_chainedNote != -1 && _chainedNoteTime < end) {
        MusicNote &n = _notes[_chainedNote];
        _soundbank->sendNote(n);
        if (n.chained) updateChainedNote(_chainedNoteTime, _chainedNote);
        else _chainedNote = -1;
    }
    return flags;
}
