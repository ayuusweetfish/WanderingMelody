#ifndef __WanderingMelody__GameData_h__
#define __WanderingMelody__GameData_h__

#include <cstdint>

class MusicNote {
public:
    char marker;
    uint8_t note;
    union {
        uint8_t vel;
        uint8_t arg;
    };
    union {
        int8_t pan;
        char flag;
    };
};

class Soundbank {
};

class MusicTrack {
protected:
    Soundbank *_soundbank;
    MusicNote *_notes;
};

class KeyNote {
protected:
    uint32_t _time; // in ticks
    MusicNote *_musNote;
};

class KeyTrack {
    virtual ~KeyTrack() = 0;
};

template <int N> class KeyTrackFixedW : public KeyTrack {
    KeyTrackFixedW() : _row(nullptr) { }
    void init(int rows) { _row = new Row[rows]; }
    ~KeyTrackFixedW() { if (_row) delete[] _row; }

protected:
    MusicTrack *_musTracks;

    struct Row {
        uint32_t time;
        char cell[N];
    };
    Row *_row;
};

template <int N> class KeyTrackBasicKeys : public KeyTrackFixedW<N> {
};

class Musician {
protected:
    KeyTrack *_keyTrack;
};

#endif
