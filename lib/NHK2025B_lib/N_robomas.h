/**
 * @file N_template.h
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
    // RobomasParameter(){
    //     robomas_id = 1;
    //     float max_current = 5.0;
    //     type = TYPE_OF_M3508;
    //     ican_ptr = &can1;
    // }
    // RobomasParameter* set_robomas_id(int id){robomas_id = id;return this;}
    // RobomasParameter* set_max_current(float cur){max_current = cur;return this;}
    // RobomasParameter* set_type(TypeOfRobomas t){type = t;return this;}
    // RobomasParameter* set_ican_ptr(ikarashiCAN_mk2 *ican){ican_ptr = ican;return this;};
};

class NHK2025B_Robomas{
public:
    /**
     * @brief コンストラクタ
     * 
     * @param param パラメータの配列
     * @param num param[]の要素数
     */
    NHK2025B_Robomas(std::array<RobomasParameter,NUM_OF_ROBOMAS> param) : robomas_sender_(&can2)
    {
        for(int i=0;i<param_.size();i++){
            // 配列の0番目から順にデバイス番号0から割り振られていく
            param_[i] = param[i];
        }
    }

    /**
     * @brief セットアップ関数
     */
    void setup()
    {
        for(int i=0;i<NUM_OF_ROBOMAS;i++){
            int m3508_i=0,m2006_i=0;
            if(param_[i].type == param_[i].TYPE_OF_M3508){
                // param_[i].motor_params.p_pairs = m3508.p_pairs;
                // param_[i].motor_params.L = m3508.L;    
                // param_[i].motor_params.R = m3508.R;    
                // param_[i].motor_params.gear_ratio = m3508.gear_ratio; 
                // m3508[m3508_i]=IkakoM3508(param_[i].robomas_id);
                // motor_config[i] = m3508[m3508_i].get_motor();
                m3508[m3508_i].set_params(param_[i].robomas_id);
                robomas_[i] = &m3508[m3508_i];
                m3508_i++;
            }
            if(param_[i].type == param_[i].TYPE_OF_M2006){
                // param_[i].motor_params.p_pairs = m2006.p_pairs;    
                // param_[i].motor_params.L = m2006.L;    
                // param_[i].motor_params.R = m2006.R;    
                // param_[i].motor_params.gear_ratio = m2006.gear_ratio;    
                // m2006[m2006_i].set_params(param_[i].robomas_id);
                // motor_config[i] = m2006[m2006_i].get_motor();
                m2006[m2006_i].set_params(param_[i].robomas_id);
                robomas_[i] = &m2006[m2006_i];
                m2006_i++;
            }
            // param_[i].motor_params.D = 0.0;
            // param_[i].motor_params.J = 0.04;    
            robomas_sender_.set_motors(robomas_[i]->get_motor());
            param_[i].max_current = 5.0;
        }
        robomas_sender_data.state.write_cnt = 0;
    }

    /**
     * @brief whileループする
     */
    void update()
    {
        for(int i=0;i<param_.size();i++){
            robomas_[i]->set_ref(robomas_data[i].cmd.current);
        }
    }

    /**
     * @brief ts間隔(1ms)でループする(重い処理は入れてはならない)
     */
    void update_ts()
    {
        ;
    }

    /**
     * @brief パラメータをセットする
     * 
     * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
     * @param param パラメータ
     */
    void setRobomasParameter(int num,RobomasParameter param)
    {
        param_[num] = param;
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
        return param_[num];
    }

    void write()
    {
        robomas_sender_.setIcan(param_[robomas_sender_data.state.write_cnt%NUM_OF_ROBOMAS].ican_ptr);
        robomas_sender_.write();
    }

    void setCurrent(int num,float cur)
    {
        robomas_data[num].cmd.current = cur;
    }


    /**
     * @brief デバッグ用関数
     */
    void print_debug()
    {
        ;
    }

private:
    std::array<RobomasParameter,NUM_OF_ROBOMAS> param_;
    std::array<ikako_robomas_motor_config*, NUM_OF_ROBOMAS> motor_config;
    std::array<IkakoM3508, NUM_OF_ROBOMAS_M3508> m3508;
    std::array<IkakoM2006, NUM_OF_ROBOMAS_M2006> m2006;
    std::array<IkakoMotor*,NUM_OF_ROBOMAS> robomas_;
    IkakoRobomasSender robomas_sender_;
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
        }state;
    }robomas_data[NUM_OF_ROBOMAS];

    struct{
        struct{
            int write_cnt;
        }state;
    }robomas_sender_data;
};

#endif // NHK2025B_ROBOMAS_H
