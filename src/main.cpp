#include <iostream>
#include <cmath>
#include <cctype>
#include <unordered_map>
#include <unistd.h>

#define RGFW_IMPLEMENTATION
#include "RGFW.h"

#include <miniaudio.h>

#include "init.h"

const std::unordered_map<char, float> keyToNote = {
        {'q', 261.63f},
        {'w', 293.66f},
        {'e', 329.63f},
        {'r', 349.23f},
        {'t', 392.00f},
        {'y', 440.00f},
        {'u', 493.88f},
        {'i', 523.25f},
        {'o', 587.33f},
        {'p', 659.25f}
};

std::unordered_map<char, bool> isKeyToggledMap = {
        {'q', false},
        {'w', false},
        {'e', false},
        {'r', false},
        {'t', false},
        {'y', false},
        {'u', false},
        {'i', false},
        {'o', false},
        {'p', false}
};

void synthFunction(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
        (void)pDevice;
        (void)pInput;

        static std::unordered_map<char, float> phaseMap;
        static float volumePerNote = 0.2f;

        float* fOutput = (float*)pOutput;

        for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++)
        {
                float rawMix = 0.0f;

                for (const auto& [key, freq] : keyToNote)
                {
                        if (isKeyToggledMap[key])
                        {
                                rawMix += std::sin(phaseMap[key]) * volumePerNote;
                                phaseMap[key] += 2.0f * 3.14159f * freq / 44100.0f;

                                if (phaseMap[key] > 2.0f * 3.14159f)
                                {
                                        phaseMap[key] -= 2.0f * 3.14159f;
                                }
                        }
                        else
                        {
                                phaseMap[key] = 0.0f;
                        }
                }

                float finalSample = std::tanh(rawMix);

                fOutput[iFrame * 2 + 0] = finalSample;
                fOutput[iFrame * 2 + 1] = finalSample;
        }
}

int main()
{
        RGFW_window* win = RGFW_createWindow("Euterpe Input", 0, 0, 250, 250, RGFW_windowCenter | RGFW_windowNoResize);

        ma_device* device = init(synthFunction);
        if (!device)
        {
                return -1;
        }

        bool isRunning = true;
        while (isRunning && !RGFW_window_shouldClose(win))
        {
                RGFW_event event;
                while (RGFW_window_checkEvent(win, &event))
                {
                        if (event.type == RGFW_keyPressed)
                        {
                                char key = std::tolower((char)event.key.value);
                                if (isKeyToggledMap.count(key))
                                {
                                        isKeyToggledMap[key] = true;
                                }
                        }

                        if (event.type == RGFW_keyReleased)
                        {
                                char key = std::tolower((char)event.key.value);
                                if (isKeyToggledMap.count(key))
                                {
                                        isKeyToggledMap[key] = false;
                                }
                        }
                }

                usleep(1000);
        }

        close(device);
        RGFW_window_close(win);

        return 0;
}
