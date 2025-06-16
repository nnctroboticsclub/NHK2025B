#ifndef ROHM_MD_H
#define ROHM_MD_H

#include <mbed.h>
#include <ikarashiCAN_mk2.h>

class RohmMD
{
public:
    /**
     * @brief コンストラクタ
     *
     * @param _can ikarashiCAN_mk2のインスタンスのアドレス
     * @param id md側のcanのid
     */
    RohmMD(ikarashiCAN_mk2 *_can, int _id);
    RohmMD();

    /**
     * @brief 値のセット
     *
     * @param mode モード
     * @param ref 値
     */
    void set(uint8_t mode, float ref);

    /**
     * @brief データの送信
     *
     * @return true 送信成功
     * @return false 送信失敗
     */
    bool send();

    /**
     * @brief 受信したデータのパース
     *
     * @return true
     * @return false
     */
    bool read();

    /**
     * @brief エンコーダの角速度の取得
     * 
     * @return float [rad/s]
     */
    float get_vel();

    /**
     * @brief モータに流れている電流値を取得
     * 
     * @return float [A]
     */
    float get_cur();

    /**
     * @brief エンコーダの角度を取得
     * 
     * @return float [rad]
     */
    float get_rad();

    /**
     * @brief デバッグ用の関数
     * 
     */
    void print_debug();

private:
    ikarashiCAN_mk2 *can;
    int id;
    int now_id;

    union reference_data_protocol
    {
        struct
        {
            uint8_t mode;
            int16_t ref; //*2^15
        } __attribute__((packed));
        uint8_t array[3];
    } __attribute__((packed));
    reference_data_protocol ref_data;

    union responce_data_protocol
    {
        struct
        {
            int16_t velocity; //*100 [rad/s]
            int16_t current;  //*100 [A]
            int16_t radian;   //*100 [rad]
        } __attribute__((packed));
        uint8_t array[6];
    } __attribute__((packed));
    responce_data_protocol res_data;
};

#endif