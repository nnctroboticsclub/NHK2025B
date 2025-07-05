/**
 * @file N_puropo.h
 * @author 高野 絆(takanokiduna@gmail.com)
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 */
#ifndef NHK2025B_PUROPO_H
#define NHK2025B_PUROPO_H

#include <mbed.h>
#include "puropo.h"
#include "pinconfig.h"
#include "definitions.h"

#define PUROPO_LEFT_X 4 
#define PUROPO_LEFT_Y 2
#define PUROPO_RIGHT_X 1
#define PUROPO_RIGHT_Y 3 

class PuropoParameter{
public:
    // PinName tx = pins.PUROPO_TX;
    // PinName rx = pins.PUROPO_RX;
    PinName tx = pins.FEP_TX;
    PinName rx = pins.FEP_RX;
    int timeout = 500; // [ms]
};
class NHK2025B_Puropo{
public:
    /**
     * @brief コンストラクタ
     */
    NHK2025B_Puropo(std::array<PuropoParameter,NUM_OF_PUROPO> param={{PuropoParameter()}}) : puropo1(param[0].tx,param[0].rx)
    #if NUM_OF_PUROPO == 2
    ,Puropo(param[1].tx,param[1].rx)
    #endif 
    {
        for(int i=0;i<NUM_OF_PUROPO;i++){
            puropo_data[i].parameter = param[i];
        }
        puropo[0] = &puropo1;
        #if NUM_OF_PUROPO == 2
        puropo[1] = &puropo2;
        #endif
    }

    /**
     * @brief セットアップ関数
     */
    void setup()
    {
        for(int i=0;i<NUM_OF_PUROPO;i++){
            puropo[i]->start();
            puropo_data[i].state.timeout = 0;
        }
    }

    /**
     * @brief 受信が成功したかを取得する
     * 
     * @param num プロポの番号 0 <= num < NUM_OF_PUROPO
     * 
     * @return true: 受信成功, false: 受信失敗
     */
    bool getCommunicatable(int num)
    {
        return puropo_data[num].state.communicatable;
    }

    /**
     * @brief 左スティックのx軸を取得する
     * 
     * @param num プロポの番号 0 <= num < NUM_OF_PUROPO
     * 
     * @return -1.0 ~ 1.0
     */
    float getLeftX(int num)
    {
        return puropo_data[num].state.left_x;
    }

    /**
     * @brief 左スティックのy軸を取得する
     * 
     * @param num プロポの番号 0 <= num < NUM_OF_PUROPO
     * 
     * @return -1.0 ~ 1.0
     */
    float getLeftY(int num)
    {
        return puropo_data[num].state.left_y;
    }

    /**
     * @brief 右スティックのx軸を取得する
     * 
     * @param num プロポの番号 0 <= num < NUM_OF_PUROPO
     * 
     * @return -1.0 ~ 1.0
     */
    float getRightX(int num)
    {
        return puropo_data[num].state.right_x;
    }

    /**
     * @brief 右スティックのy軸を取得する
     * 
     * @param num プロポの番号 0 <= num < NUM_OF_PUROPO
     * 
     * @return -1.0 ~ 1.0
     */
    float getRightY(int num)
    {
        return puropo_data[num].state.right_y;
    }

    /**
     * @brief 受信したデータを取得する
     * 
     * @param num プロポの番号 0 <= num < NUM_OF_PUROPO
     * @param ch 5~6(1~4はスティック)
     * 
     * @return -1.0 ~ 1.0 
     */
    float getAxis(int num,int ch){
        return puropo_data[num].state.axis[ch-4];
    }

    /**
     * @brief whileで回す関数
     */
    void update()
    {
        for(int i=0;i<NUM_OF_PUROPO;i++){
            puropo_data[i].state.communicatable = (puropo_data[i].state.timeout < puropo_data[i].parameter.timeout);
            puropo_data[i].state.left_x = -puropo[i]->get(PUROPO_LEFT_X);
            puropo_data[i].state.left_y = puropo[i]->get(PUROPO_LEFT_Y);
            puropo_data[i].state.right_x = -puropo[i]->get(PUROPO_RIGHT_X);
            puropo_data[i].state.right_y = puropo[i]->get(PUROPO_RIGHT_Y);
            for(int j=5;j<=10;j++){
                puropo_data[i].state.axis[j-5] = puropo[i]->get(j);
            }
        }
        
    }

    /**
     * @brief ts間隔で処理する関数
     */
    void update_ts()
    {
        for(int i=0;i<NUM_OF_PUROPO;i++){
            if(puropo[i]->is_ok()){
                puropo_data[i].state.timeout = 0;
            }else{
                puropo_data[i].state.timeout++;
                // puropo_data[i].state.left_x = 0;
                // puropo_data[i].state.left_y = 0;
                // puropo_data[i].state.right_x = 0;
                // puropo_data[i].state.right_y = 0;
                // for(int j=0;j<6;j++) puropo_data[i].state.axis[j] = 0;
            }
        }
    }

    /**
     * @brief デバッグ関数
     */
    void print_debug()
    {
        // puropo[0]->debug();
        for(int i=0;i<NUM_OF_PUROPO;i++){
            printf("puropo_data{");
            printf("state{");
            printf("communicatable: %d",puropo_data[i].state.communicatable);
            printf("|");
            printf("LX: %0.2f",puropo_data[i].state.left_x);
            printf("|");
            printf("LY: %0.2f",puropo_data[i].state.left_y);
            printf("|");
            printf("RX: %0.2f",puropo_data[i].state.right_x);
            printf("|");
            printf("RY: %0.2f",puropo_data[i].state.right_y);
            printf("|");
            printf("axis:");
            printf("[");
            for(int j=0;j<6;j++){
                printf("%0.2f,", puropo_data[i].state.axis[j]);
            }
            printf("]");
    
            printf("}");
            printf("}");
            printf("|");
        }
    }
private:
    std::array<Puropo*, NUM_OF_PUROPO> puropo;
    Puropo puropo1;
    #if NUM_OF_PUROPO == 2
    Puropo puropo2;
    #endif
    struct{
        struct{
            float left_x;
            float left_y;
            float right_x;
            float right_y;
            float axis[6];
            bool communicatable = false;
            int timeout;
        }state;
        PuropoParameter parameter;
    }puropo_data[NUM_OF_PUROPO];
};

#endif // NHK2025B_PUROPO_H