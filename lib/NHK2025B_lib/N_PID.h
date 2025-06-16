#ifndef NHK2025B_PID_H
#define NHK2025B_PID_H

#include <mbed.h>
#include <definitions.h>

class PidParameter{
public:
    float kp = 1.0;
    float ki = 0.0;
    float kd = 0.0;
    int ts = 1; // 制御周期[ms] // update_ts()関数で処理するため、1ms間隔
    float out_max = 10.0;
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_PID{
public:
    NHK2025B_PID(std::array<PidParameter,NUM_OF_PID_CONTROLLER> param = {{PidParameter()}})
    {
        for(int i=0;i<param.size();i++){
            pid_data[i].parameter = param[i];
        }
    }

    void setup()
    {
        for(int i=0;i<NUM_OF_PID_CONTROLLER;i++)
        {
            // pid_data[i].state.i_error = 0;
        }
    }

    void setParameter(int num, PidParameter param)
    {
        pid_data[num].parameter = param;
    }

    PidParameter getParameter(int num){
        return pid_data[num].parameter;
    }

    void update()
    {
        ;
    }

    void update_ts()
    {
    }

private:
    struct{
        struct{
            float goal_value; // 目標値
            float process_value; // センサーから読み取った値
        }cmd;
        struct{
            // float error; // 誤差 = goal_value - process_value
            // float i_error; // 累積誤差
            // float pre_error; // 前回の誤差
            float output;
        }state;
        PidParameter parameter;
    }pid_data[NUM_OF_PID_CONTROLLER];
};

#endif // NHK2025B_PID_H