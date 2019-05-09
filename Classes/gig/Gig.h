#ifndef __WanderingMelody__Gig_h__
#define __WanderingMelody__Gig_h__

#include "gig/Musician.h"

#include <string>
#include <unordered_map>
#include <vector>

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
