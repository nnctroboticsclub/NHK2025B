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

struct ControllerParameter {
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_Controller{
public:
    /**
     * @brief コンストラクタ
     */
    NHK2025B_Controller(){}

    /**
     * @brief セットアップ
     */
    void setup()
    {
        controller_data.steer.parameter.cw_border = 0.5;
        controller_data.steer.parameter.max_vel = STEER_MAX_VELOCITY;
        controller_data.steer.parameter.max_angle = STEER_MAX_RAD;
    }

    /**
     * @brief ステアの速度をセットする
     * 
     * @param val -1.0 ~ 1.0
     */
    void setSteerVelocity(float val)
    {
        controller_data.steer.cmd.vel = val * controller_data.steer.parameter.max_vel;
    }

    /**
     * @brief ステアの移動方向をセットする
     * 
     * @param val -1.0 ~ 1.0
     */
    void setSteerDirection(float val)
    {
        controller_data.steer.cmd.dir = val * controller_data.steer.parameter.max_angle;
    }

    /**
     * @brief ステアの旋回方向をセットする
     * 
     * @param val -1.0 ~ 1.0 ,val > border_cw: 左回転, val < -border_cw: 右回転
     */
    void setSteerTurn(float val)
    {
        val = 0;
        controller_data.steer.cmd.turn = (val > controller_data.steer.parameter.cw_border)?1:val;
        controller_data.steer.cmd.turn = (val < -controller_data.steer.parameter.cw_border)?-1:val;
    }

    /**
     * @brief ステアの速度を取得する
     * 
     * @return [m/s]
     */
    float getSteerVelocity(){
        return controller_data.steer.cmd.vel;
    }

    /**
     * @brief ステアの移動方向を取得する
     */
    float getSteerDirection()
    {
        return controller_data.steer.cmd.dir;
    }

    /**
     * 
     * @brief ステアの旋回方向を取得する
     * 
     * @return 1: 左回転, 0: 無回転, -1: 右回転
     */
    int getSteerTurn()
    {
        return controller_data.steer.cmd.turn;
    }

    /**
     * @brief ステアの最大速度を取得する
     * 
     * @return [m/s]
     */
    float getSteerMaxVelocity()
    {
        return controller_data.steer.parameter.max_vel;
    }

    /**
     * @brief ステアの最大可動角度を取得する
     * 
     * @return [rad]
     */
    float getSteerMaxAngle()
    {
        return controller_data.steer.parameter.max_angle;
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
        printf("vel: %0.2f",controller_data.steer.cmd.vel);
        printf("|");
        printf("dir: %0.2f",controller_data.steer.cmd.dir);
        printf("|");
        printf("turn: %c",(controller_data.steer.cmd.turn == 1)?'L':(controller_data.steer.cmd.turn == -1)?'R':'N');
        printf("}");
        printf("|");

        printf("robot{");
        printf("cmd{");
        printf("es: %d",controller_data.robot.cmd.es);
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
            }cmd;
            struct{
                float max_vel; // 最大速度[m/s]
                float max_angle; // 最大可動角度[rad]
                float cw_border; // 旋回するかを判定する境界値
            }parameter;
        }steer;
        struct{
            struct{
                bool es;
            }cmd;
        }robot;
    }controller_data;
};

#endif // NHK2025B_CONTROLLER_H