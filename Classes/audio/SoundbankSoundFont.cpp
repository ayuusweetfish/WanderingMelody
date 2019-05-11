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
}

void SoundbankSoundFont::render(float *output, uint32_t nframes)
{
}
