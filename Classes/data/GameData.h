#ifndef __WanderingMelody__GameData_h__
#define __WanderingMelody__GameData_h__

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

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
    std::vector<MusicNote> _notes;
};

class KeyNote {
public:
    uint32_t time;  // in ticks
    char tag;
};

class KeyTrack {
public:
    virtual int getWidth() = 0;

    static KeyTrack *create(const std::string &name);
};

template <int N> class KeyTrackBasicKeys : public KeyTrack {
public:
    KeyTrackBasicKeys() { }
    virtual int getWidth() override { return N; }

protected:
    class Note : public KeyNote {
        uint8_t trackIdx;
    };
    std::vector<Note> _notes;
};

class Musician {
public:
    Musician() : _keyTrack(nullptr) { }
    inline void setKeyTrack(KeyTrack *keyTrack) { _keyTrack = keyTrack; }
    inline void allocateMusicTrack(int idx) {
        if (_musicTracks.size() <= idx)
            _musicTracks.resize(idx + 1);
    }
    inline void addTempoChange(uint32_t time, uint16_t tempo) {
        _tempoChanges.push_back({time, tempo});
    }

protected:
    std::vector<std::pair<uint32_t, uint16_t>> _tempoChanges;
    KeyTrack *_keyTrack;
    std::vector<MusicTrack> _musicTracks;
};

class Gig {
public:
    Gig() { }

    enum FileReadResult {
        SUCCESS = 0,
        ERR_FILEOPEN,
        ERR_FILECONTENTS,   // TODO: Add more specific error codes
    };
    FileReadResult init(const std::string &path);

protected:
    std::unordered_map<std::string, std::string> _metadata;
    std::vector<Musician> _musicians;

    FileReadResult initWithStdioFile(FILE *f);
};

#endif
