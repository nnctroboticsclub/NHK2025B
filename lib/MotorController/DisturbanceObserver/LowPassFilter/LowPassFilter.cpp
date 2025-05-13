#include "LowPassFilter.h"

LPF::LPF(float _Ts, float _omega)
{
    set_params(_Ts, _omega);
}

LPF::LPF()
{
}

void LPF::set_params(float _Ts, float _omega)
{
    Ts = _Ts;
    omega = _omega;
    K[0] = (omega / (2 * M_PI) * Ts) / (2 + omega / (2 * M_PI) * Ts);
    K[1] = (2 - omega / (2 * M_PI) * Ts) / (2 + omega / (2 * M_PI) * Ts);
}

void LPF::update()
{
    if (!enabled)
        return;
    // output = K[0] * (response[1] + response[0]) + K[1] * response[0];
    output[1] = (1 / (1 + Ts * omega)) * (Ts * omega * response[1] + output[0]);
    output[0] = output[1];
}

float LPF::get_output()
{
    return output[1];
}

void LPF::set_response(float res)
{
    response[1] = res;
}

void LPF::enable()
{
    enabled = true;
}

void LPF::disable()
{
    enabled = false;
}

bool LPF::is_enabled()
{
    return enabled;
}