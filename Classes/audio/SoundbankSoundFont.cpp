#include "audio/SoundbankSoundFont.h"
#include "audio/AudioOutput.h"

#include <cmath>

SoundbankSoundFont::SoundbankSoundFont(const char *s)
{
    AudioOutput::getInstance()->registerCallback(
        std::bind(&SoundbankSoundFont::render,
            this, std::placeholders::_1, std::placeholders::_2));

    _lastNoteOffTime = -99;
    _isCurNoteOn = false;
}

void SoundbankSoundFont::sendNote(const MusicNote &note)
{
    if (note.note == MusicNote::NOCHANGE) return;
    _lastNote = _curNote;
    _lastNoteOffTime = _lastNote.phase;
    if (note.note == MusicNote::NOTE_OFF) {
        _isCurNoteOn = false;
    } else {
        _curNote.musicNote = note;
        float freq = 440 * powf(2, (note.note - 69) / 12.0f);
        _curNote.angVel = freq / 44100 * M_PI * 2;
        _curNote.phase = 0;
        _isCurNoteOn = true;
    }
}

void SoundbankSoundFont::render(float *output, uint32_t nframes)
{
    if (_isCurNoteOn) {
        for (int i = 0; i < nframes; i++) {
            float x = sinf((_curNote.phase + i) * _curNote.angVel);
            if (_curNote.phase + i < 64)
                x *= (_curNote.phase + i) / 64.0f;
            output[i * 2] += x;
            output[i * 2 + 1] += x;
        }
        _curNote.phase += nframes;
    }
    if (_lastNote.phase - _lastNoteOffTime < 64) {
        // XXX: DRY
        for (int i = 0; i < nframes; i++) {
            float x = sinf((_lastNote.phase + i) * _lastNote.angVel);
            if (_lastNote.phase + i - _lastNoteOffTime < 64)
                x *= (1 - (_lastNote.phase + i - _lastNoteOffTime) / 64.0f);
            else break;
            output[i * 2] += x;
            output[i * 2 + 1] += x;
        }
        _lastNote.phase += nframes;
    }
}
