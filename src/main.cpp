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

std::atomic<float> frequency{0.0f};

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

void synthFunction(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
        static float phase = 0.0f;
        static float volume = 0.5f;

        float* fOutput = (float*)pOutput;
        float currentFreq = frequency.load();

        for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++)
        {
                float sample = (currentFreq > 0) ? sinf(phase) * volume : 0.0f;

                fOutput[iFrame * 2 + 0] = sample;
                fOutput[iFrame * 2 + 1] = sample;

                phase += 2.0f * 3.14159f * currentFreq / 44100.0f;

                if (phase > 2.0f * 3.14159f) phase -= 2.0f * 3.14159f;
        }
}

void processInput(std::atomic<float>& freq, const std::unordered_map<char, float>& keyMap)
{
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0)
        {
                if (keyMap.count(ch))
                {
                        freq = keyMap.at(ch);
                }
                if (ch == 'x')
                {
                        freq = -1.0f;
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
                processInput(frequency, keyToNote);
                if (frequency < 0) isRunning = false;
                usleep(1000);
        }

        close(device);

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        return 0;
}
