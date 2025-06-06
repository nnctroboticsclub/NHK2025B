#ifndef NHK2025B_PID_H
#define NHK2025B_PID_H

#include <mbed.h>

struct PidParameter{
    float kp = 1.0;
    float ki = 0.0;
    float kd = 0.0;
    int ts = 1; // 制御周期[ms] // update_ts()関数で処理するため、1ms間隔
    float out_max = 10.0;
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_PID{
    ;
};

#endif // NHK2025B_PID_H