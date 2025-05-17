#ifndef NHK2025B_SERVO_H
#define NHK2025B_SERVO_H

#include <mbed.h>

class NHK2025B_Servo{
    void writeCan1();
    void writeCan2();
};

#endif // NHK2025B_SERVO_H