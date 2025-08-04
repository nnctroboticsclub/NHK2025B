#ifndef NHK2025B_ARM_H
#define NHK2025B_ARM_H

#include <mbed.h>
#include <algorithm>
#include "definitions.h"

class ArmParameter{
public:
    float arm_length = 500.0; // [mm]
    float max_grip_effort = 5.0; // [N]
    float max_grip_position = 300.0; // [mm]
    float max_bend_effort = 10.0; // [N]
    float max_arm_angle = M_PI_2; // [rad]
    float init_angle = 0; // [rad]
};

class NHK2025B_Arm{
public:
    NHK2025B_Arm(std::array<ArmParameter, 2> param={{ArmParameter(),ArmParameter()}})
    {
        arm_data[(int)Direction2::RIGHT].parameter = param[(int)Direction2::RIGHT];
        arm_data[(int)Direction2::LEFT].parameter = param[(int)Direction2::LEFT];
    }

    void setup()
    {
        ;
    }

    void setHolding(Direction2 dir2,bool is_holding)
    {
        arm_data[(int)dir2].cmd.is_holding = is_holding;
    }

    void setGripPosition(Direction2 dir2,float pos)
    {
        arm_data[(int)dir2].cmd.grip_pos = pos;
    }

    void setArmAngle(Direction2 dir2,float angle)
    {
        arm_data[(int)dir2].cmd.arm_angle = angle;
    }

    void setParameter(Direction2 dir2,ArmParameter param)
    {
        arm_data[(int)dir2].parameter = param;
    }

    bool getHolding(Direction2 dir2)
    {
        return arm_data[(int)dir2].cmd.is_holding;
    }

    float getGripPosition(Direction2 dir2)
    {
        return arm_data[(int)dir2].cmd.grip_pos;
    }

    float getArmAngle(Direction2 dir2)
    {
        return arm_data[(int)dir2].cmd.arm_angle;
    }

    ArmParameter getParameter(Direction2 dir2)
    {
        return arm_data[(int)dir2].parameter;
    }

    void udpate()
    {
        for(int i=0;i<2;i++){
            arm_data[i].state.distance_to_hand = sin(arm_data[i].cmd.arm_angle) * arm_data[i].parameter.arm_length;
            arm_data[i].state.height_to_hand = cos(arm_data[i].cmd.arm_angle) * arm_data[i].parameter.arm_length;
        }
    }
public:
    struct{
        struct{
            bool is_holding;
            float arm_angle;
            float grip_pos;
        }cmd;
        struct{
            float grip_pos;
            float distance_to_hand;
            float height_to_hand;
        }state;
        ArmParameter parameter;
    }arm_data[2];

};

#endif