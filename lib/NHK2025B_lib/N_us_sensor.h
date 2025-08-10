/**
 * @file N_us_sensor.h
 * @author Nazuna Sugaya(naduna727su5@gmail.com)
 * @brief NHK2025Bの超音波センサのクラス
 * @version 0.1
 * @date 2025-06-18
 * 
 * @copyright Copyright (c) 2025
 */
#ifndef NHK2025B_US_SENSOR_H
#define NHK2025B_US_SENSOR_H

#include <mbed.h>

class US_sensor{
public:
    /**
     * @brief セットアップ関数
     */
    void setup()
    {
        ;
    }

    /**
     * @brief 超音波の発射
     */
    void shot(){
        US_sensor_date.cmd.trig_pin.write(1);
        wait_us(10);   
        US_sensor_date.cmd.trig_pin.write(0);
    }

    /**
     * @brief echoが立ち上がったときにタイマースタートさせます．
     */
    void rising_callback(){
        US_sensor_date.state.timer.stop();
        US_sensor_date.state.distance_ = US_sensor_date.state.timer.read();
        US_sensor_date.falling_flag = 0;
    }

    /**
     * @brief echoが立ち下がったときにタイマーを止める．
     */
    void falling_callback(){
        US_sensor_date.state.timer.reset();
        US_sensor_date.state.timer.start();
        US_sensor_date.falling_flag = 1;
    }

    /**
     * @brief セットアップ関数
     */
    void setup(){
        US_sensor_date.cmd.echo_pin.fall(callback(this, &falling_callback));
        US_sensor_date.cmd.echo_pin.rise(callback(this, &rising_callback));
        shot();
        US_sensor_date.state.getDistance();;
    }


private:
    struct{
        struct{
            DigitalOut trig_pin;
            InterruptIn echo_pin;
        }cmd;
        struct{
            Timer timer;
            float distance_;
            float getDistance(){
                return distance_*340/2;
            }
        }state;
        bool falling_flag = 0;  //立ち下がり確認
    }US_sensor_date;
};

#endif  //NHK2025B_US_SENSOR_H