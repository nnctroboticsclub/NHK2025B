#ifndef NHK2025B_STEER_H
#define NHK2025B_STEER_H

#include <mbed.h>

struct SteerParameter {
    int front_id = 1;
    int back_id = 2;
    float max_vel = 10; // [m/s]
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_Steer{
    ;
};

#endif // NHK2025B_STEER_H