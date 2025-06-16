/**
 * @file US_sensor.h
 * @author Nazuna Sugaya
 * @brief 超音波センサを用いて距離を測る
 * @version 0.1
 * @date 2025-05-13
 */
#ifndef US_sensor_H
#define US_sensor_H

#ifndef SONIC_SPEED
#define SONIC_SPEED 340
#endif

#include <mbed.h>

class US_Sensor
{
public:
    US_Sensor(PinName trig, PinName echo);
    void setup();
    void shot();
    float getDistance(){
        return distance_*SONIC_SPEED/2;
    };
    bool falling_frag = 0; //立ち下がり確認

private:
    Timer timer;
    DigitalOut trig_pin;
    InterruptIn echo_pin;
    float distance_;

    void falling_callback();
    void rising_callback();

};

#endif
