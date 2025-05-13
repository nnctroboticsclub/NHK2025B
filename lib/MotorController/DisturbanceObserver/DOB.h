/**
 * @file DOB.h
 * @author Ikarashi Kota (kotakota925ika@gmail.com)
 * @brief オカピさんのを参考に作った外乱オブザーバーライブラリ
 * @note とりあえず作ってみただけ
 * @version 0.1
 * @date 2024-03-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef IKAKO_DISTURBANCEOBSERVER
#define IKAKO_DISTURBANCEOBSERVER

// #include <Arduino.h>
#include <mbed.h>
#include "LowPassFilter/LowPassFilter.h"

class DOB
{
public:
    /**
     * @brief コンストラクタ
     *
     * @param _Ts 制御周期 [s]
     * @param _omega 外乱オブザーバーの帯域 [rad/s]
     * @param _Kin 入力側ゲイン
     * @param _Kout 出力側ゲイン
     */
    DOB(float _Ts, float _omega, float _Kin, float _Kout);
    DOB();

    /**
     * @brief コンストラクタと同じ。この関数をコンストラクタの中で呼び出してる
     *
     * @param _Ts
     * @param _omega
     * @param _Kin
     * @param _Kout
     */
    void set_params(float _Ts, float _omega, float _Kin, float _Kout);

    /**
     * @brief DOBをオンにする
     */
    void enable();

    /**
     * @brief DOBをオフにする
     */
    void disable();

    /**
     * @brief メインの処理。Ts [s]周期で実行する
     */
    void update();

    /**
     * @brief DOBがオンかオフか取得
     *
     * @return true
     * @return false
     */
    bool is_enabled();

    /**
     * @brief DOBの出力を取得
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

    /**
     * @brief 指令値をセット
     *
     * @param ref
     */
    void set_reference(float ref);

    LPF lpf;

protected:
    float Kin = 0.0;
    float Kout = 0.0;
    float Ts = 0.0;
    float omega = 0.0;
    bool enabled = false;
    float response = 0.0;
    float reference = 0.0;
    float output = 0.0;
};

#endif