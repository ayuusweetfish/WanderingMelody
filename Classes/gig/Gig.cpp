#include "gig/Gig.h"
#include "gig/MusicianNKeys.h"
#include "audio/SoundbankSoundFont.h"

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

static inline Musician *createMusicianOfType(const std::string &name)
{
    if (name == "4K") return new MusicianNKeys<4>();
    if (name == "2K") return new MusicianNKeys<2>();
    return nullptr;
}

Gig::FileReadResult Gig::init(const std::string &path)
{
    FILE *f = fopen(path.c_str(), "r");
    if (!f) return FileReadResult(false, "Cannot open file \"" + path + '\"');
    auto ret = this->initWithStdioFile(f);
    fclose(f);
    return ret;
}

Gig::FileReadResult Gig::initWithStdioFile(FILE *f)
{
    _musicians.resize(4);

    char s[256];
    char err[256];
    int lineNum = 0;

#define RET_ERR(__str) do { \
    snprintf(err, sizeof err, "Line %d: %s", lineNum, __str); \
    return FileReadResult(false, err); \
} while (0)

#define RET_ERRF(__format, ...) do { \
    snprintf(err, sizeof err, "Line %d: " __format, lineNum, __VA_ARGS__); \
    return FileReadResult(false, err); \
} while (0)

    ////// Metadata //////
    while (fgets(s, 256, f) != NULL) {
        lineNum++;
        rtrim(s);
        if (s[0] == '\0') break;    // Empty line, the end of metadata

        int lastNonSpace = -1, i;
        for (i = 0; s[i] != '\0' && s[i] != '|'; i++)
            if (!isspace(s[i])) lastNonSpace = i;
        if (s[i] == '\0') RET_ERR("Invalid metadata record: no delimiter '|'");
        if (lastNonSpace == -1) RET_ERR("Invalid metadata record: empty key");

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
        lineNum++;
        rtrim(s);
        if (s[0] == '\0') break;    // Empty line, the end of track setup

        int i = 0;
        if (s[i++] != 'P')
            RET_ERR("Col 1: Invalid track tag: should start with 'P'");
        int musicianIdx = s[i++] - '0';
        int trackIdx = -1;
        if (musicianIdx < 1 || musicianIdx > _musicians.size())
            RET_ERRF("Col %d: Invalid musician index: "
                "should be between 1 and 4", i);
        musicianIdx -= 1;
        if (s[i] == '/') {
            i++;
            if (s[i++] != 'T') RET_ERR("Invalid track tag: 'T' expected");
            trackIdx = 0;
            int iStart = i;
            while (s[i] >= '0' && s[i] <= '9') {
                trackIdx = trackIdx * 10 + s[i++] - '0';
                if (trackIdx > 64) break;
            }
            if (trackIdx < 1 || trackIdx > 64)
                RET_ERRF("Col %d: Invalid track index: "
                    "should between 1 and 64", iStart + 1);
        }
        for (; s[i] != '|'; i++) if (!isspace(s[i]))
            RET_ERRF("Col %d: Invalid track tag: redundant characters", i + 1);
        i++;    // Skip the delimiter

        if (trackIdx == -1) {
            _musicians[musicianIdx] = createMusicianOfType(s + i);
            if (_musicians[musicianIdx] == nullptr)
                RET_ERRF("Col %d: Unrecognized track type: %s", i + 1, s + i);
            tracks.push_back({musicianIdx, -1, offset});
            offset += (1 + _musicians[musicianIdx]->getWidth());
        } else {
            trackIdx -= 1;
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
                        RET_ERRF("Col %d: Unrecognized parameter name: \"%c%c\"",
                            i + 1, s[i], s[i + 1]);
                    }
                    for (int j = 0; j + 1 < fields.size(); j++)
                        if (fields[j] == fields.back())
                            RET_ERRF("Col %d: Duplicate parameter \"%c%c\"",
                                i + 1, s[i], s[i + 1]);
                    i += 3;
                    if (s[i - 1] == '|') break;
                    if (s[i - 1] != ' ')
                        RET_ERRF("Col %d: Parameter name too long", i - 2);
                }
            }
            auto bank = new SoundbankSoundFont(s + i);
            tracks.push_back({musicianIdx, trackIdx, offset, fields});
            _musicians[musicianIdx]->allocateMusicTrack(trackIdx);
            _musicians[musicianIdx]->getMusicTrack(trackIdx).setSoundbank(bank);
            offset += (5 + 2 * fields.size());
        }
    }
    if (tracks.empty()) RET_ERR("No tracks in file");

    ////// Sequence grid //////
    int32_t lastBarEnd = 0, lastTime = 0, time = 0;
    int lastStep = 0;
    while (fgets(s, 256, f) != NULL) {
        lineNum++;

        int l = strlen(s);
        if (s[l] == '\n') l--;
        if (l < offset)
            RET_ERRF("Expected %d characters in row, got %d", offset, l);

        int bpm = gridReadNum(s, 4);
        if (bpm == -1)
            RET_ERR("Col 1: Incorrect format for tempo; "
                "use a right-aligned integer");
        if (bpm == 0 && lastStep == 0)
            RET_ERR("Col 1: Tempo not specified on the first tick");

        if (s[5] == '.') {
            if (lastStep == 0) RET_ERR("Col 6: First row must be step 1");
            int tick = gridReadNum(s + 6, 3);
            if (tick == -1)
                RET_ERR("Col 6: Incorrect number format for tick; "
                    "use a three-digit decimal");
            time = lastBarEnd + (lastStep - 1) * 240 + (int)(tick * 0.240);
            if (time <= lastTime)
                RET_ERR("Col 6: Time should be strictly greater than last row");
        } else {
            int step = gridReadNumR(s + 5, 4);
            if (step == -1)
                RET_ERR("Incorrect number format for step; "
                    "use a left-aligned integer");
            if (step != 1 && step != lastStep + 1) {
                if (lastStep == 0)
                    RET_ERR("Col 6: First row must be step 1");
                else
                    RET_ERRF("Col 6: Step should be either 1 or %d", lastStep + 1);
            }
            if (step == 1) lastBarEnd += lastStep * 240;
            time = lastBarEnd + (step - 1) * 240;
            lastStep = step;
        }

        if (bpm != 0) {
            for (Musician *m : _musicians)
                if (m != nullptr) m->addTempoChange(time, bpm);
        }

        for (const trackLayout &t : tracks) {
            if (t.trackIdx == -1) {
                // Key track
                _musicians[t.musicianIdx]
                    ->parseGrid(time, s + t.gridOffset);
            } else {
                // Music track
                MusicNote n;
                n.time = time;
                n.tag = s[t.gridOffset];
                int noteName = gridReadNote(s + t.gridOffset + 1);
                if (noteName == -1)
                    RET_ERRF("Col %d: Invalid note name: \"%c%c%c\"",
                        t.gridOffset + 2, s[t.gridOffset + 1],
                        s[t.gridOffset + 2], s[t.gridOffset + 3]);
                n.note = noteName;
                int gridPos = t.gridOffset + 4;
                for (size_t memOffset : t.fields) {
                    int val = gridReadNum(s + gridPos, 2, MusicNote::NOCHANGE);
                    if (val == -1)
                        RET_ERRF("Col %d: Incorrect number format: \"%c%c\"",
                            gridPos + 1, s[gridPos], s[gridPos + 1]);
                    *((char *)&n + memOffset) = val;
                    gridPos += 2;
                }
                _musicians[t.musicianIdx]->getMusicTrack(t.trackIdx).addNote(n);
            }
        }

        lastTime = time;
    }

    // Remove unused musicians
    for (int i = 0; i < _musicians.size(); i++)
        if (_musicians[i] == nullptr) {
            _musicians.erase(_musicians.begin() + i);
            i--;
        }

    return FileReadResult(true, "");
}
