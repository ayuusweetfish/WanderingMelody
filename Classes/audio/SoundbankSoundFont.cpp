#include "audio/SoundbankSoundFont.h"
#include "audio/AudioOutput.h"

#define TSF_IMPLEMENTATION
#include "audio/tsf.h"

#include <cmath>

SoundbankSoundFont::cache_t SoundbankSoundFont::_cache;

SoundbankSoundFont::SoundbankSoundFont(
    const std::string &s,
    std::unordered_map<std::string, std::string> args)
{
    auto mapItr = _cache.find(s);

    if (mapItr != _cache.end()) {
        _f = std::get<0>(mapItr->second);
        _channelNum = std::get<1>(mapItr->second)++;
    } else {
        _f.first = tsf_load_filename(s.c_str());
        if (!_f.first) {
            _valid = false;
            return;
        }
        tsf_set_output(_f.first, TSF_STEREO_INTERLEAVED, 44100, 0);
        _channelNum = 0;

        _f.second = new std::mutex();

        int callbackId = AudioOutput::getInstance()->registerCallback(
            std::bind(&SoundbankSoundFont::render,
                _f, std::placeholders::_1, std::placeholders::_2));

        mapItr = _cache.insert({s, {_f, 1, callbackId}}).first;
    }
    _cacheItr = mapItr;

    int presetNum = getArgNumber(args, "Patch", 0);
    double volume = getArgNumber(args, "Gain", 1);
    tsf_channel_set_presetindex(_f.first, _channelNum, presetNum);
    tsf_channel_set_volume(_f.first, _channelNum, volume);

    _lastNote.vel = 50;
    _lastNote.pan = 50;
    _lastNote.dtune = 50;
}

SoundbankSoundFont::~SoundbankSoundFont()
{
    if ((std::get<1>(_cacheItr->second) -= 1) == 0) {
        AudioOutput::getInstance()->removeCallback(std::get<2>(_cacheItr->second));
        tsf_close(_f.first);
        delete _f.second;
        _cache.erase(_cacheItr);
    }
}

void SoundbankSoundFont::sendNote(const MusicNote &note)
{
    std::lock_guard<std::mutex> guard(*_f.second);
    if (note.note == MusicNote::NOTE_OFF) {
        tsf_channel_note_off_all(_f.first, _channelNum);
        printf("Off %d\n", _channelNum);
    } else if (note.note < MusicNote::NOTE_MAX) {
        tsf_channel_note_off_all(_f.first, _channelNum);
        if (note.vel != MusicNote::NOCHANGE) _lastNote.vel = note.vel;
        if (note.pan != MusicNote::NOCHANGE) _lastNote.pan = note.pan;
        if (note.dtune != MusicNote::NOCHANGE) _lastNote.dtune = note.dtune;
        printf("On  %d %d %d\n", _channelNum, note.note, _lastNote.vel);
        tsf_channel_note_on(_f.first, _channelNum, note.note, _lastNote.vel / 50.0f);
    }
}

void SoundbankSoundFont::render(tsfMutex f, float *output, uint32_t nframes)
{
    std::lock_guard<std::mutex> guard(*f.second);
    tsf_render_float(f.first, output, nframes, 1);
    for (int i = 0; i < nframes * 2; i++) output[i] *= 10;
}
