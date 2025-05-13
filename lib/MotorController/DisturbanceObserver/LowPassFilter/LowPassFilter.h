/**
 * @file LowPassFilter.h
 * @author Ikarashi Kota (kotakota925ika@gmail.com)
 * @brief オカピさんのやつを参考にしたローパスフィルタライブラリ
 * @note とりあえず作ってみただけ
 * @version 0.1
 * @date 2024-03-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef IKAKO_LOWPASSFILTER
#define IKAKO_LOWPASSFILTER

// #include <Arduino.h>
#include <mbed.h>

class LPF
{
public:
    /**
     * @brief コンストラクタ
     * 
     * @param _Ts 制御周期 [s]
     * @param _omega カットオフ周波数 [rad/s]
     */
    LPF(float _Ts, float _omega);
    LPF();

    /**
     * @brief コンストラクタと同じ。この関数をコンストラクタ内で呼び出してる
     * 
     * @param _Ts 
     * @param _omega 
     */
    void set_params(float _Ts, float _omega);

    /**
     * @brief LPFをオンにする
     */
    void enable();

    /**
     * @brief LPFをオフにする
     */
    void disable();

    /**
     * @brief メインの処理
     */
    void update();

    /**
     * @brief LPFがオンかオフかを取得する
     * 
     * @return true 
     * @return false 
     */
    bool is_enabled();

    /**
     * @brief LPFの出力を取得
     * 
     * @return float 
     */
    float get_output();

    /**
     * @brief フィードバック値をセット
     * 
     * @param res 
     */
    void set_response(float res);
// protected:
    float K[2] = {0.0 ,0.0};
    float Ts = 0.0;
    float omega = 0.0;
    bool enabled = false;
    float response[2] = {0.0, 0.0};
    float output[2] = {0.0, 0.0};
};

#endif