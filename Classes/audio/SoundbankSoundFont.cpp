#include "audio/SoundbankSoundFont.h"
#include "audio/AudioOutput.h"

#define TSF_IMPLEMENTATION
#include "audio/tsf.h"

#include <cmath>

std::map<std::string, std::pair<tsf *, int>> SoundbankSoundFont::_cache;

SoundbankSoundFont::SoundbankSoundFont(const std::string &s)
{
    auto mapItr = _cache.find(s);

    if (mapItr != _cache.end()) {
        _f = mapItr->second.first;
        _channelNum = mapItr->second.second++;
    } else {
        _f = tsf_load_filename(s.c_str());
        if (!_f) return;
        tsf_set_output(_f, TSF_STEREO_INTERLEAVED, 44100, 0);
        _channelNum = 0;
        _cache.insert({s, {_f, 1}});
        // TODO: Reference counting, current logic is terrible T-T
        AudioOutput::getInstance()->registerCallback(
            std::bind(&SoundbankSoundFont::render,
                this, std::placeholders::_1, std::placeholders::_2));
    }

    tsf_channel_set_bank(_f, _channelNum, 0);

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
        tsf_channel_note_off_all(_f, _channelNum);
        printf("Off %d\n", _channelNum);
    } else {
        tsf_channel_note_off_all(_f, _channelNum);
        if (note.vel != MusicNote::NOCHANGE) _lastNote.vel = note.vel;
        if (note.pan != MusicNote::NOCHANGE) _lastNote.pan = note.pan;
        if (note.dtune != MusicNote::NOCHANGE) _lastNote.dtune = note.dtune;
        printf("On  %d %d %d\n", _channelNum, note.note, _lastNote.vel);
        tsf_channel_note_on(_f, _channelNum, note.note, _lastNote.vel / 50.0f);
    }
}

void SoundbankSoundFont::render(float *output, uint32_t nframes)
{
    tsf_render_float(_f, output, nframes, 1);
}
