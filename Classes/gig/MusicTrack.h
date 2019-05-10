#ifndef __WanderingMelody__MusicTrack_h__
#define __WanderingMelody__MusicTrack_h__

#include <cstdint>
#include <vector>

class Soundbank;

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

class MusicTrack {
public:
    MusicTrack() : _soundbank(nullptr) { }

    inline void setSoundbank(Soundbank *bank) { _soundbank = bank; }
    void addNote(const MusicNote &note);
    void triggerNote();

protected:
    Soundbank *_soundbank;
    std::vector<MusicNote> _notes;
};

#endif
