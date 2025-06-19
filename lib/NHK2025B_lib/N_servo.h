/**
 * @file servo.h
 * @author 五十嵐幸多 (kotakota925ika@gmail.com)
 * @brief サーボモータ関係のクラス
 * @version 0.1
 * @date 2024-06-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef NHK2024A_SERVO_H
#define NHK2024A_SERVO_H

#include <mbed.h>
#include "can_servo.h"
#include <vector>
#include "definitions.h"
#include "pinconfig.h"

class NHK_servo
{
public:
    /**
     * @brief コンストラクタ
     *
     * @param _can canのアドレス
     * @param _id サーボボードのcanID
     */
    NHK_servo(ikarashiCAN_mk2 *_can, int _id)
    {
        can = _can;
        id = _id;
    }

    /**
     * @brief セットアップ
     *
     */
    void setup()
    {
        for (int i = 0; i < NUM_OF_SERVO_BOARD; i++)
        {
            servo.push_back(can_servo(can, id));
        }
    }

    /**
     * @brief サーボの指令値をセット
     *
     * @param num サーボモータの番号(0 ~ 7)
     * @param param 指令値
     */
    void set(int num, uint8_t param)
    {
        servo[0].set(num, param);
    }

    /**
     * @brief サーボの角度指令値をセット
     *
     * @param num サーボモータの番号(0 ~ 7)
     * @param deg 角度指令値[deg]
     * @param max_deg サーボモータの最大角度[deg]
     */
    void set_deg(int num, int deg, int max_deg)
    {
        servo[0].set(num, (float)deg / max_deg * 255);
    }

    /**
     * @brief ループ処理
     *
     */
    void update()
    {
        for (int i = 0; i < NUM_OF_SERVO_BOARD; i++)
            servo[i].send();
    }

private:
    std::vector<can_servo> servo;
    ikarashiCAN_mk2 *can;
    int id;
};

#endif