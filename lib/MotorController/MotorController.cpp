#include "MotorController.h"

MotorController::MotorController(ControlType _type, MotorParams *_params, float _Ts, float omega_v) : pid(Ts)
{
    type = _type;
    params = _params;
    Ts = _Ts;

    // float Kpv = (omega_v / (2.0 * M_PI) * params->J) / (params->Kt * params->p_pairs);
    // float Kiv = 0.0;//(omega_v / (2.0 * M_PI) * params->D) / (params->Kt * params->p_pairs);
    pid.set_first_value();
    // pid.param.Kp = Kpv;
    // pid.param.Ki = Kiv;

    // float omega_v_dob = 4.0 * M_PI;
    dob.set_params(Ts, omega_v, params->Kt * params->p_pairs, omega_v / (2 * M_PI) * params->J);
}

void MotorController::start()
{
    enable();
}

void MotorController::update()
{
    if (!enabled)
        return;
    dob.set_response(velocity.res);
    pid.set_response(velocity.res);
    switch (type)
    {
    case ControlType::POSITION:
        break;
    case ControlType::VELOCITY:
        pid.set_reference(velocity.ref);
        pid.update();
        dob.update();
        current.ref = dob.get_output() + pid.get_output();
    case ControlType::CURRENT:
        current.ref = std::max(std::min(current.ref, max), min);
        dob.set_reference(current.ref);
        break;
    default:
        break;
    }
}

float MotorController::get_output()
{
    return current.ref;
}

MotorController::MotorState *MotorController::get_state()
{
    MotorState *return_value;
    switch (type)
    {
    case ControlType::CURRENT:
        return_value = &current;
        break;
    case ControlType::VELOCITY:
        return_value = &velocity;
        break;
    case ControlType::POSITION:
        return_value = &position;
        break;
    default:
        // return_value = &velocity;
        break;
    }

    return return_value;
}

void MotorController::set_reference(float value)
{
    MotorState *state = get_state();
    state->ref = value;
}

void MotorController::set_response(float value)
{
    MotorState *state = get_state();
    state->res = value;
}

void MotorController::set_limit(float _min, float _max)
{
    min = _min;
    max = _max;
    pid.set_output_limits(min, max);
}

void MotorController::set_pid_gain(float kp, float ki, float kd)
{
    pid.param.Kp = kp;
    pid.param.Ki = ki;
    pid.param.Kd = kd;
}

void MotorController::enable()
{
    enabled = true;
    dob.enable();
    pid.enable();
}

void MotorController::disable()
{
    enabled = false;
    dob.disable();
    pid.disable();
}

bool MotorController::is_enabled()
{
    return enabled;
}