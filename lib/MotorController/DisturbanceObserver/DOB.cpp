#include "DOB.h"

DOB::DOB(float _Ts, float _omega, float _Kin, float _Kout)
{
    set_params(_Ts, _omega, _Kin, _Kout);
}

DOB::DOB(){}

void DOB::set_params(float _Ts, float _omega, float _Kin, float _Kout)
{
    Ts = _Ts;
    omega = _omega;
    Kin = _Kin;
    Kout = _Kout;
    lpf.set_params(Ts, omega);
}

void DOB::update()
{
    if(!enabled)
        return;
    lpf.set_response(Kin * reference + Kout * response);
    lpf.update();
    output = (lpf.get_output() - Kout * response) / Kin;
    // output = lpf.get_output() / Kin;
}

void DOB::set_response(float res)
{
    response = res;
}

void DOB::set_reference(float ref)
{
    reference = ref;
}

void DOB::enable()
{
    enabled = true;
    lpf.enable();
}

void DOB::disable()
{
    enabled = false;
    lpf.disable();
}

bool DOB::is_enabled()
{
    return enabled;
}

float DOB::get_output()
{
    return output;
}