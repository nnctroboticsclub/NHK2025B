/**
 * @file N_robomas.h
 * @author 鷲見 建太 (takanokiduna@gmail.com)
 * @brief NHK2025B用ロボマス制御クラス
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

#define M3508_MAX_RAD_PER_SEC 48

class RobomasParameter {
public:
    int robomas_id = 1;
    float max_current = 5.0;
    enum {
        TYPE_OF_M2006,
        TYPE_OF_M3508
    } type = TYPE_OF_M3508;
    ikarashiCAN_mk2 *ican_ptr = &can2;
};

class NHK2025B_Robomas {
public:
    /**
     * @brief コンストラクタ
     * 
     * @param param モーターごとの設定配列
     */
    NHK2025B_Robomas(std::array<RobomasParameter, NUM_OF_ROBOMAS> param = {{RobomasParameter()}})
        : robomas_sender_{{IkakoRobomasSender(&can1), IkakoRobomasSender(&can2)}} {
        for (int i = 0; i < NUM_OF_ROBOMAS; i++) {
            // 最初の構成順を使ってロボマスモーター配列に設定
            robomas_data[i].parameter = param[i];
        }
    }

    /**
     * @brief 初期化関数（setup）
     */
    void setup() {
        robomas_sender_data.state.use_can1_flag = false;
        robomas_sender_data.state.use_can2_flag = false;
        robomas_sender_data.state.write_cnt = 0;

        for (int i = 0, m3508_i = 0, m2006_i = 0; i < NUM_OF_ROBOMAS; i++) {
            robomas_data[i].state.rev = 0;
            if (robomas_data[i].parameter.type == robomas_data[i].parameter.TYPE_OF_M3508) {
                m3508[m3508_i].set_params(robomas_data[i].parameter.robomas_id);
                robomas_[i] = &m3508[m3508_i];
                robomas_data[i].state.gear_ratio = m3508[m3508_i].gear_ratio;
                m3508_i++;
            } else if (robomas_data[i].parameter.type == robomas_data[i].parameter.TYPE_OF_M2006) {
                m2006[m2006_i].set_params(robomas_data[i].parameter.robomas_id);
                robomas_data[i].state.gear_ratio = m2006[m2006_i].gear_ratio;
                robomas_[i] = &m2006[m2006_i];
                m2006_i++;
            }

            if (robomas_data[i].parameter.ican_ptr == &can1) {
                robomas_sender_[0].set_motors(robomas_[i]->get_motor());
                robomas_sender_data.state.use_can1_flag = true;
            } else if (robomas_data[i].parameter.ican_ptr == &can2) {
                robomas_sender_[1].set_motors(robomas_[i]->get_motor());
                robomas_sender_data.state.use_can2_flag = true;
            }
        }
    }

    /**
     * @brief 通常ループでの更新関数
     */
    void update() {
        for (int i = 0; i < NUM_OF_ROBOMAS; i++) {
            if (robomas_data[i].parameter.robomas_id != 0) {
                robomas_[i]->set_ref(robomas_data[i].cmd.current);
            }
        }
    }

    /**
     * @brief 1ms周期での定期更新処理
     */
    void update_ts() {
        if (robomas_sender_data.state.use_can1_flag) {
            robomas_sender_[0].read();
        }
        if (robomas_sender_data.state.use_can2_flag) {
            robomas_sender_[1].read();
        }

        for (int i = 0; i < NUM_OF_ROBOMAS; i++) {
            auto &data = robomas_data[i];
            data.state.angle = robomas_[i]->get_angle();
            data.state.torque = robomas_[i]->get_torque();
            data.state.vel = robomas_[i]->get_vel(data.state.gear_ratio);

            if (data.state.vel > 0.5f) {
                if (data.state.angle < data.state.pre_angle) {
                    data.state.rev++;
                }
            } else {
                if ((data.state.angle <= (120 * M_PI / 180)) &&
                    (data.state.pre_angle >= (240 * M_PI / 180))) {
                    data.state.rev++;
                }
            }

            if (data.state.vel < -0.5f) {
                if (data.state.angle > data.state.pre_angle) {
                    data.state.rev--;
                }
            } else {
                if ((data.state.angle >= (240 * M_PI / 180)) &&
                    (data.state.pre_angle <= (120 * M_PI / 180))) {
                    data.state.rev--;
                }
            }

            data.state.pre_angle = data.state.angle;
            data.state.abs_angle = data.state.angle + data.state.rev * M_TWOPI;
        }
    }

    /**
     * @brief モーター設定を取得
     */
    RobomasParameter getParam(int num) {
        return robomas_data[num].parameter;
    }

    /**
     * @brief モーター状態をリセット
     */
    void resetState(int num) {
        memset(&robomas_data[num].state, 0, sizeof(robomas_data[num].state));
    }

    /**
     * @brief CAN送信用のwrite関数（スレッドから呼び出す）
     */
    void write() {
        if (robomas_sender_data.state.use_can1_flag && robomas_sender_data.state.use_can2_flag) {
            robomas_sender_[robomas_sender_data.state.write_cnt % NUM_OF_CAN].write();
        } else if (robomas_sender_data.state.use_can1_flag) {
            robomas_sender_[0].write();
        } else {
            robomas_sender_[1].write();
        }
        robomas_sender_data.state.write_cnt++;
    }

    /**
     * @brief 電流指令を設定
     */
    void setCurrent(int num, float cur) {
        robomas_data[num].cmd.current = cur;
    }

    /**
     * @brief 現在角度を取得 [rad]
     */
    float getAngle(int num) {
        return robomas_data[num].state.angle;
    }

    /**
     * @brief 累積回転角度を取得する [rad]
     */
    float getAbsAngle(int num)
    {
        return robomas_data[num].state.abs_angle;
    }

    /**
     * @brief 回転速度を取得する [rad/s]
     */
    float getVeclocity(int num)
    {
        return robomas_data[num].state.vel;
    }

    /**
     * @brief 現在トルクを取得 [N・m]
     */
    float getTorque(int num) {
        return robomas_data[num].state.torque;
    }

    /**
     * @brief 蓄積した回転角度を取得する[rad]
     */
    float getAbsAngle(int num)
    {
        return robomas_data[num].state.abs_angle;
    }

    /**
     * @brief デバッグ用関数
     */
    void print_debug() {
        for (int i = 0; i < NUM_OF_ROBOMAS; i++) {
            printf("robomas_data[%d]{", i);
            printf("cmd{current: %.2f}", robomas_data[i].cmd.current);
            printf(" state{rev: %d | pre_angle: %.2f | abs_angle: %.2f | angle: %.2f | vel: %.2f | torque: %.2f}",
                   robomas_data[i].state.rev,
                   robomas_data[i].state.pre_angle,
                   robomas_data[i].state.abs_angle,
                   robomas_data[i].state.angle,
                   robomas_data[i].state.vel,
                   robomas_data[i].state.torque);
            printf(" }\n");
        }

        printf("robomas_sender_data{state{write_cnt: %d | use_can1_flag: %d | use_can2_flag: %d}}\n",
               robomas_sender_data.state.write_cnt,
               robomas_sender_data.state.use_can1_flag,
               robomas_sender_data.state.use_can2_flag);
    }

private:
    std::array<ikako_robomas_motor_config*, NUM_OF_ROBOMAS> motor_config;
    std::array<IkakoM3508, NUM_OF_ROBOMAS_M3508> m3508;
    std::array<IkakoM2006, NUM_OF_ROBOMAS_M2006> m2006;
    std::array<IkakoMotor*, NUM_OF_ROBOMAS> robomas_;
    std::array<IkakoRobomasSender, NUM_OF_CAN> robomas_sender_;

    struct {
        struct {
            float current;
        } cmd;

        struct {
            int rev;
            float pre_angle;
            float abs_angle;
            float angle;
            float vel;
            float torque;
            float gear_ratio;
        } state;

        RobomasParameter parameter;
    } robomas_data[NUM_OF_ROBOMAS];

    struct {
        struct {
            int write_cnt;
            bool use_can1_flag;
            bool use_can2_flag;
        } state;
    } robomas_sender_data;
};

#endif // NHK2025B_ROBOMAS_H
