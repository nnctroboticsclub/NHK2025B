#ifndef NHK2025B_SERVO_H
#define NHK2025B_SERVO_H

#include <mbed.h>
#include "definitions.h"
#include "ikarashiCAN_mk2.h"
#include "can_servo.h"

class ServoParameter {
public:
    int id = 1;
    int board_id = 1;
    ikarashiCAN_mk2* ican = &can1;
    float max_angle = M_PI + M_2_PI; // [rad]
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_Servo{
public:
    NHK2025B_Servo(std::array<ServoParameter, NUM_OF_SERVO> param){
        for(int i=0;i<NUM_OF_SERVO;i++){
            int use_board_id[NUM_OF_SERVO_BOARD];
            servo_data[i].parameter = param[i];
            servo[i] = can_servo(servo_data[i].parameter.ican,servo_data[i].parameter.board_id);
        }
    }
    void setup(){
        cnt = 0;
    }
    void write()
    {
        servo[cnt % NUM_OF_SERVO].send();
        cnt++;
    }
    void setAngle(int num, float angle)
    {
        servo_data[num].cmd.angle = angle;
    }
    void update(){
        for(int i=0;i<NUM_OF_SERVO;i++){
            servo[i].set(servo_data[i].parameter.id,servo_data[i].cmd.angle);
        }
    }
private:
    std::array<can_servo,NUM_OF_SERVO> servo;
    struct{
        struct{
            float angle;
        }cmd;
        struct{
            ;
        }state;
        ServoParameter parameter;
    }servo_data[NUM_OF_SERVO];
    int cnt;
};

#endif // NHK2025B_SERVO_H