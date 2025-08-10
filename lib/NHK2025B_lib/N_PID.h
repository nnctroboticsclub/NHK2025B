/**
 * @file N_PID.h
 * @author Tsugaru Kenta (googology.fan@gmail.com)
 * @brief PID controller class
 * @version 1.0.1
 * @date 2025-06-11
 */

#include "definitions.h"
#ifndef NHK2025B_PID_H
#define NHK2025B_PID_H

#ifndef NUM_OF_PID_CONTROLLER
#define NUM_OF_PID_CONTROLLER 1
#endif

#ifndef rep
#define rep(i, n) for (int i = 0; i < (n); ++i)
#endif

class PidParameter
{
public:
    PidParameter() {}

    float kp = 1.0;
    float ki = 0.0;
    float kd = 0.0;
    float offset = 0.0;
    // float ts = 1.0; // サンプリング周期 [ms]（update_ts()使用時に1msと仮定）
    float out_max = 10.0;
    bool reverse = false;
    // reverse = true にすると出力符号が反転する
};

class NHK2025B_PID
{
public:
    NHK2025B_PID() : NHK2025B_PID(PidParameter()) {}
    NHK2025B_PID(const PidParameter &param) { rep(i, NUM_OF_PID_CONTROLLER) pid_data[i].parameter = param; }
    NHK2025B_PID(const PidParameter *&params, int N) { rep(i, N) if (i < NUM_OF_PID_CONTROLLER) pid_data[i].parameter = params[i]; }
    NHK2025B_PID(std::array<PidParameter, NUM_OF_PID_CONTROLLER> params) { rep(i, NUM_OF_PID_CONTROLLER) pid_data[i].parameter = params[i]; }

    void reset(int num = 0)
    {
        pid_data[num].state.i = 0;
        pid_data[num].state.ie = 0.F;
    }

    void debug(int num, bool b);
    void print_debug(int num);

    float calc(int num, float val);
    float calc(float val) { return calc(0, val); }
    float getOutput(int num);

    void setGoalValue(int num, float val);
    void setProcessValue(int num, float val);
    void setParameter(int num, PidParameter param);

    void update_ts();

private:
    struct
    {
        struct
        {
            float goal_value;    // 目標値
            float process_value; // 現在値
            bool Isdebug = false;
        } cmd;
        struct
        {
            int i = 0;
            float ie = 0.F;
            float prev_error = 0.F;
            float output = 0.F;
        } state;
        PidParameter parameter;
    } pid_data[NUM_OF_PID_CONTROLLER];
};

#include <stdio.h>

float NHK2025B_PID::calc(int num, float val)
{
    float e = pid_data[num].cmd.goal_value - (pid_data[num].cmd.process_value = val);

    float Kp = pid_data[num].parameter.kp;
    float Ki = pid_data[num].parameter.ki;
    float Kd = pid_data[num].parameter.kd;
    float offset = pid_data[num].parameter.offset;
    bool rev = pid_data[num].parameter.reverse;
    int &i = pid_data[num].state.i;
    float &ie = pid_data[num].state.ie;
    float &prev_error = pid_data[num].state.prev_error;
    float &output = pid_data[num].state.output;

    if (!i++) prev_error = e;

    ie += e + prev_error;

    float ret = offset + Kp * e + Ki * ie + Kd * (e - prev_error);
    prev_error = e;
    output = ret * (rev ? -1.0f : 1.0f);

    return pid_data[num].state.output = output;
}

float NHK2025B_PID::getOutput(int num) { return pid_data[num].state.output; }

void NHK2025B_PID::debug(int num, bool b) { pid_data[num].cmd.Isdebug = b; }

void NHK2025B_PID::print_debug(int num)
{
    printf("PID | Kp:%f, Ki:%f, Kd:%f, e:%f -> output:%f\n",
           pid_data[num].parameter.kp,
           pid_data[num].parameter.ki,
           pid_data[num].parameter.kd,
           pid_data[num].state.prev_error,
           pid_data[num].state.output);
}

void NHK2025B_PID::update_ts()
{
    for (int i = 0; i < NUM_OF_PID_CONTROLLER; i++)
        pid_data[i].state.output = calc(i, pid_data[i].cmd.process_value);
}

void NHK2025B_PID::setGoalValue(int num, float val) { pid_data[num].cmd.goal_value = val; }

void NHK2025B_PID::setProcessValue(int num, float val) { pid_data[num].cmd.process_value = val; }

void NHK2025B_PID::setParameter(int num, PidParameter param) { pid_data[num].parameter = param; }

#endif // NHK2025B_PID_H
