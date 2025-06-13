#ifndef NHK2025B_N_ROHM_MD_H
#define NHK2025B_N_ROHM_MD_H

#include <mbed.h>
#include "rohm_md.h"
#include "definitions.h"
#include "ikarashiCAN_mk2.h"

class RohmMdParameter{
public:
    int id = 1;
    ikarashiCAN_mk2* ican = &can1;
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_RohmMD{
public:
    NHK2025B_RohmMD(std::array<RohmMdParameter,NUM_OF_ROHM_MD> param = {{RohmMdParameter()}})
    {
        for(int i=0;i<param.size();i++){
            rohmMD_data[i].parameter = param[i];
            md[i] = RohmMD(rohmMD_data[i].parameter.ican,rohmMD_data[i].parameter.id);
        }
    }

    void setup()
    {
        cnt = 0;
        for(int i=0;i<NUM_OF_ROHM_MD;i++){
            md[i].print_debug();
        }
    }

    void write()
    {
        md[cnt%NUM_OF_ROHM_MD].send();
        cnt++;
    }

    RohmMdParameter getRohmMdParameter(int num)
    {
        return rohmMD_data[num].parameter;
    }

    void setPower(int num,float pow)
    {
        rohmMD_data[num].cmd.power = pow;
    }
    
    float getAngle(int num) {
        return rohmMD_data[num].state.angle;
    }
    
    float getVelocity(int num) {
        return rohmMD_data[num].state.velocity;
    }
    
    float getCurrent(int num) {
        return rohmMD_data[num].state.current;
    }

    void update()
    {
        for(int i=0;i<NUM_OF_ROHM_MD;i++)
        {
            md[i].set(0,rohmMD_data[i].cmd.power);
            md[i].read();
            rohmMD_data[i].state.angle = md[i].get_rad();
            rohmMD_data[i].state.velocity = md[i].get_vel();
            rohmMD_data[i].state.current = md[i].get_cur();
        }
    }

    void print_debug()
    {
        ;
    }

private:
    std::array<RohmMD, NUM_OF_ROHM_MD> md;
    struct{
        struct{
            float power;
        }cmd;
        struct{
            float angle;
            float velocity;
            float current;
        }state;
        RohmMdParameter parameter;
    }rohmMD_data[NUM_OF_ROHM_MD];
    int cnt = 0;
};

#endif // NHK2025B_N_ROHM_MD_H