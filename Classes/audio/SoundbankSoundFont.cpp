#include "audio/SoundbankSoundFont.h"
#include "audio/AudioOutput.h"

#define TSF_IMPLEMENTATION
#include "audio/tsf.h"

#include <cmath>

SoundbankSoundFont::SoundbankSoundFont(const char *s)
{
    _f = tsf_load_filename(s);
    if (!_f) return;
    tsf_set_output(_f, TSF_STEREO_INTERLEAVED, 44100, 0);
    tsf_channel_set_bank(_f, 0, 0);

    AudioOutput::getInstance()->registerCallback(
        std::bind(&SoundbankSoundFont::render,
            this, std::placeholders::_1, std::placeholders::_2));

    _lastNote.vel = 50;
    _lastNote.pan = 50;
    _lastNote.dtune = 50;
}

SoundbankSoundFont::~SoundbankSoundFont()
{
    tsf_close(_f);
}

void SoundbankSoundFont::sendNote(const MusicNote &note)
{
    if (note.note == MusicNote::NOTE_OFF) {
        tsf_channel_note_off_all(_f, 0);
    } else {
        tsf_channel_note_off_all(_f, 0);
        if (note.vel != MusicNote::NOCHANGE) _lastNote.vel = note.vel;
        if (note.pan != MusicNote::NOCHANGE) _lastNote.pan = note.pan;
        if (note.dtune != MusicNote::NOCHANGE) _lastNote.dtune = note.dtune;
        tsf_channel_note_on(_f, 0, note.note, _lastNote.vel / 50.0f);
    }
}

void SoundbankSoundFont::render(float *output, uint32_t nframes)
{
    tsf_render_float(_f, output, nframes, 1);
}
