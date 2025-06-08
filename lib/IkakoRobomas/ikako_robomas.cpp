#include "ikako_robomas.h"

void IkakoMotor::set_params(int _id)
{
    config.id = _id;
}
void IkakoMotor::set_ref(float current)
{
    current = min(max(current, -config.current_limit), config.current_limit);
    config.mc.current = current * config.convert_current_ref_ratio;
    swap_two_elements(config.mc.data_array);
}

float IkakoMotor::get_current()
{
    return (float)config.df_fb.current * config.convert_current_to_float;
}

float IkakoMotor::get_angle()
{
    return (float)config.df_fb.angle * config.convert_position_to_float;
}

float IkakoMotor::get_vel(float gear_ratio)
{
    gear_ratio = (gear_ratio > 0) ? gear_ratio : params.gear_ratio;
    return (float)config.df_fb.rpm * config.convert_velocity_to_float / gear_ratio;
}

float IkakoMotor::get_torque()
{
    return this->get_current() * params.Kt;
}

float IkakoMotor::get_temperature()
{
    return config.df_fb.temperature;
}

int IkakoMotor::get_read_flag()
{
    return config.read_flag;
}

void IkakoMotor::swap_two_elements(uint8_t *array)
{
    uint8_t buff = array[1];
    array[1] = array[0];
    array[0] = buff;
}

ikako_robomas_motor_config *IkakoMotor::get_motor()
{
    return &config;
}

MotorParams *IkakoMotor::get_motor_params()
{
    return &params;
}

IkakoRobomasSender::IkakoRobomasSender(ikarashiCAN_mk2 *_ican)
{
    ican = _ican;
    cnt = 0;
}

int IkakoRobomasSender::write()
{
    cnt++;
    for (int i = 0; i < (int)motor.size(); i++)
    {
        if (motor[i]->id <= 4)
        {
            df_0x200.current[motor[i]->id - 1] = motor[i]->mc.current;
        }
        else if (motor[i]->id <= 8)
        {
            df_0x1FF.current[motor[i]->id - 4 - 1] = motor[i]->mc.current;
        }
    }
    if (cnt % 2)
    {
        // for(int i=0; i<8; i+=2)
        //     this->swap_two_elements(&df_0x200.data_array[i]);
        ican->set_this_id(0x200);
        ican->set(df_0x200.data_array, 8);
    }
    else
    {
        // for(int i=0; i<8; i+=2)
        //     this->swap_two_elements(&df_0x1FF.data_array[i]);
        ican->set_this_id(0x1FF);
        ican->set(df_0x1FF.data_array, 8);
    }
    return ican->write(0);
}

void IkakoRobomasSender::read()
{
    for (int i = 0; i < (int)motor.size(); i++)
    {
        if ((ican->get_sender_id() == (0x200 | motor[i]->id)) && ican->get_read_flag())
        {
            ican->get(motor[i]->df_fb.data_array, 8);
            for (int j = 0; j < 8; j += 2)
                this->swap_two_elements(&motor[i]->df_fb.data_array[j]);
            motor[i]->read_flag = 1;
        }
        else
            motor[i]->read_flag = 0;
    }
}

void IkakoRobomasSender::set_motors(ikako_robomas_motor_config *_motor)
{
    motor.push_back(_motor);
}

void IkakoRobomasSender::print_debug()
{
    printf("size=%d", (int)motor.size());
    printf(",read=");
    for (int i = 0; i < (int)motor.size(); i++)
        printf("%d ", motor[i]->read_flag);
    printf(",0x200=");
    for (int i = 0; i < 8; i++)
        printf("%d ", df_0x200.data_array[i]);
    printf(",0x1ff=");
    for (int i = 0; i < 8; i++)
        printf("%d ", df_0x1FF.data_array[i]);
    printf(",mc.cur=");
    for (int i = 0; i < (int)motor.size(); i++)
        printf("%d ", motor[i]->mc.current);
    printf("|");
}

void IkakoRobomasSender::swap_two_elements(uint8_t *array)
{
    uint8_t buff = array[1];
    array[1] = array[0];
    array[0] = buff;
}