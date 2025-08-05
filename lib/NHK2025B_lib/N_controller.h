/**
 * @file N_controller.h
 * @author 高野 絆(takanokiduna@gmail.com)
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 */
#ifndef NHK2025B_CONTROLLER_H
#define NHK2025B_CONTROLLER_H

#include <mbed.h>
#include "N_puropo.h"
#include "definitions.h"

class ControllerParameter {
public:
    struct{
        float max_vel = STEER_MAX_VELOCITY; // 最大速度[m/s]
        float max_angle = STEER_MAX_RAD; // 最大可動角度[rad]
        float cw_border = 0.5; // 旋回するかを判定する境界値
    }steer;
    struct{
        float max_effort = ARM_MAX_EFFORT; // 最大握力[N]
    }arm;
};

class NHK2025B_Controller{
public:
    /**
     * @brief コンストラクタ
     */
    NHK2025B_Controller(ControllerParameter param=ControllerParameter()){
        controller_data.parameter = param;
    }

    /**
     * @brief セットアップ
     */
    void setup()
    {
        ;
    }

    /**
     * @brief コントローラのパラメータをセットする
     */
    void setParameter(ControllerParameter param)
    {
        controller_data.parameter = param;
    }

    /**
     * @brief ステアの速度をセットする
     * 
     * @param val -1.0 ~ 1.0
     */
    void setSteerVelocity(float val)
    {
        controller_data.cmd.steer.vel = val * controller_data.parameter.steer.max_vel;
    }

    /**
     * @brief ステアの移動方向をセットする
     * 
     * @param val -1.0 ~ 1.0
     */
    void setSteerDirection(float val)
    {
        controller_data.cmd.steer.dir = val * controller_data.parameter.steer.max_angle;
    }

    /**
     * @brief ステアの旋回方向をセットする
     * 
     * @param val -1.0 ~ 1.0 ,val > border_cw: 左回転, val < -border_cw: 右回転
     */
    void setSteerTurn(float val)
    {
        if(val > controller_data.parameter.steer.cw_border){
            controller_data.cmd.steer.turn = 1;
        }else if(val < -controller_data.parameter.steer.cw_border){
            controller_data.cmd.steer.turn = -1;
        }else{
            controller_data.cmd.steer.turn = 0;
        }
    }

    /**
     * @brief アームの握力をセットする
     * 
     * @param val -1.0 ~ 1.0
     */
    void setArmEffort(float val)
    {
        controller_data.cmd.arm.effort = val * controller_data.parameter.arm.max_effort;
    }

    /**
     * @brief ステアの速度を取得する
     * 
     * @return [m/s]
     */
    float getSteerVelocity(){
        return controller_data.cmd.steer.vel;
    }

    /**
     * @brief ステアの移動方向を取得する
     */
    float getSteerDirection()
    {
        return controller_data.cmd.steer.dir;
    }

    /**
     * 
     * @brief ステアの旋回方向を取得する
     * 
     * @return 1: 左回転, 0: 無回転, -1: 右回転
     */
    int getSteerTurn()
    {
        return controller_data.cmd.steer.turn;
    }

    /**
     * @brief アームの握力を取得する
     */
    float getArmEffort()
    {
        return controller_data.cmd.arm.effort;
    }

    /**
     * @brief コントローラのパラメータを取得する
     */
    ControllerParameter getParameter()
    {
        return controller_data.parameter;
    }

    /**
     * @brief Ts(1ms)間隔で処理する関数
     */
    void update_1ms(){
        ;
    }

    /**
     * @brief whileループで回す
     */
    void update()
    {
        ;
    }

    /**
     * @brief デバッグ関数
     */
    void print_debug()
    {
        printf("controller_data{");

        printf("steer{");
        printf("cmd{");
        printf("vel: %0.2f",controller_data.cmd.steer.vel);
        printf("|");
        printf("dir: %0.2f",controller_data.cmd.steer.dir);
        printf("|");
        printf("turn: %c",(controller_data.cmd.steer.turn == 1)?'R':(controller_data.cmd.steer.turn == -1)?'L':'N');
        printf("}");
        printf("|");

        printf("robot{");
        printf("cmd{");
        printf("es: %d",controller_data.cmd.robot.es);
        printf("}");
        printf("}");

        printf("}");
        printf("}");
        printf("|");
    }
private:
    struct{
        struct{
            struct{
                float vel; // turn == 0: 移動速度, turn != 1: 旋回速度
                float dir; // 移動方向
                int turn; // -1:右回転, 0:無回転, 1:左回転
            }steer;
            struct{
                float effort; // 握力
            }arm;
            struct{
                bool es;
            }robot;
        }cmd;
        ControllerParameter parameter;
    }controller_data;
};

#endif // NHK2025B_CONTROLLER_H