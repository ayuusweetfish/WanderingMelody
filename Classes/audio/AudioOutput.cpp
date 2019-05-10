#include "audio/AudioOutput.h"

#define MINIAUDIO_IMPLEMENTATION
#include "audio/miniaudio.h"

#include <cmath>

AudioOutput *AudioOutput::_instance;

static int phase = 0;

void audioOutputCallback(ma_device *device, float *outbuf, float *intbuf, ma_uint32 nframes)
{
    for (int i = 0; i < nframes * 2; i++)
        outbuf[i] = sin((phase + i / 2) * M_PI / 100) * 0.2;
    phase += nframes;
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
