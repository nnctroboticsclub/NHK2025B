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
    PinName tx = pins.PUROPO_TX;
    PinName rx = pins.PUROPO_RX;
};
class NHK2025B_Puropo{
public:
    /**
     * @brief コンストラクタ
     */
    NHK2025B_Puropo(std::array<PuropoParameter,NUM_OF_PUROPO> param={{PuropoParameter()}}) : puropo(param[0].tx, param[0].rx)
    #if NUM_OF_PUROPO == 2
    puropo(param[1].tx,param[1].rx)
    #endif 
    {}

    /**
     * @brief セットアップ関数
     */
    void setup()
    {
        puropo.start();
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
            puropo_data[i].state.communicatable = puropo.is_ok();
            puropo_data[i].state.left_x = puropo.get(PUROPO_LEFT_X);
            puropo_data[i].state.left_y = -puropo.get(PUROPO_LEFT_Y);
            puropo_data[i].state.right_x = -puropo.get(PUROPO_RIGHT_X);
            puropo_data[i].state.right_y = puropo.get(PUROPO_RIGHT_Y);
            for(int i=5;i<=10;i++){
                puropo_data[i].state.axis[i-1] = puropo.get(i);
            }
        }
        
    }

    /**
     * @brief デバッグ関数
     */
    void print_debug()
    {
        int i;
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
            for(int i=0;i<6;i++){
                printf("%0.2f,", puropo_data[i].state.axis[i]);
            }
            printf("]");
    
            printf("}");
            printf("}");
            printf("|");
        }
    }
private:
    Puropo puropo;
    struct{
        struct{
            float left_x;
            float left_y;
            float right_x;
            float right_y;
            float axis[6];
            bool communicatable = false;
        }state;
    }puropo_data[NUM_OF_PUROPO];
};

#endif // NHK2025B_PUROPO_H