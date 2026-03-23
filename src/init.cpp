#include <iostream>
#include <functional>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "config.h"

ma_device* init(std::function<void(ma_device*, void*, const void*, ma_uint32)> callback)
{
        static ma_device device;

        static std::function<void(ma_device*, void*, const void*, ma_uint32)> storedCallback;
        storedCallback = callback;

        ma_result result;
        ma_device_config deviceConfig;

        deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = ma_format_f32;
        deviceConfig.playback.channels = channels;
        deviceConfig.sampleRate = sampleRate;
        deviceConfig.dataCallback = [](ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
                storedCallback(pDevice, pOutput, pInput, frameCount);
        };

        result = ma_device_init(NULL, &deviceConfig, &device);
        if (result != MA_SUCCESS)
        {
                std::cerr << "Failed to initialize audio device: " << result << std::endl;
                return nullptr;
        }

        result = ma_device_start(&device);
        if (result != MA_SUCCESS)
        {
                ma_device_uninit(&device);
                std::cerr << "Failed to start audio device: " << result << std::endl;
                return nullptr;
        }

        return &device;
}

void close(ma_device* device)
{
        ma_device_uninit(device);
}
