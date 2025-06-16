#include "ikakoMDC.h"

ikakoMDC::ikakoMDC(int _motor_num, int _min_speed, int _max_speed, float cycle_s, float time_s, float Kc, float tauI, float tauD, float interval)
    : PID(Kc, tauI, tauD, interval), lpf(cycle_s, time_s)
{
    motor_num = (_motor_num <= 0) ? 0 : _motor_num - 1;
    min_speed = _min_speed;
    max_speed = _max_speed;
    min_out_speed = _min_speed;
    max_out_speed = _max_speed;
    pid_min_speed = 0;
    pid_max_speed = 0;
    pid_speed = 0;
    speed = 0;
    result_speed = 0;
    enc = 0;
    stop_flag = false;
    PID::setBias(0);
    PID::setMode(1);
}

void ikakoMDC::set_speed(int8_t _speed, bool mode)
{
    speed = _speed;
    if (speed > 0)
    {
        speed = (float)speed / 100 * abs(max_out_speed);
    }
    else if (speed < 0)
    {
        speed = (float)speed / 100 * abs(min_out_speed);
    }
}

int ikakoMDC::get_enc()
{
    return enc;
}

int ikakoMDC::get_speed()
{
    result_speed = speed + pid_speed;
    result_speed = (result_speed > max_speed) ? max_speed : result_speed;
    result_speed = (result_speed < min_speed) ? min_speed : result_speed;
    return stop_flag ? 0 : lpf::path_value((float)result_speed);
}

int ikakoMDC::get_motor_num()
{
    return motor_num;
}

void ikakoMDC::pid_update(bool mode)
{
    pid_speed = mode ? PID::compute() : -PID::compute();
    // if (pid_speed > 0)
    // {
    //     pid_speed = (float)pid_speed / 100 * abs(pid_max_speed);
    // }
    // else if (pid_speed < 0)
    // {
    //     pid_speed = (float)pid_speed / 100 * abs(pid_min_speed);
    // }
}

void ikakoMDC::set_limits(float inMin, float inMax, float outMin, float outMax)
{
    PID::setInputLimits(inMin, inMax);
    PID::setOutputLimits(outMin, outMax);
    pid_min_speed = outMin;
    pid_max_speed = outMax;
    min_out_speed = min_speed - outMin;
    max_out_speed = max_speed - outMax;
}

void ikakoMDC::set_tunings(float Kc, float tauI, float tauD)
{
    PID::setTunings(Kc, tauI, tauD);
}

void ikakoMDC::set_set_point(float sp)
{
    PID::setSetPoint(sp);
}

void ikakoMDC::set_process_value(float pv)
{
    PID::setProcessValue(pv);
}

void ikakoMDC::stop()
{
    stop_flag = true;
}

void ikakoMDC::start()
{
    stop_flag = false;
}

////////////////////////////////////////////////////////////////////////
ikakoMDC_sender::ikakoMDC_sender(ikakoMDC *_imdc, int _imdc_size, ikarashiCAN_mk2 *_can, int _mdc_id)
{
    imdc = _imdc;
    mdc_size = _imdc_size;
    mdc_id = _mdc_id;
    can = _can;
    cmsDF.data = 0;
    ceDF.data = 0;
}

int ikakoMDC_sender::send()
{
    int speed_buffer = 0;
    for (int i = 0; i < mdc_size; i++)
    {
        speed_buffer = imdc[i].get_speed();
        cmsDF.motor[i] = speed_buffer;
    }
    can->set(cmsDF.data_array, sizeof(cmsDF.data_array));
    return can->write(mdc_id);
}

bool ikakoMDC_sender::read_enc()
{
    bool return_val = false;
    if (can->get_id() == mdc_id)
    {
        can->get(ceDF.data_array, sizeof(ceDF.data_array));
        for (int i = 0; i < mdc_size; i++)
            imdc[i].enc = ceDF.encoder[i];
        return_val = true;
    }

    return return_val;
}

int ikakoMDC_sender::get_mdc_id()
{
    return mdc_id;
}

void ikakoMDC_sender::send_start()
{
    mdc_thread = new Thread();
    mdc_thread->start(Callback<void()>(this, &ikakoMDC_sender::thread_task));
}

void ikakoMDC_sender::thread_task()
{
    while(1)
    {
        this->send();
        ThisThread::sleep_for(1ms);
    }
}