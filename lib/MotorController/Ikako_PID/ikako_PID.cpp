#include "ikako_PID.h"

Ikako_PID::Ikako_PID(float _Ts)
{
    Ts = _Ts;
}

Ikako_PID::Ikako_PID(){}

void Ikako_PID::set_first_value(float _e, float _ie)
{
    ie = _ie;
    e_pre = _e;
}

void Ikako_PID::set_output_limits(float _min, float _max)
{
    min = _min;
    max = _max;
}

void Ikako_PID::set_reference(float ref)
{
    state.ref = ref;
    state.error = state.ref - state.res;
}

void Ikako_PID::set_response(float res)
{
    state.res = res;
    state.error = state.ref - state.res;
}

void Ikako_PID::update()
{
    if (!enabled)
        return;
    
    de = (state.error - e_pre) / Ts;
    ie += (state.error + e_pre) * Ts / 2;
    state.out = param.Kp * state.error + param.Ki * ie + param.Kd * de;
    state.out = std::max(std::min(state.out, max), min);
    e_pre = state.error;
}

float Ikako_PID::get_output()
{
    return state.out;
}

void Ikako_PID::enable()
{
    enabled = true;
}

void Ikako_PID::disable()
{
    enabled = false;
}

bool Ikako_PID::is_enabled()
{
    return enabled;
}