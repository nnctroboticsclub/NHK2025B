#ifndef NHK2025B_ARM_H
#define NHK2025B_ARM_H

#include <mbed.h>
#include <algorithm>
#include "definitions.h"

class ArmParameter{
public:
    float max_effort = 5.0; // [N]
};

class NHK2025B_Arm{
public:
    NHK2025B_Arm(std::array<ArmParameter, NUM_OF_ARM> param = {{ArmParameter()}})
    {
        for(int i=0;i<param.size();i++){
            arm_data[i].parameter = param[i];
        }
    }

    void setup()
    {
        ;
    }

    void setEffort(int num,float effort)
    {
        arm_data[num].cmd.effort = effort;
    }

    float getEffort(int num)
    {
        return arm_data[num].state.effort;
    }

    void udpate()
    {
        for(int i=0;i<NUM_OF_ARM;i++){
            std::min(arm_data[i].parameter.max_effort,std::max(-arm_data[i].parameter.max_effort,arm_data[i].cmd.effort));
        }
    }
public:
    struct{
        struct{
            float effort;
        }cmd;
        struct{
            float effort;
        }state;
        ArmParameter parameter;
    }arm_data[NUM_OF_ARM];

};

#endif