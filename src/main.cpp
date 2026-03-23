#include <iostream>

#define MINIAUDIO_IMPLEMENTATION

#include <miniaudio.h>

float phase = 0.0f;
float frequency = 440.0f;
float volume = 0.2f;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
        float* fOutput = (float*)pOutput;

        for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++)
        {
                float sample = sinf(phase) * volume;

                fOutput[iFrame * 2 + 0] = sample;
                fOutput[iFrame * 2 + 1] = sample;

                phase += 2.0f * 3.14159f * frequency / 44100.0f;

                if (phase > 2.0f * 3.14159f) phase -= 2.0f * 3.14159f;
        }
}

int main()
{
        const int channels = 2;
        const int samples = 44100;

        ma_result result;
        ma_device_config deviceConfig;
        ma_device device;

        deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = ma_format_f32;
        deviceConfig.playback.channels = channels;
        deviceConfig.sampleRate = samples;
        deviceConfig.dataCallback = data_callback;

        result = ma_device_init(NULL, &deviceConfig, &device);
        if (result != MA_SUCCESS)
        {
                return -1;
        }

        result = ma_device_start(&device);
        if (result != MA_SUCCESS)
        {
                ma_device_uninit(&device);
                return -1;
        }

        std::cout << "Press Enter to quit...\n";
        std::cin.get();

        ma_device_uninit(&device);

        return 0;
}
