#include "OpenXLSX/OpenXLSX.h"
#include <cerrno>
#include <cstdio>

#define ERR_INVALIDUSE  1
#define ERR_FILEOPEN    2
#define ERR_FORMAT      3
#define ERR_FILEWRITE   4

/*#define RETURN_ERR(__errcode, __format, ...) do { \
    fprintf(stderr, __format, __VA_ARGS__); \
    return (__errcode); \
}*/

static inline void printEncoded(FILE *f, const char *s)
{
    for (; *s != '\0'; s++) {
        if (*s == '|' || *s == '\\') fputc('\\', f);
        fputc(isspace(*s) ? ' ' : *s, f);
    }
}

static inline void printEncodedIgnoreSpace(FILE *f, const char *s)
{
    for (; *s != '\0'; s++) if (!isspace(*s) || *s == '\n') {
        if (*s == '|' || *s == '\\') fputc('\\', f);
        fputc(*s == '\n' ? ' ' : *s, f);
    }
}

static inline std::pair<int, int> parseTrackTag(const std::string &tag)
{
    if (tag[0] != 'P') return {-1, -1};
    int pos = 1;

    int p = 0;
    while (pos < tag.length() && tag[pos] >= '0' && tag[pos] <= '9')
        p = p * 10 + tag[pos++] - '0';
    if (pos == tag.length()) return {p, -1};    // Player track

    if (tag[pos] != '/' || tag[pos + 1] != 'T') return {-1, -1};
    pos += 2;
    int t = 0;
    while (pos < tag.length() && tag[pos] >= '0' && tag[pos] <= '9')
        t = t * 10 + tag[pos++] - '0';

    return {p, t};
}

static inline int playerTrackWidth(const std::string &setting)
{
    if (setting == "4K") return 4;
    if (setting == "2K") return 2;
    return -1;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s <input> <output>\n", argv[0]);
        return ERR_INVALIDUSE;
    }

    OpenXLSX::XLDocument doc;
    try {
        doc.OpenDocument(argv[1]);
    } catch (...) {
        fprintf(stderr, "File \"%s\" does not exist, "
            "or is not a valid Excel document\n", argv[1]);
        return ERR_FILEOPEN;
    }

    if (!doc.Workbook().WorksheetExists("Meta")) {
        fprintf(stderr, "No workbook named \"Meta\" in the document\n");
        return ERR_FORMAT;
    }
    if (!doc.Workbook().WorksheetExists("Sequence")) {
        fprintf(stderr, "No workbook named \"Sequence\" in the document\n");
        return ERR_FORMAT;
    }

    auto sheetMeta = doc.Workbook().Worksheet("Meta");
    auto sheetSeq = doc.Workbook().Worksheet("Sequence");

    FILE *f = fopen(argv[2], "w");
    if (!f) {
        fprintf(stderr, "Cannot open fiile \"%s\" for writing\n", argv[2]);
        return ERR_FILEWRITE;
    }

    ////// Metadata //////
    int metaItemMaxLen = 0;
    int metaItemCnt = 0;
    for (int i = 1; i <= sheetMeta.RowCount(); i++) {
        const std::string &s = sheetMeta.Cell(i, 1).Value().AsString();
        if (s.empty()) break;
        if (metaItemMaxLen < s.length())
            metaItemMaxLen = s.length();
        metaItemCnt = i;
    }

    for (int i = 1; i <= metaItemCnt; i++) {
        const std::string &s = sheetMeta.Cell(i, 1).Value().AsString();
        printEncoded(f, s.c_str());
        for (int j = 0; j < metaItemMaxLen - s.length(); j++)
            fputc(' ', f);
        fputc('|', f);
        const std::string &t = sheetMeta.Cell(i, 2).Value().AsString();
        printEncoded(f, t.c_str());
        fputc('\n', f);
    }
    fputc('\n', f);
    ////// End of metadata //////

    ////// Track configuration //////
    std::vector<int> trackWidths;
    int trackColPtr = 3;
    while (true) {
        const std::string &s = sheetSeq.Cell(1, trackColPtr).Value().AsString();
        if (s.empty()) break;
        std::pair<int, int> id = parseTrackTag(s);
        if (id.first == -1) {
            fprintf(stderr, "Invalid track tag at (%d, %d): \"%s\"\n",
                1, trackColPtr, s.c_str());
            return ERR_FORMAT;
        }

        const std::string &t = sheetSeq.Cell(3, trackColPtr).Value().AsString();
        if (id.second == -1) {
            // Player track
            int w = playerTrackWidth(t);
            if (w == -1) {
                fprintf(stderr, "Unrecognized setting for P%d: \"%s\"\n",
                    id.first, t.c_str());
                return ERR_FORMAT;
            }
            trackWidths.push_back(w);
            trackColPtr += w;
            fprintf(f, "P%d   |", id.first);
            printEncoded(f, t.c_str());
            fputc('\n', f);
        } else {
            fprintf(f, "P%d/T%d|", id.first, id.second);
            trackColPtr += 2;
            int w = 0;
            while (true) {
                const std::string &nextTrack =
                    sheetSeq.Cell(1, trackColPtr + w).Value().AsString();
                const std::string &argName =
                    sheetSeq.Cell(2, trackColPtr + w).Value().AsString();
                if (!nextTrack.empty() || argName.empty()) break;
                if (w != 0) fputc(' ', f);
                fprintf(f, "%2s", argName.substr(0, 2).c_str());
                w += 1;
            }
            trackWidths.push_back(-w);
            trackColPtr += w;
            fputc('|', f);
            printEncodedIgnoreSpace(f, t.c_str());
            fputc('\n', f);
        }
    }
    fputc('\n', f);
    ////// End of track configuration //////

    ////// Sequencer data //////
    for (int seqRow = 4; ; seqRow++) {
        const std::string &tempoStr = sheetSeq.Cell(seqRow, 1).Value().AsString();
        const std::string &tickStr = sheetSeq.Cell(seqRow, 2).Value().AsString();
        if (tickStr.empty()) break;
        fprintf(f, "%4s|%-4s",
            tempoStr.substr(0, 4).c_str(),
            tickStr.substr(0, 4).c_str());
        int seqCol = 3;
        for (int w : trackWidths) {
            fputc('|', f);
            if (w <= 0) {
                // Music track
                w = -w;
                const std::string
                    &cellKey = sheetSeq.Cell(seqRow, seqCol).Value().AsString(),
                    &cellNote = sheetSeq.Cell(seqRow, seqCol + 1).Value().AsString();
                fputc(cellKey.empty() ? ' ' : cellKey[0], f);
                char ch = cellNote.empty() ? ' ' : cellNote[0];
                fputc(ch, f);
                if (cellNote.length() <= 1) {
                    fputc(ch, f);
                    fputc(ch, f);
                } else {
                    fputc(cellNote.length() == 2 ? '-' : cellNote[1], f);
                    fputc(cellNote.length() == 2 ? cellNote[1] : cellNote[2], f);
                }
                for (int j = 0; j < w; j++) {
                    const std::string &cell =
                        sheetSeq.Cell(seqRow, seqCol + j + 2).Value().AsString();
                    fprintf(f, "%2s", cell.substr(0, 2).c_str());
                }
                seqCol += (w + 2);
            } else {
                // Key track
                for (int j = 0; j < w; j++) {
                    const std::string &cell =
                        sheetSeq.Cell(seqRow, seqCol + j).Value().AsString();
                    fputc(cell.empty() ? ' ' : cell[0], f);
                }
                seqCol += w;
            }
        }
        fputc('\n', f);
    }
    ////// End of sequencer data //////

    if (fclose(f) != 0) {
        fprintf(stderr, "Error during file save: errno %d\n", errno);
        return ERR_FILEWRITE;
    }
    return 0;
}
