/**
 * @file N_PID.h
 *
 * @author Tsugaru Kenta (googology.fan@gmail.com)
 * @brief PID controller class
 * @version 1.0.1
 * @date 2025-06-11
 */

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
    // int ts = 1; // 制御周期[ms] // update_ts()関数で処理するため、1ms間隔
    float out_max = 10.0;
    bool reverse = false;
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_PID
{
public:
    NHK2025B_PID() : NHK2025B_PID(PidParameter()) {}
    NHK2025B_PID(const PidParameter &param) { rep(i, NUM_OF_PID_CONTROLLER) pid_data[i].parameter = param; }
    NHK2025B_PID(const vector<PidParameter> &params) { rep(i, params.size()) if (i < NUM_OF_PID_CONTROLLER) pid_data[i].parameter = params[i]; }
    NHK2025B_PID(const PidParameter *&params, int N) { rep(i, N) if (i < NUM_OF_PID_CONTROLLER) pid_data[i].parameter = params[i]; }

    void reset(int j = 0)
    {
        pid_data[j].state.i = 0;
        pid_data[j].state.ie = 0.F;
    }

    void debug(int j, bool b);
    void print_debug(int j);

    float calc(float e, int j);
    float get(int j);

private:
    struct
    {
        struct
        {
            float goal_value;    // 目標値
            float process_value; // センサーから読み取った値
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

float NHK2025B_PID::calc(float e, int j = 0)
{
    float Kp = pid_data[j].parameter.kp;
    float Ki = pid_data[j].parameter.ki;
    float Kd = pid_data[j].parameter.kd;
    float offset = pid_data[j].parameter.offset;
    bool rev = pid_data[j].parameter.reverse;
    int &i = pid_data[j].state.i;
    float &ie = pid_data[j].state.ie;
    float &prev_error = pid_data[j].state.prev_error;
    float &output = pid_data[j].state.output;
    if (!i++)
        prev_error = e;
    ie += e + prev_error;
    float ret = offset + Kp * e + Ki * ie / i + Kd * (e - prev_error);
    prev_error = e;
    output = ret * (rev * -2 + 1);

    return output;
}
float NHK2025B_PID::get(int j = 0) { return pid_data[j].state.output; }

void NHK2025B_PID::debug(int j = 0, bool b = true) { pid_data[j].cmd.Isdebug = b; }

void NHK2025B_PID::print_debug(int j = 0)
{
    printf("PID | Kp:%f, Ki:%f, Kd:%f, e:%f -> output:%f\n",
           pid_data[j].parameter.kp,
           pid_data[j].parameter.ki,
           pid_data[j].parameter.kd,
           pid_data[j].state.prev_error,
           pid_data[j].state.output);
}

#endif // NHK2025B_PID_H