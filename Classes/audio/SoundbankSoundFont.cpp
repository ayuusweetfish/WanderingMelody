#include "audio/SoundbankSoundFont.h"
#include "audio/AudioOutput.h"

#include <cmath>

SoundbankSoundFont::SoundbankSoundFont(const char *s)
{
    AudioOutput::getInstance()->registerCallback(
        std::bind(&SoundbankSoundFont::render,
            this, std::placeholders::_1, std::placeholders::_2));
}

void SoundbankSoundFont::sendNote(const MusicNote &note)
{
    printf("trigger %d\n", (int)note.note);
}

void SoundbankSoundFont::render(float *output, uint32_t nframes)
{
    for (int i = 0; i < nframes * 2; i++)
        output[i] += sin((_phase + i / 2) * M_PI / 70) * 0.2;
    _phase += nframes;
}
