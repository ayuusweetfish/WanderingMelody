#include "audio/AudioOutput.h"

#define MINIAUDIO_IMPLEMENTATION
#include "audio/miniaudio.h"

#include <cmath>

AudioOutput *AudioOutput::_instance;

static int phase = 0;

void audioOutputCallback(ma_device *device, float *outbuf, float *inbuf, ma_uint32 nframes)
{
    AudioOutput::getInstance()->render(outbuf, nframes);
}

int AudioOutput::registerCallback(callback_t callback)
{
    _callbacks.push_back(callback);
    return _callbacks.size() - 1;
}

void AudioOutput::removeCallback(int id)
{
}

void AudioOutput::render(float *outbuf, uint32_t nframes)
{
    // Assume outbuf is zeroed out
    for (const auto &f : _callbacks) f(outbuf, nframes);
}

AudioOutput::AudioOutput()
  : _errMsg(nullptr)
{
    ma_device_config deviceConfig;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = 44100;
    deviceConfig.dataCallback = (ma_device_callback_proc)audioOutputCallback;

    if (ma_device_init(NULL, &deviceConfig, &_device) != MA_SUCCESS) {
        _errMsg = "Cannot open playback device";
        return;
    }
}

AudioOutput::~AudioOutput()
{
    ma_device_uninit(&_device);
}

void AudioOutput::startPlayback()
{
    _errMsg = nullptr;
    if (ma_device_start(&_device) != MA_SUCCESS) {
        _errMsg = "Cannot start playback device";
        return;
    }
}
