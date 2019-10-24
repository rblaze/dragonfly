#include <vector>
#include "Servo.h"
#include "mbed.h"
#include "platform/mbed_thread.h"

PinName LEFT_EYE{A2};
PinName RIGHT_EYE{A0};
PinName SERVO{D6};

std::array<PinName, 9> LEDS{PG_1, PF_9, PF_7, PF_8, PE_3,
                            PE_6, PE_5, PE_4, PE_2};

int main() {
    AnalogIn leftEye(LEFT_EYE);
    AnalogIn rightEye(RIGHT_EYE);
    Servo servo(SERVO);
    std::vector<DigitalOut> leds;

    for (auto led : LEDS) {
        leds.emplace_back(led, 0);
    }

    float position = 0.5;
    const float step = 0.01;

    const uint16_t threshold = 1000;

    const int printStep = 100;
    int count = 0;
    const size_t logsize = 150;
    char logbuf[logsize];

    while (true) {
        uint16_t left = leftEye.read_u16();
        uint16_t right = rightEye.read_u16();
        uint16_t diff = std::max(left, right) - std::min(left, right);

        size_t activeLed = LEDS.size() / 2;

        if (diff >= threshold) {
            if (left > right) {
                position -= step;
                if (position < 0) {
                    position = 0;
                }

                activeLed = 3 - (diff >> 14);
            } else {
                position += step;
                if (position > 1) {
                    position = 1;
                }

                activeLed = 5 + (diff >> 14);
            }
        }

        for (size_t i = 0; i < leds.size(); i++) {
            leds[i].write(i == activeLed ? 1 : 0);
        }

        if (count++ == printStep) {
            count = 0;
            auto offset =
                snprintf(logbuf, logsize, "L: %hu R: %hu diff: %hu led: %d\n",
                         left, right, diff, activeLed);
            offset += snprintf(logbuf + offset, logsize - offset, "pos %f\n",
                               position);
            printf("%s\n", logbuf);
        }
        servo.write(position);

        thread_sleep_for(25);
    }
}
