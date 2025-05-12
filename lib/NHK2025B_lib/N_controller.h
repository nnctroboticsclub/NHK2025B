#include <mbed.h>
#include "N_puropo.h"

#ifndef NHK2025B_CONTROLLER_H
#define NHK2025B_CONTROLLER_H
class NHK2025B_Controller{
public:
    /**
     * @brief コンストラクタ
     */
    NHK2025B_Controller(){}

    /**
     * @brief セットアップ
     */
    void setup();

    /**
     * @brief ステアのスピードをセットする
     */
    void setSteerSpeed(float axe){
        ;
    }

    /**
     * @brief ステアのスピードを取得する
     */
    float getSteerSpeed(){
        return cmd.steer.speed;
    }

    /**
     * @brief Ts(1ms)間隔で処理する関数
     */
    void update_1ms();

    /**
     * @brief whileループで回す
     */
    void update();
private:
    struct{
        struct{
            float speed; // turn == 0: 移動速度, turn != 1: 旋回速度
            float direct; // 移動方向
            int turn; // -1:右回転, 0:無回転, 1:左回転
        }steer;
    }cmd;
};

#endif NHK2025B_CONTROLLER_H