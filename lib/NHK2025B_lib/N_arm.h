/**
 * @brief N_arm.h
 * @author 高野 絆(takanokiduna@gmail.com)
 * @brief NHK2025Bのアームクラス
 * @version 0.1
 * @date 2025-08-05
 * 
 * @copyright Copyright (c) 2025
 */
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

    /**
     * @brief アームが握るかどうかをセットする
     * 
     * @param dir2 アームの左右
     * @param is_holding trueなら握る, falseなら握らない
     */
    void setHolding(Direction2 dir2,bool is_holding)
    {
        arm_data[(int)dir2].cmd.is_holding = is_holding;
    }

    /**
     * @brief アームのグリップ目標位置をセットする
     * 
     * @param dir2 アームの左右
     * @param pos グリップ目標位置 [mm]
     */
    void setGripPosition(Direction2 dir2,float pos)
    {
        arm_data[(int)dir2].cmd.grip_pos = pos;
    }

    /**
     * @brief アームの角度をセットする
     * 
     * @param dir2 アームの左右
     * @param angle アームの角度 [rad]
     */
    void setArmAngle(Direction2 dir2,float angle)
    {
        arm_data[(int)dir2].cmd.arm_angle = angle;
    }

    /**
     * @brief アームのパラメータをセットする
     * 
     * @param dir2 アームの左右
     * @param param アームのパラメータ
     */
    void setParameter(Direction2 dir2,ArmParameter param)
    {
        arm_data[(int)dir2].parameter = param;
    }

    /**
     * @brief アームが握っているかどうかを取得する
     * 
     * @param dir2 アームの左右
     * 
     * @return true: 握っている, false: 握っていない
     */
    bool getHolding(Direction2 dir2)
    {
        return arm_data[(int)dir2].cmd.is_holding;
    }

    /**
     * @brief アームのグリップ目標位置を取得する
     * 
     * @param dir2 アームの左右
     * 
     * @return グリップ目標位置 [mm]
     */
    float getGripPosition(Direction2 dir2)
    {
        return arm_data[(int)dir2].cmd.grip_pos;
    }

    /**
     * @brief アームの角度を取得する
     * 
     * @param dir2 アームの左右
     * 
     * @return アームの角度 [rad]
     */
    float getArmAngle(Direction2 dir2)
    {
        return arm_data[(int)dir2].cmd.arm_angle;
    }

    /**
     * @brief アームのパラメータを取得する
     * 
     * @param dir2 アームの左右
     * 
     * @return アームのパラメータ
     */
    ArmParameter getParameter(Direction2 dir2)
    {
        return arm_data[(int)dir2].parameter;
    }

    void update()
    {
        for(int i=0;i<2;i++){
            arm_data[i].state.distance_to_hand = sin(arm_data[i].cmd.arm_angle) * arm_data[i].parameter.arm_length;
            arm_data[i].state.height_to_hand = cos(arm_data[i].cmd.arm_angle) * arm_data[i].parameter.arm_length;
            if(arm_data[i].cmd.is_holding){
                arm_data[i].state.grip_pos = arm_data[i].parameter.max_grip_position;
            }else{
                arm_data[i].state.grip_pos = 0;
            }
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