#ifndef __WanderingMelody__MusicTrack_h__
#define __WanderingMelody__MusicTrack_h__

#include <cstdint>
#include <vector>

class MusicNote {
public:
    MusicNote() : tag(' '),
        note(NOCHANGE), vel(NOCHANGE), pan(NOCHANGE), dtune(NOCHANGE) { }

    uint32_t time;
    char tag;
    uint8_t note;
    uint8_t vel;
    uint8_t pan;
    uint8_t dtune;

    static const uint8_t NOCHANGE = 0xff;
    static const uint8_t NOTE_OFF = 0xfe;
};

class Soundbank {
};

class SoundBankAudioFile : public Soundbank {
};

class MusicTrack {
public:
    MusicTrack() : _soundbank(nullptr) { }
    inline void addNote(const MusicNote &note) {
        if (note.note != MusicNote::NOCHANGE || note.vel != MusicNote::NOCHANGE ||
            note.pan != MusicNote::NOCHANGE || note.dtune != MusicNote::NOCHANGE)
        {
            _notes.push_back(note);
            printf("%d %c %d %d %d %d\n",
                note.time, note.tag, note.note, note.vel, note.pan, note.dtune);
        }
    }

protected:
    Soundbank *_soundbank;
    std::vector<MusicNote> _notes;
};

#endif
