#ifndef __WanderingMelody__MusicTrack_h__
#define __WanderingMelody__MusicTrack_h__

#include <cstdint>
#include <vector>

class Soundbank;

class MusicNote {
public:
    MusicNote() : tag(' '),
        note(NOCHANGE), vel(NOCHANGE), pan(NOCHANGE), dtune(NOCHANGE) { }

    int32_t time;
    char tag;
    uint8_t note;
    uint8_t vel;
    uint8_t pan;
    uint8_t dtune;

    static const uint8_t NOTE_MAX = 0x80;
    static const uint8_t NOCHANGE = 0xff;
    static const uint8_t NOTE_OFF = 0xfe;
    static const uint8_t NOTE_BREAK = 0xfd;
};

class MusicTrack {
public:
    MusicTrack() : _soundbank(nullptr) { }

    typedef short flags_t;
    static const int FLAGS_BREAK = 1;

    inline void setSoundbank(Soundbank *bank) { _soundbank = bank; }
    void releaseSoundbank();
    void addNote(const MusicNote &note);
    flags_t triggerNote(int32_t time, char tag);
    flags_t triggerAutoNotes(int32_t start, int32_t end, bool disregardAuto = false);

protected:
    Soundbank *_soundbank;
    std::vector<MusicNote> _notes;
};

#endif
