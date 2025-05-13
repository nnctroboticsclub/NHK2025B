#include "puropo.h"

Puropo::Puropo(PinName tx, PinName rx, int _timeout) : serial(tx, rx, 100000)
{
    serial.format(8, SerialBase::None, 2);
    timer.reset();
    timer.start();
    timeout = _timeout;
}

void Puropo::start()
{
    serial.attach(Callback<void()>(this, &Puropo::read_attach), UnbufferedSerial::RxIrq);
}

bool Puropo::is_ok()
{
    return enable;
}

int Puropo::get_ch(int ch)
{
    int return_value = 0;
    switch (ch)
    {
    case 1:
        return_value = channels.channel1;
        break;
    case 2:
        return_value = channels.channel2;
        break;
    case 3:
        return_value = channels.channel3;
        break;
    case 4:
        return_value = channels.channel4;
        break;
    case 5:
        return_value = channels.channel5;
        break;
    case 6:
        return_value = channels.channel6;
        break;
    case 7:
        return_value = channels.channel7;
        break;
    case 8:
        return_value = channels.channel8;
        break;
    case 9:
        return_value = channels.channel9;
        break;
    case 10:
        return_value = channels.channel10;
        break;
    case 11:
        return_value = channels.channel11;
        break;
    case 12:
        return_value = channels.channel12;
        break;
    case 13:
        return_value = channels.channel13;
        break;
    case 14:
        return_value = channels.channel14;
        break;
    case 15:
        return_value = channels.channel15;
        break;
    case 16:
        return_value = channels.channel16;
        break;
    case 17:
        return_value = sub_data.channel17;
        break;
    case 18:
        return_value = sub_data.channel18;
        break;
    default:
        break;
    }
    return return_value;
}

int Puropo::get_threshold(int ch, int threshold)
{
    int value = this->get_ch(ch);
    if (abs(value) >= 1000 - threshold && abs(value) <= 1000 + threshold)
        value = 1000;
    else if (abs(value) <= 400)
        value = 400 * value / abs(value);
    return value;
}

float Puropo::get_normal(int ch, int threshold)
{
    float value = -(this->get_threshold(ch, threshold) - 1000 * this->get_threshold(ch, threshold) / abs(this->get_threshold(ch, threshold)));
    value /= 600;
    if (value > 1)
        value = 1.0;
    else if (value < -1)
        value = -1.0;
    return value;
}

float Puropo::get(int ch)
{
    if(timer.read_ms() - time_buff <= timeout)
        return data[ch-1];
    else 
        return 0.0;
}

void Puropo::read_attach()
{
    enable = false;
    serial.read(&protocol.data_array[length], 1);
    if (protocol.start == 0x0f)
        length++;
    else
        length = 0;
    if (length >= (int)sizeof(protocol.data_array) - 1)
    {
        memcpy(channels.data_array, protocol.main_data, sizeof(channels.data_array));
        sub_data.sub_data = protocol.sub_data;
        if (!sub_data.frame_lost)
        {
            for (int i = 0; i < 16; i++)
                data[i] = this->get_normal(i + 1);
            enable = true;
            time_buff = timer.read_ms();
        }
        length = 0;
    }
}