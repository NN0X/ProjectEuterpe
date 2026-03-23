#include <iostream>
#include <cmath>

#include <miniaudio.h>

#include "init.h"

void synthFunction(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
        static float phase = 0.0f;
        static float frequency = 440.0f;
        static float volume = 0.5f;

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
        ma_device* device = init(synthFunction);

        bool isRunning = true;
        while (isRunning)
        {
        }

        close(device);

        return 0;
}
