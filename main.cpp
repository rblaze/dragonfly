#include "mbed.h"
#include "platform/mbed_thread.h"
#include "Servo.h"


PinName LEFT_EYE = A0;
PinName RIGHT_EYE = A2;
PinName SERVO = D6;

int main()
{
    AnalogIn leftEye(LEFT_EYE);
    AnalogIn rightEye(RIGHT_EYE);
    Servo servo(SERVO);

    float position = 0.5;
    const float step = 0.01;
    const float threshold = 0.015;

    const int printStep = 100;
    int count = 0;
    const size_t logsize = 150;
    char logbuf[logsize];

    while (true) {
        float left = leftEye.read();
        float right = rightEye.read();

        if (left - right > threshold) {
            position -= step;
            if (position < 0) {
                position = 0;
            }
        } else if (right - left > threshold) {
            position += step;
            if (position > 1) {
                position = 1;
            }
        }

        if (count++ == printStep) {
            count = 0;
            auto offset = snprintf(logbuf, logsize, "L: %f R%f\n", left, right);
            snprintf(logbuf + offset, logsize - offset, "\npos %f\n", position);
            printf("%s\n", logbuf);
        }
        servo.write(position);

        thread_sleep_for(25);
    }
}
