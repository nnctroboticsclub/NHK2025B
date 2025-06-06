#ifndef NHK2025B_SERVO_H
#define NHK2025B_SERVO_H

#include <mbed.h>
#include "definitions.h"
#include "ikarashiCAN_mk2.h"
#include "can_servo.h"

struct ServoParameter {
    int id = 1;
    int board_id = 1;
    ikarashiCAN_mk2* ican = &can1;
    float max_angle = M_PI + M_2_PI; // [rad]
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_Servo{
public:
    NHK2025B_Servo(ServoParameter params[]){
        for(int i=0;i<NUM_OF_SERVO;i++){
            servo[i] = can_servo(params[i].ican,params[i].board_id);
        }
    }
    void setup(){
    }
    void writeCan1();
    void writeCan2();
    void update(){
        servo[0].set(1,100);
    }
private:
    can_servo servo[NUM_OF_SERVO];
    typedef struct{
        ikarashiCAN_mk2 *ican;
        int board_id;
        int id;
    }CanAndBoardIdAndId;
};

#endif // NHK2025B_SERVO_H