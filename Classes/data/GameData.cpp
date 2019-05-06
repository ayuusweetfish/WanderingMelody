#include "GameData.h"
#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <tuple>

static inline void rtrim(char *s)
{
    int l = strlen(s) - 1;
    while (l >= 0 && isspace(s[l])) s[l--] = '\0';
}

static inline int gridReadNum(const char *s, int len, int dfault = 0)
{
    int i, ret = 0;
    for (i = 0; i < len && isspace(s[i]); i++) { }
    if (i == len) return dfault;
    for (; i < len; i++)
        if (s[i] < '0' || s[i] > '9') return -1;
        else ret = ret * 10 + s[i] - '0';
    return ret;
}

static inline int gridReadNumR(const char *s, int len)
{
    int i, ret = 0;
    for (i = 0; i < len && s[i] >= '0' && s[i] <= '9'; i++)
        ret = ret * 10 + s[i] - '0';
    for (; i < len; i++) if (!isspace(s[i])) return -1;
    return ret;
}

static inline int gridReadNote(const char *s)
{
    if (s[0] == ' ' && s[1] == ' ' && s[2] == ' ') return MusicNote::NOCHANGE;
    if (s[0] == '=' && s[1] == '=' && s[2] == '=') return MusicNote::NOTE_OFF;

    char pitchClass = s[0];
    if (pitchClass < 'A' || pitchClass > 'G') return -1;

    char accidental = s[1];
    accidental = (accidental == '-' ? 0 :
        accidental == '#' ? +1 : accidental == 'b' ? -1 : -2);
    if (accidental == -2) return -1;

    char octave = s[2];
    if (octave < '0' || octave > '9') return -1;

    static const int semitones[7] = { 9, 11, 0, 2, 4, 5, 7 };
    int ret = (octave - '0') * 12 + semitones[pitchClass - 'A'] + accidental;
    return ret + 12;    // For compliance with MIDI
}

KeyTrack *KeyTrack::create(const std::string &name)
{
    if (name == "4K") return new KeyTrackBasicKeys<4>();
    if (name == "2K") return new KeyTrackBasicKeys<2>();
    return nullptr;
}

Gig::FileReadResult Gig::init(const std::string &path)
{
    FILE *f = fopen(path.c_str(), "r");
    if (!f) return ERR_FILEOPEN;
    auto ret = this->initWithStdioFile(f);
    fclose(f);
    return ret;
}

Gig::FileReadResult Gig::initWithStdioFile(FILE *f)
{
    _musicians.resize(4);

    char s[256];

    ////// Metadata //////
    while (fgets(s, 256, f) != NULL) {
        rtrim(s);
        if (s[0] == '\0') break;    // Empty line, the end of metadata

        int lastNonSpace = -1, i;
        for (i = 0; s[i] != '\0' && s[i] != '|'; i++)
            if (!isspace(s[i])) lastNonSpace = i;
        if (s[i] == '\0') return ERR_FILECONTENTS;
        if (lastNonSpace == -1) return ERR_FILECONTENTS;

        s[lastNonSpace + 1] = '\0';
        _metadata[s] = s + i + 1;
    }

    ////// Track setup //////
    struct trackLayout {
        int musicianIdx, trackIdx, gridOffset;
        std::vector<size_t> fields;
    };
    std::vector<trackLayout> tracks;
    int offset = 10;

    while (fgets(s, 256, f) != NULL) {
        rtrim(s);
        if (s[0] == '\0') break;    // Empty line, the end of track setup

        int i = 0;
        if (s[i++] != 'P') return ERR_FILECONTENTS;
        int musicianIdx = s[i++] - '0';
        int trackIdx = -1;
        if (musicianIdx < 1 || musicianIdx > _musicians.size())
            return ERR_FILECONTENTS;
        if (s[i] == '/') {
            i++;
            if (s[i++] != 'T') return ERR_FILECONTENTS;
            trackIdx = 0;
            while (s[i] >= '0' && s[i] <= '9') {
                trackIdx = trackIdx * 10 + s[i++] - '0';
                if (trackIdx > 64) return ERR_FILECONTENTS;
            }
            if (trackIdx == 0) return ERR_FILECONTENTS;
        }
        for (; s[i] != '|'; i++) if (!isspace(s[i])) return ERR_FILECONTENTS;
        i++;    // Skip the delimiter

        if (trackIdx == -1) {
            KeyTrack *t = KeyTrack::create(s + i);
            if (t == nullptr) return ERR_FILECONTENTS;
            tracks.push_back({musicianIdx, -1, offset});
            _musicians[musicianIdx].setKeyTrack(t);
            offset += (1 + t->getWidth());
        } else {
            std::vector<size_t> fields;
            if (s[i] == '|') {
                i++;
            } else {
                while (1) {
                    if (s[i] == 'V' && s[i + 1] == 'l') {
                        fields.push_back(offsetof(MusicNote, vel));
                    } else if (s[i] == 'P' && s[i + 1] == 'n') {
                        fields.push_back(offsetof(MusicNote, pan));
                    } else if (s[i] == 'D' && s[i + 1] == 't') {
                        fields.push_back(offsetof(MusicNote, dtune));
                    } else {
                        return ERR_FILECONTENTS;
                    }
                    if (s[i + 2] == '|') break;
                    if (s[i + 2] != ' ') return ERR_FILECONTENTS;
                    i += 3;
                }
            }
            tracks.push_back({musicianIdx, trackIdx, offset, fields});
            _musicians[musicianIdx].allocateMusicTrack(trackIdx);
            offset += (5 + 2 * fields.size());
        }
    }
    if (tracks.empty()) return ERR_FILECONTENTS;

    ////// Sequence grid //////
    uint32_t lastBarEnd = 0, lastTime = 0, time = 0;
    int lastStep = 0;
    while (fgets(s, 256, f) != NULL) {
        int l = strlen(s);
        if (s[l] == '\n') l--;
        if (l < offset) return ERR_FILECONTENTS;

        int bpm = gridReadNum(s, 4);
        if (bpm == -1) return ERR_FILECONTENTS;
        if (bpm == 0 && lastStep == 0) return ERR_FILECONTENTS;

        if (s[5] == '.') {
            if (lastStep == 0) return ERR_FILECONTENTS;
            int tick = gridReadNum(s + 6, 3);
            if (tick == -1) return ERR_FILECONTENTS;
            time = lastBarEnd + (lastStep - 1) * 240 + (int)(tick * 0.240);
            if (time <= lastTime) return ERR_FILECONTENTS;
        } else {
            int step = gridReadNumR(s + 5, 4);
            if (step == -1) return ERR_FILECONTENTS;
            if (step != 1 && step != lastStep + 1) return ERR_FILECONTENTS;
            if (step == 1) lastBarEnd += lastStep * 240;
            time = lastBarEnd + (step - 1) * 240;
            lastStep = step;
        }

        if (bpm != 0) {
            for (Musician &m : _musicians) m.addTempoChange(time, bpm);
        }

        for (const trackLayout &t : tracks) {
            if (t.trackIdx == -1) {
                // Key track
                _musicians[t.musicianIdx].getKeyTrack()
                    ->parseGrid(time, s + t.gridOffset);
            } else {
                // Music track
                MusicNote n;
                n.time = time;
                n.tag = s[t.gridOffset];
                int noteName = gridReadNote(s + t.gridOffset + 1);
                if (noteName == -1) return ERR_FILECONTENTS;
                n.note = noteName;
                int gridPos = t.gridOffset + 4;
                for (size_t memOffset : t.fields) {
                    int val = gridReadNum(s + gridPos, 2, MusicNote::NOCHANGE);
                    if (val == -1) return ERR_FILECONTENTS;
                    *((char *)&n + memOffset) = val;
                    gridPos += 2;
                }
                _musicians[t.musicianIdx].getMusicTrack(t.trackIdx).addNote(n);
            }
        }

        lastTime = time;
    }

    return SUCCESS;
}
