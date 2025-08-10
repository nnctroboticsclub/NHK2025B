/**
 * @file N_robomas.h
 * @author 高野 絆(takanokiduna@gmail.com)
 * @brief NHK2025Bのロボマスクラス
 * @version 0.1
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025
 */
#ifndef NHK2025B_ROBOMAS_H
#define NHK2025B_ROBOMAS_H

#include <mbed.h>
#include "ikako_m3508.h"
#include "ikako_m2006.h"
#include "ikako_robomas.h"
#include "ikarashiCAN_mk2.h"

#include "definitions.h"

class RobomasParameter{
public:
    int robomas_id = 1;
    float max_current = 5.0;
    enum{
        TYPE_OF_M2006,
        TYPE_OF_M3508
    }type = TYPE_OF_M3508;
    ikarashiCAN_mk2 *ican_ptr = &can1;
};

class NHK2025B_Robomas{
public:
    /**
     * @brief コンストラクタ
     * 
     * @param param パラメータの配列
     */
    NHK2025B_Robomas(std::array<RobomasParameter,NUM_OF_ROBOMAS> param={{RobomasParameter()}}) : robomas_sender_{{IkakoRobomasSender(&can1),IkakoRobomasSender(&can2)}}
    {
        for(int i=0;i<NUM_OF_ROBOMAS;i++){
            // 配列の0番目から順にデバイス番号0から割り振られていく
            robomas_data[i].parameter = param[i];
        }
    }

    /**
     * @brief セットアップ関数
     */
    void setup()
    {
        robomas_sender_data.state.use_can1_flag = false;
        robomas_sender_data.state.use_can2_flag = false;
        robomas_sender_data.state.write_cnt = 0;
        for(int i=0,m3508_i=0,m2006_i=0;i<NUM_OF_ROBOMAS;i++){
            robomas_data[i].state.rev = 0;
            if(robomas_data[i].parameter.type == robomas_data[i].parameter.TYPE_OF_M3508){
                m3508[m3508_i].set_params(robomas_data[i].parameter.robomas_id);
                robomas_[i] = &m3508[m3508_i];
                robomas_data[i].state.gear_ratio = m3508[m3508_i].gear_ratio;
                printf("%p,%p\n",robomas_[i],&m3508[m3508_i]);
                m3508_i++;
            }else if(robomas_data[i].parameter.type == robomas_data[i].parameter.TYPE_OF_M2006){
                m2006[m2006_i].set_params(robomas_data[i].parameter.robomas_id);
                robomas_data[i].state.gear_ratio = m2006[m2006_i].gear_ratio;
                robomas_[i] = &m2006[m2006_i];
                m2006_i++;
            }
            if(robomas_data[i].parameter.ican_ptr == &can1){
                robomas_sender_[0].set_motors(robomas_[i]->get_motor());
                robomas_sender_data.state.use_can1_flag = true;
            }else if(robomas_data[i].parameter.ican_ptr == &can2){
                robomas_sender_[1].set_motors(robomas_[i]->get_motor());
                robomas_sender_data.state.use_can2_flag = true;
            }
        }
    }

    /**
     * @brief whileでループする
     */
    void update()
    {
        for(int i=0;i<NUM_OF_ROBOMAS;i++){
            if(robomas_data[i].parameter.robomas_id != 0){
                robomas_[i]->set_ref(robomas_data[i].cmd.current);
            }
        }
    }

    /**
     * @brief ts間隔(1ms)でループする(重い処理は入れてはならない)
     */
    void update_ts()
    {
        if(robomas_sender_data.state.use_can1_flag){
            robomas_sender_[0].read();
        }
        if(robomas_sender_data.state.use_can2_flag){
            robomas_sender_[1].read();
        }

        for(int i=0;i<NUM_OF_ROBOMAS;i++){
            robomas_data[i].state.angle = robomas_[i]->get_angle();
            robomas_data[i].state.torque = robomas_[i]->get_torque();
            robomas_data[i].state.vel = robomas_[i]->get_vel(robomas_data[i].state.gear_ratio);
            if(robomas_data[i].state.vel > 0.5){
                if(robomas_data[i].state.angle < robomas_data[i].state.pre_angle){
                    robomas_data[i].state.rev++;
                }
            }else{
                if((robomas_data[i].state.angle <= (120 * M_PI / 180)) && 
                (robomas_data[i].state.pre_angle >= (240 * M_PI / 180))){
                    robomas_data[i].state.rev++;
                }
            }
            if(robomas_data[i].state.vel < -0.5){
                if(robomas_data[i].state.angle > robomas_data[i].state.pre_angle){
                    robomas_data[i].state.rev--;
                }
            }else{
                if((robomas_data[i].state.angle >= (240 * M_PI / 180)) && 
                (robomas_data[i].state.pre_angle <= (120 * M_PI / 180))){
                robomas_data[i].state.rev--;
                }
            }

            robomas_data[i].state.pre_angle = robomas_data[i].state.angle;
            robomas_data[i].state.abs_angle = robomas_data[i].state.angle + robomas_data[i].state.rev * M_TWOPI;
        }
    }

    /**
     * @brief パラーメータを取得する
     * 
     * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
     * 
     * @return パラメータ
     */
    RobomasParameter getParam(int num)
    {
        return robomas_data[num].parameter;
    }

    /**
     * @brief ロボマスの回転カウントなどのステータスをリセットする
     * 
     * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
     */
    void resetState(int num)
    {
        memset(&robomas_data[num].state,0,sizeof(robomas_data->state));
    }

    /**
     * @brief send_threadの中に入れる
     */
    void write()
    {
        if(robomas_sender_data.state.use_can1_flag & robomas_sender_data.state.use_can2_flag){
            robomas_sender_[robomas_sender_data.state.write_cnt % NUM_OF_CAN].write();
        }else if(robomas_sender_data.state.use_can1_flag){
            robomas_sender_[0].write();
        }else{
            robomas_sender_[1].write();
        }
        robomas_sender_data.state.write_cnt++;
    }

    /**
     * @brief 電流値をセットする
     * 
     * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
     * @param cur 電流値[A]
     * 
     */
    void setCurrent(int num,float cur)
    {
        robomas_data[num].cmd.current = cur;
    }

    /**
     * @brief 回転角度を取得する
     * 
     * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
     * 
     * @return [rad]
     */
    float getAngle(int num)
    {
        return robomas_data[num].state.angle;
    }

    float getAbsAngle(int num)
    {
        return robomas_data[num].state.abs_angle;
    }


    /**
     * @brief 回転速度を取得する
     * 
     * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
     * 
     * @return [rad/s]
     */
    float getVeclocity(int num)
    {
        return robomas_data[num].state.vel;
    }

    /**
     * @brief トルクを取得する
     * 
     * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
     * 
     * @return [N・m]
     */
    float getTorque(int num)
    {
        return robomas_data[num].state.torque;
    }

    /**
     * @brief デバッグ用関数
     */
    void print_debug()
    {
        for (int i = 0; i < NUM_OF_ROBOMAS; i++) {
            printf("robomas_data[%d]{", i);
    
            printf("cmd{");
            printf("current: %.2f", robomas_data[i].cmd.current);
            printf("}");
    
            printf("state{");
            printf("rev: %d", robomas_data[i].state.rev);
            printf("|pre_angle: %.2f", robomas_data[i].state.pre_angle);
            printf("|abs_angle: %.2f", robomas_data[i].state.abs_angle);
            printf("|angle: %.2f", robomas_data[i].state.angle);
            printf("|vel: %.2f", robomas_data[i].state.vel);
            printf("|torque: %.2f", robomas_data[i].state.torque);
            printf("}");
    
            printf("|}");  // robomas_data 閉じ、区切り
        }
    
        printf("robomas_sender_data{");
        printf("state{");
        printf("write_cnt: %d", robomas_sender_data.state.write_cnt);
        printf("|use_can1_flag: %d", robomas_sender_data.state.use_can1_flag);
        printf("|use_can2_flag: %d", robomas_sender_data.state.use_can2_flag);
        printf("}");
        printf("}");  // robomas_sender_data 閉じ
    }

private:
    std::array<ikako_robomas_motor_config*, NUM_OF_ROBOMAS> motor_config;
    std::array<IkakoM3508, NUM_OF_ROBOMAS_M3508> m3508;
    std::array<IkakoM2006, NUM_OF_ROBOMAS_M2006> m2006;
    std::array<IkakoMotor*,NUM_OF_ROBOMAS> robomas_;
    std::array<IkakoRobomasSender,NUM_OF_CAN> robomas_sender_;
    struct{
        struct{
            float current;
        }cmd;
        struct{
            int rev; // 減速後の蓄積回転数[回]
            float pre_angle; // 前回の減速後の角度[rad]
            float abs_angle; // 減速後の蓄積した角度[rad]
            float angle;
            float vel;
            float torque; // [N]
            float gear_ratio;
        }state;
        RobomasParameter parameter;
    }robomas_data[NUM_OF_ROBOMAS];

    struct{
        struct{
            int write_cnt;
            bool use_can1_flag;
            bool use_can2_flag;
        }state;
    }robomas_sender_data;
};

#endif // NHK2025B_ROBOMAS_H
