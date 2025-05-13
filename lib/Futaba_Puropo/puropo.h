#ifndef FUTABA_PUROPO_H
#define FUTABA_PUROPO_H

#include <mbed.h>

class Puropo
{
public:
    /**
     * @brief コンストラクタ
     *
     * @param tx uartのtxピン
     * @param rx uartのrxピン
     * @param _timeout タイムアウトする時間[ms]
     */
    Puropo(PinName tx, PinName rx, int _timeout = 1000);

    /**
     * @brief 受信割り込みスタート
     *
     */
    void start();

    /**
     * @brief 受信が完了したかの処理
     *
     * @return true 受信完了
     * @return false 受信失敗
     */
    bool is_ok();

    /**
     * @brief データを取得
     * 
     * @param ch チャンネルの指定
     * @return float -1.0 ~ 1.0
     */
    float get(int ch);

    /**
     * @brief デバッグ用のprintf関数
     *
     */
    void debug()
    {
        printf("\n");
        // printf("len:%d, ", length);
        printf("check:%d, ", this->is_ok());
        printf("data:");
        for (int i = 0; i < (int)sizeof(protocol.data_array); i++)
            printf("%d,", protocol.data_array[i]);
    }

private:
    /**
     * @brief 受信割り込み用関数
     *
     */
    void read_attach();

    /**
     * @brief 指定したチャンネルの入力を取得する関数
     *
     * @param ch チャンネルの指定
     * @return int 指定したチャンネルの入力値
     */
    int get_ch(int ch);

    /**
     * @brief get_chに閾値を適用したバージョン
     *
     * @param ch チャンネルの指定
     * @param threshold 閾値
     * @return int
     */
    int get_threshold(int ch, int threshold = 50);

    /**
     * @brief get関数をNormalization（正規化）したやつ
     *
     * @param ch チャンネルの指定
     * @param threshold 閾値
     * @return float -1 ~ 1
     */
    float get_normal(int ch, int threshold = 50);

    UnbufferedSerial serial;
    uint8_t length = 0;
    bool enable = false;

    float data[16] = {0.0}; //処理した後のデータを格納

    Timer timer;
    int time_buff;
    int timeout;

    // sbusのデータフレーム
    union s_bus_data_frame
    {
        struct
        {
            uint8_t start;
            uint8_t main_data[22];
            uint8_t sub_data;
            uint8_t end;
        } __attribute__((packed));
        uint8_t data_array[25];
    } __attribute__((packed));
    s_bus_data_frame protocol;

    // メインデータのパース
    union s_bus_channels
    {
        struct
        {
            int16_t channel1 : 11; // r:横 左-368 ~ -1007 ~ 368右
            int16_t channel2 : 11; // l:縦 上-434 ~ -1002 ~ 433下
            int16_t channel3 : 11; // r:縦 上-434 ~ 1012 ~ 433下
            int16_t channel4 : 11; // r:横 左-368 ~ 1017 ~ 368右
            int16_t channel5 : 11;
            int16_t channel6 : 11;
            int16_t channel7 : 11;
            int16_t channel8 : 11;
            int16_t channel9 : 11;
            int16_t channel10 : 11;
            int16_t channel11 : 11;
            int16_t channel12 : 11;
            int16_t channel13 : 11;
            int16_t channel14 : 11;
            int16_t channel15 : 11;
            int16_t channel16 : 11;
        } __attribute__((packed));
        uint8_t data_array[22];
    } __attribute__((packed));
    s_bus_channels channels;

    // サブデータのパース
    union s_bus_sub_data
    {
        struct
        {
            uint8_t channel17 : 1;
            uint8_t channel18 : 1;
            uint8_t frame_lost : 1;
            uint8_t fail_safe : 1;
            uint8_t none_bit : 4;
        } __attribute__((packed));
        uint8_t sub_data;
    } __attribute__((packed));
    s_bus_sub_data sub_data;
};

#endif