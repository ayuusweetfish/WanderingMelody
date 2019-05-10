#ifndef __WanderingMelody__AudioOutput_h__
#define __WanderingMelody__AudioOutput_h__

#include "audio/miniaudio.h"

class AudioOutput {
public:
    AudioOutput();
    ~AudioOutput();

    void startPlayback();

    static inline void createInstance() { if (!_instance) _instance = new AudioOutput(); }
    static inline void destroyInstance() { if (_instance) { delete _instance; _instance = nullptr; } }
    static inline AudioOutput *getInstance() { return _instance; }
    static inline const char *getError() { return getInstance()->_errMsg; }

private:
    const char *_errMsg;

    ma_device _device;

    static AudioOutput *_instance;
};

#endif
