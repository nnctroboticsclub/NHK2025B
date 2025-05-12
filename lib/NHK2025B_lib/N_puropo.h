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

#define PUROPO_LEFT_X 4 
#define PUROPO_LEFT_Y 2
#define PUROPO_RIGHT_X 1
#define PUROPO_RIGHT_Y 3 

class NHK2025B_Puropo{
public:
    /**
     * @brief コンストラクタ
     */
    NHK2025B_Puropo() : puropo(pins.PUROPO_TX, pins.PUROPO_RX) {}

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
     * @return true: 受信成功, false: 受信失敗
     */
    bool getCommunicatable()
    {
        return puropo_data.state.communicatable;
    }

    /**
     * @brief 左スティックのx軸を取得する
     * 
     * @return -1.0 ~ 1.0
     */
    float getLeftX()
    {
        return puropo_data.state.left_x;
    }

    /**
     * @brief 左スティックのy軸を取得する
     * 
     * @return -1.0 ~ 1.0
     */
    float getLeftY()
    {
        return puropo_data.state.left_y;
    }

    /**
     * @brief 右スティックのx軸を取得する
     * 
     * @return -1.0 ~ 1.0
     */
    float getRightX()
    {
        return puropo_data.state.right_x;
    }

    /**
     * @brief 右スティックのy軸を取得する
     * 
     * @return -1.0 ~ 1.0
     */
    float getRightY()
    {
        return puropo_data.state.right_y;
    }

    /**
     * @brief 受信したデータを取得する
     * 
     * @param ch 5~6(1~4はスティック)
     * 
     * @return -1.0 ~ 1.0 
     */
    float getAxis(int ch){
        return puropo_data.state.axis[ch-4];
    }

    /**
     * @brief whileで回す関数
     */
    void update()
    {
        puropo_data.state.communicatable = puropo.is_ok();
        puropo_data.state.left_x = puropo.get(PUROPO_LEFT_X);
        puropo_data.state.left_y = -puropo.get(PUROPO_LEFT_Y);
        puropo_data.state.right_x = -puropo.get(PUROPO_RIGHT_X);
        puropo_data.state.right_y = puropo.get(PUROPO_RIGHT_Y);
        for(int i=5;i<=10;i++){
            puropo_data.state.axis[i-1] = puropo.get(i);
        }
    }

    /**
     * @brief デバッグ関数
     */
    void print_debug()
    {
        printf("puropo_data{");
        printf("state{");
        printf("communicatable: %d");
        printf("|");
        printf("LX: %0.2f",puropo_data.state.left_x);
        printf("|");
        printf("LY: %0.2f",puropo_data.state.left_y);
        printf("|");
        printf("RX: %0.2f",puropo_data.state.right_x);
        printf("|");
        printf("RY: %0.2f",puropo_data.state.right_y);
        printf("|");
        printf("axis:");
        printf("[");
        for(int i=0;i<6;i++){
            printf("%0.2f,", puropo_data.state.axis[i]);
        }
        printf("]");

        printf("}");
        printf("}");
        printf("|");
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
    }puropo_data;
};

#endif // NHK2025B_PUROPO_H