/**
 * @file PID.h
 * @author Ikarashi Kota (kotakota925ika@gmail.com)
 * @brief 外乱オブザーバーの実装をしてたらPIDも自作したくなって作った
 * @note こんとろらぼさんの記事を参考に書いた(https://controlabo.com/pid-program/)
 * @version 0.1
 * @date 2024-03-17
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef IKAKO_PID
#define IKAKO_PID

#include <mbed.h>

class Ikako_PID
{
public:

    /**
     * @brief PIDのゲインを格納する構造体
     */
    struct PID_Params
    {
        float Kp = 0.0;
        float Ki = 0.0;
        float Kd = 0.0;
    };
    PID_Params param;

    /**
     * @brief 指令値、誤差、出力を格納する構造体
     * 
     * @param ref 指令値
     * @param error 誤差
     * @param out PIDの出力(計算結果)
     */
    struct PID_state
    {
        float ref = 0.0;
        float res = 0.0;
        float error = 0.0;
        float out = 0.0;
    };
    PID_state state;

    /**
     * @brief コンストラクタ
     * 
     * @param _Ts 制御周期 [s]
     */
    Ikako_PID(float _Ts);
    Ikako_PID();

    /**
     * @brief 誤差の初期値と積分の初期値を設定する関数
     * @note なくてもいいが、これをちゃんと設定すると精度が上がるらしい
     * 
     * @param _e 誤差の初期値
     * @param _ie 積分の初期値
     */
    void set_first_value(float _e = 0.0, float _ie = 0.0);

    /**
     * @brief 出力値の範囲を設定する関数
     * 
     * @param _min 最小値
     * @param _max 最大値
     */
    void set_output_limits(float _min, float _max);

    /**
     * @brief 指令値の入力
     * 
     * @param ref 
     */
    void set_reference(float ref);

    /**
     * @brief フィードバック値の入力
     * 
     * @param res 
     */
    void set_response(float res);

    /**
     * @brief メインの処理。Ts [s]間隔で実行すること
     */
    void update();

    /**
     * @brief PIDの出力を取得
     * 
     * @return float 
     */
    float get_output();

    /**
     * @brief PIDを有効化する
     */
    void enable();

    /**
     * @brief PIDを無効化する
     */
    void disable();

    /**
     * @brief PIDが有効か無効か取得する関数
     * 
     * @return true 
     * @return false 
     */
    bool is_enabled();

protected:
    float Ts = 0.0;
    float reference = 0.0;
    float min = 0.0;
    float max = 0.0;
    float e_pre = 0.0;
    float de = 0.0;
    float ie = 0.0;
    bool enabled = false;
};

#endif