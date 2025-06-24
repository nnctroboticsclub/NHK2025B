/**
 * @file K_ikakoMDC.h
 * @author 高野絆(takanokiduna@gmail.com)
 * @brief NHK2025BのikakoMDCクラス
 * @version 0.1
 * @date 2025-06-16
 * 
 * @copyright Copyright (c) 2025
 */
#ifndef NHK2025B_N_IKAKO_MDC_H
#define NHK2025B_N_IKAKO_MDC_H

#include <mbed.h>
#include "definitions.h"
#include "ikarashiCAN_mk2.h"
#include "ikakoMDC.h"

class IkakoMdcParameter {
public:
    int id = 0;
    int board_id = 0;
    ikarashiCAN_mk2* ican = &can1;
    int max_power = 100; // max_power < 125?
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_IkakoMDC{
public:
    /**
     * @brief コンストラクタ
     */
    NHK2025B_IkakoMDC(std::array<IkakoMdcParameter,NUM_OF_IKAKO_MD> param ={{IkakoMdcParameter()}}){
        for(int i=0;i<param.size();i++){
            md_data[i].parameter = param[i];
        }
        cnt_write = 0;
    }

    /**
     * @brief セットアップ関数
     */
    void setup()
    {
        for(int i=0;i<NUM_OF_IKAKO_MD;i++){
            // md[i] = ikakoMDC(md_data[i].parameter.id,-md_data[i].parameter.max_power,md_data[i].parameter.max_power,1e-3f,0.0f,0.0f,0.0f,1.5e-5f,1e-2f);

        }
    }

    /**
     * @brief 全てのモータにデータを送信
     */
    void write()
    {
        // mdc[cnt_write%NUM_OF_IKAKO_MDC].send();
        cnt_write++;
    }

    /**
     * @brief パラーメータを取得する
     * 
     * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
     * 
     * @return パラメータ
     */
    IkakoMdcParameter getIkakoMdcParameter(int num)
    {
        return md_data[num].parameter;
    }

    /**
     * @brief パラメータをセットする
     * 
     * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
     * @param param パラメータ
     */
    void setIkakoMdcParameter(int num,IkakoMdcParameter param)
    {
        md_data[num].parameter = param;
    }

    /**
     * @brief 出力をセットする
     * 
     * @param -1.0 < pow < 1.0
     */
    void setPower(int num,float pow)
    {
        md_data[num].cmd.power = pow;
    }


    /**
     * @brief whileでループする
     */
    void update()
    {
        for(int i=0;i<NUM_OF_IKAKO_MD;i++){
            md[i].set_speed((int)(md_data[i].cmd.power * md_data[i].parameter.max_power));
        }
    }

    /**
     * @brief デバッグ用関数
     */
    void print_debug()
    {
        ;
    }
private:
    struct{
        struct{
            float power;
        }cmd;
        struct{
            ;
        }state;
        IkakoMdcParameter parameter;
    }md_data[NUM_OF_IKAKO_MD];
    
    int cnt_write;
};

#endif // NHK2025B_N_ROHM_MD_H