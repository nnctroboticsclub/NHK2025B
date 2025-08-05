/**
 * @file N_rohmMD.h
 * @author 高野 絆(takanokiduna@gmail.com)
 * @brief NHK2025BのロームMDクラス
 * @version 0.1
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025
 */
#ifndef NHK2025B_ROHM_MD_H
#define NHK2025B_ROHM_MD_H

#include <mbed.h>
#include "rohm_md.h"
#include "definitions.h"
#include "ikarashiCAN_mk2.h"

class RohmMdParameter{
public:
    int id = 1;
    ikarashiCAN_mk2* ican = &can1;
};

class NHK2025B_RohmMD{
public:
    /**
     * @brief コンストラクタ
     * 
     * @param param パラメータの配列
     */
    NHK2025B_RohmMD(std::array<RohmMdParameter,NUM_OF_ROHM_MD> param = {{RohmMdParameter()}})
    {
        for(int i=0;i<param.size();i++){
            rohmMD_data[i].parameter = param[i];
            md[i] = RohmMD(rohmMD_data[i].parameter.ican,rohmMD_data[i].parameter.id);
        }
    }

    /**
     * @brief セットアップ関数
     */
    void setup()
    {
        cnt = 0;
        // for(int i=0;i<NUM_OF_ROHM_MD;i++){
        //     md[i].print_debug();
        // }
    }

    /**
     * @brief send_threadの中に入れる
     */
    void write()
    {
        md[cnt%NUM_OF_ROHM_MD].send();
        cnt++;
    }

    /**
     * @brief パラーメータを取得する
     * 
     * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
     * 
     * @return パラメータ
     */
    RohmMdParameter getRohmMdParameter(int num)
    {
        return rohmMD_data[num].parameter;
    }

    /**
     * @brief 出力をセットする
     * 
     * @param -1.0 < pow < 1.0
     */
    void setPower(int num,float pow)
    {
        rohmMD_data[num].cmd.power = pow;
    }

    /**
     * @brief whileでループする
     */
    void update()
    {
        for(int i=0;i<NUM_OF_ROHM_MD;i++)
        {
            md[i].set(0,rohmMD_data[i].cmd.power);
            md[i].read();
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
        // struct{
        //     float angle;
        //     float velocity;
        //     float current;
        // }state;
        RohmMdParameter parameter;
    }rohmMD_data[NUM_OF_ROHM_MD];
    int cnt = 0;
};

#endif // NHK2025B_ROHM_MD_H