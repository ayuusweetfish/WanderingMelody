#ifndef __WanderingMelody__SoundbankPureSine_h__
#define __WanderingMelody__SoundbankPureSine_h__

#include "audio/Soundbank.h"

#include <cstdint>
#include <mutex>

class SoundbankPureSine : public Soundbank {
public:
    SoundbankPureSine(const char *s);

    virtual void sendNote(const MusicNote &note) override;

protected:
    void render(float *output, uint32_t nframe);
    struct {
        MusicNote musicNote;
        float angVel;
        int phase;
    } _lastNote, _curNote;
    int _lastNoteOffTime;
    bool _isCurNoteOn;

    std::mutex _mutex;
};

#endif
