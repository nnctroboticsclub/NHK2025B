#include "can_servo.h"

can_servo::can_servo(ikarashiCAN_mk2 *_ican, int _id)
{
    ican = _ican;
    for (int i = 0; i < 8; i++)
    {
        csdf.data_array[i] = 0;
    }
    cnt = 0;
    id = _id;
}

void can_servo::set(int num, uint8_t param)
{
    csdf.servo[num] = param;
}

int can_servo::send()
{
    ican->set(csdf.data_array, sizeof(csdf.data_array));
    ican->set_this_id(0);
    return ican->write(id);
}

uint8_t can_servo::get(int num)
{
    return csdf.servo[num];
}