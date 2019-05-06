#ifndef __WanderingMelody__GameData_h__
#define __WanderingMelody__GameData_h__

#include <cstdint>

class MusicNote {
public:
    char tag;
    uint8_t note;
    uint8_t vel;
    uint8_t pan;
    uint8_t dtune;
};

class Soundbank {
};

class SoundBankAudioFile : public Soundbank {
};

class MusicTrack {
protected:
    Soundbank *_soundbank;
    int _numNotes;
    MusicNote _notes[];
};

class KeyNote {
public:
    uint32_t time;  // in ticks
    char tag;
};

class KeyTrack {
    virtual ~KeyTrack() = 0;
};

template <int N> class KeyTrackBasicKeys : public KeyTrack {
    KeyTrackBasicKeys() : _notes(nullptr) { }

protected:
    class Note : public KeyNote {
        uint8_t trackIdx;
    };
    Note _notes[];
};

class Musician {
protected:
    KeyTrack *_keyTrack;
    int _numMusicTracks;
    MusicTrack _musicTracks[];
};

#endif
