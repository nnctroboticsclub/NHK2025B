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
    int board_id = 1;
    ikarashiCAN_mk2* ican = &can1;
    float max_power = 1.0;
    bool is_enable = false;
};

class NHK2025B_IkakoMDC{
public:
    /**
     * @brief コンストラクタ
     */
    NHK2025B_IkakoMDC(std::array<IkakoMdcParameter, NUM_OF_IKAKO_MD> param = {{IkakoMdcParameter()}}) {
        printf("=== NHK2025B_IkakoMDC Constructor Start ===\n");
        struct AddIndex {
            IkakoMdcParameter param;
            int index;
        };
        std::array<AddIndex, NUM_OF_IKAKO_MD> buf;
        int i;
        auto can_number = [](ikarashiCAN_mk2* ican) {
            return (ican == &can1) ? 1 : (ican == &can2) ? 2 : 0;
        };
        printf("[1] パラメータ代入開始\n");
        for (i = 0; i < NUM_OF_IKAKO_MD; i++) {
            md_data[i].parameter = param[i];
            buf[i].param = param[i];
            buf[i].index = i;
            printf("  param[%d] ican=%p, board_id=%d, id=%d\n", i, param[i].ican, param[i].board_id, param[i].id);
        }
        printf("[2] ソート開始\n");
        std::sort(buf.begin(), buf.end(),
            [can_number](const struct AddIndex &a, const struct AddIndex &b){
                return (can_number(a.param.ican) < can_number(b.param.ican)) &&
                       (a.param.board_id < b.param.board_id) &&
                       (a.param.id < b.param.id);
            }
        );
        printf("[3] ソート完了\n");
        for (i = 0; i < NUM_OF_IKAKO_MD; i++) {
            printf("  sorted buf[%d]: ican=%p, board_id=%d, id=%d\n",
                i, buf[i].param.ican, buf[i].param.board_id, buf[i].param.id);
        }
        printf("[4] ikakoMDC インスタンス生成開始\n");

        int mdc_size=0;
        for(i=0,md_num=mdc_num=0;i<NUM_OF_IKAKO_MD;i++){
            if(!buf[i].param.is_enable) continue;
            if((i!=0)&&
            ((buf[i].param.ican != buf[i-1].param.ican)||
            buf[i].param.board_id != buf[i-1].param.board_id)){
                mdc[mdc_num] = ikakoMDC_sender(&md[md_num - mdc_size], mdc_size,
                    buf[i-1].param.ican, buf[i-1].param.board_id);
                printf("  mdc[%d] 作成: 開始md=%d, 数=%d, ican=%p, board_id=%d\n",
                       mdc_num, md_num - mdc_size, mdc_size, buf[i-1].param.ican, buf[i-1].param.board_id);
                mdc_num++;
                mdc_size = 0;
            }
            printf("  md[%d] 作成: id=%d\n", md_num, buf[i].param.id);
            md[md_num] = ikakoMDC(buf[i].param.id, INT8_MIN, INT8_MAX,
                                    1e-3f, 0.0f, 2.7f, 0.0f, 1.5e-5f, 1e2f);
            md_data[buf[i].index].state.md = &md[md_num];
            md_data[buf[i].index].state.mdc = &mdc[mdc_num];
            md_num++;
            mdc_size++;
        }
        if(mdc_size>0){
            printf("  mdc[%d] 作成: 開始md=%d, 数=%d, ican=%p, board_id=%d\n",
                   mdc_num, md_num - mdc_size, mdc_size, buf[i-1].param.ican, buf[i-1].param.board_id);
            mdc[mdc_num] = ikakoMDC_sender(&md[md_num - mdc_size], mdc_size,
                                             buf[i-1].param.ican, buf[i-1].param.board_id);
            mdc_num++;
        }
        cnt_write = 0;
        printf("[5] コンストラクタ完了 cnt_write = %d\n", cnt_write);
        printf("=== NHK2025B_IkakoMDC Constructor End ===\n");
    }
    
    /**
     * @brief セットアップ関数
     */
    void setup()
    {
        for(int i=0;i<md_num;i++){
            ;
        }
    }

    /**
     * @brief 全てのモータにデータを送信
     */
    void write()
    {
        if(md_data[cnt_write%NUM_OF_IKAKO_MD].parameter.is_enable){
            md_data[cnt_write%NUM_OF_IKAKO_MD].state.mdc->send();
        }
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

    // /**
    //  * @brief パラメータをセットする
    //  * 
    //  * @param num デバイス番号 (0 <= num < NUM_OF_ROBOMAS)
    //  * @param param パラメータ
    //  */
    // void setIkakoMdcParameter(int num,IkakoMdcParameter param)
    // {
    //     md_data[num].parameter = param;
    // }

    /**
     * @brief 出力をセットする
     * 
     * @param -1.0 < pow < 1.0
     */
    void setPower(int num,float pow)
    {
        md_data[num].cmd.power = pow / md_data[num].parameter.max_power;
    }


    /**
     * @brief whileでループする
     */
    void update()
    {
        for(int i=0;i<NUM_OF_IKAKO_MD;i++){
            if(md_data[i].parameter.is_enable){
                md_data[i].state.md->set_speed(
                    (int)(md_data[i].cmd.power * INT8_MAX) % INT8_MAX);
            }
        }
    }

    /**
     * @brief デバッグ用関数
     */
    void print_debug()
    {
        printf("md_data{");
        for (int i = 0; i < NUM_OF_IKAKO_MD; i++) {
            printf("md_data[%d]{", i);
    
            printf("cmd{");
            printf("power: %.2f", md_data[i].cmd.power);
            printf("}");
    
            printf("}");
    
        }
    
        printf("}");
        printf("|cnt_write: %d", cnt_write);
        printf("\n");
    }
private:
    ikakoMDC md[NUM_OF_IKAKO_MDC*4];
    ikakoMDC_sender mdc[NUM_OF_IKAKO_MDC];
    struct{
        struct{
            float power;
        }cmd;
        struct{
            ikakoMDC *md;
            ikakoMDC_sender *mdc;
        }state;
        IkakoMdcParameter parameter;
    }md_data[NUM_OF_IKAKO_MD];
    
    int cnt_write;
    int md_num;
    int mdc_num;
};

#endif // NHK2025B_N_ROHM_MD_H