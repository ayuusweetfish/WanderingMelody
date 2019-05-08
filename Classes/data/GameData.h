#ifndef __WanderingMelody__GameData_h__
#define __WanderingMelody__GameData_h__

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class MusicianNode;

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

class KeyNote {
public:
    uint32_t time;  // in ticks
    char tag;
};

class KeyTrack {
public:
    virtual int getWidth() = 0;
    virtual void parseGrid(uint32_t time, const char *grid) = 0;
    virtual MusicianNode *createMusicianNode() = 0;
    // XXX: Should `_note` be moved to this base class?
    virtual void getNotesToDisplay(uint32_t hitpos, std::vector<KeyNote *> &out) = 0;

    static KeyTrack *create(const std::string &name);
};

template <int N> class KeyTrackBasicKeys : public KeyTrack {
public:
    KeyTrackBasicKeys() { }
    virtual int getWidth() override { return N; }
    virtual void parseGrid(uint32_t time, const char *grid) override {
        for (int i = 0; i < N; i++) if (!isspace(grid[i])) {
            KeyTrackBasicKeys<N>::Note n;
            n.time = time;
            n.tag = grid[i];
            n.trackIdx = i;
            _notes.push_back(n);
            printf("%d %c %d\n", n.time, n.tag, n.trackIdx);
        }
    }

    virtual MusicianNode *createMusicianNode() override;
    void getNotesToDisplay(uint32_t hitpos, std::vector<KeyNote *> &out) override;

    class Note : public KeyNote {
    public:
        uint8_t trackIdx;
    };

protected:
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
    inline KeyTrack *getKeyTrack() { return _keyTrack; }
    inline MusicTrack &getMusicTrack(int idx) { return _musicTracks[idx]; }

    void startPlay();
    void tick(double dt);
    void sendEvent(int message);
    inline const std::vector<MusicNote *> &getRecentTriggers() {
        return _recentTriggers;
    }
    uint32_t getTimePositionInTrack();
    inline void getNotesToDisplay(uint32_t hitpos, std::vector<KeyNote *> &out) {
        _keyTrack->getNotesToDisplay(hitpos, out);
    }

    MusicianNode *createMusicianNode();

protected:
    std::vector<std::pair<uint32_t, uint16_t>> _tempoChanges;
    KeyTrack *_keyTrack;
    std::vector<MusicTrack> _musicTracks;

    // Fields used during playback
    int _tempoChangePtr;
    double _curTick;
    std::vector<MusicNote *> _recentTriggers;
};

class Gig {
public:
    Gig() { }

    struct FileReadResult {
        FileReadResult(bool succeeded, const std::string &errMsg) :
            succeeded(succeeded), errMsg(errMsg) { }
        FileReadResult(bool succeeded, const char *errMsg) :
            succeeded(succeeded), errMsg(errMsg) { }
        bool succeeded;
        std::string errMsg;
    };
    FileReadResult init(const std::string &path);

    inline int getMusicianCount() { return _musicians.size(); }
    inline Musician &getMusician(int idx) { return _musicians[idx]; }

protected:
    std::unordered_map<std::string, std::string> _metadata;
    std::vector<Musician> _musicians;

    FileReadResult initWithStdioFile(FILE *f);
};

#endif
