#ifndef __WanderingMelody__Gig_h__
#define __WanderingMelody__Gig_h__

#include "gig/Musician.h"

#include <string>
#include <unordered_map>
#include <vector>

class Gig {
public:
    Gig() { }
    ~Gig();

    struct FileReadResult {
        FileReadResult(bool succeeded, const std::string &errMsg) :
            succeeded(succeeded), errMsg(errMsg) { }
        FileReadResult(bool succeeded, const char *errMsg) :
            succeeded(succeeded), errMsg(errMsg) { }
        bool succeeded;
        std::string errMsg;
    };
    FileReadResult init(const std::string &path);
    FileReadResult peek(const std::string &path);

    inline std::unordered_map<std::string, std::string> getMetadata() {
        return _metadata;
    }

    inline int getMusicianCount() { return _musicians.size(); }
    inline Musician *getMusician(int idx) { return _musicians[idx]; }

    void startPlay();
    void tick(float dt);

    void dispatchHit(int idx, double time, int32_t noteTick);

protected:
    std::unordered_map<std::string, std::string> _metadata;
    std::vector<Musician *> _musicians;

    std::string _workingDir;
    FileReadResult initWithStdioFile(FILE *f);
};

#endif
