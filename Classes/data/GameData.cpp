#include "GameData.h"
#include <cctype>
#include <cstddef>
#include <cstring>
#include <tuple>

static inline void rtrim(char *s)
{
    int l = strlen(s) - 1;
    while (l >= 0 && isspace(s[l])) s[l--] = '\0';
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

    for (auto p : _metadata)
        printf("%s: %s\n", p.first.c_str(), p.second.c_str());

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
        if (musicianIdx < 1 || musicianIdx > 4) return ERR_FILECONTENTS;
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
        printf("%d\n", offset);

        if (trackIdx == -1) {
            KeyTrack *t = KeyTrack::create(s + i);
            if (t == nullptr) return ERR_FILECONTENTS;
            tracks.push_back({musicianIdx, trackIdx, offset});
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
            offset += (5 + 2 * fields.size());
        }
    }

    return SUCCESS;
}
