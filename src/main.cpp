#include <iostream>
#include <cmath>
#include <array>
#include <unordered_map>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

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

void processInput(const std::unordered_map<char, float>& keyMap, std::unordered_map<char, bool>& isKeyToggledMap)
{
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0)
        {
                if (keyMap.count(ch))
                {
                        if (!isKeyToggledMap.at(ch))
                        {
                                isKeyToggledMap[ch] = true;
                        }
                        else
                        {
                                isKeyToggledMap[ch] = false;
                        }
                }
                if (ch == 'x')
                {
                        for (auto& [key, toggled] : isKeyToggledMap)
                        {
                                toggled = false;
                        }
                }
        }
}

int main()
{
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ma_device* device = init(synthFunction);

        bool isRunning = true;
        while (isRunning)
        {
                processInput(keyToNote, isKeyToggledMap);
                usleep(1000);
        }

        close(device);

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        return 0;
}
