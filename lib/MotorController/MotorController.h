/**
 * @file MotorController.h
 * @author Ikarashi Kota (kotakota925ika@gmail.com)
 * @brief オカピさんのを参考にしたモータ制御用ライブラリ。自分なりに汎用性を持たせたつもり
 * @note とりあえず作ってみただけ
 * @version 0.1
 * @date 2024-03-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef IKAKO_MOTORCONTROLLER
#define IKAKO_MOTORCONTROLLER

// #include <Arduino.h>
#include <mbed.h>
#include "DisturbanceObserver/DOB.h"
#include "Ikako_PID/ikako_PID.h"

/**
 * @brief モータの制御方式の種類
 */
enum ControlType
{
    CURRENT,
    VELOCITY,
    POSITION,
};
// CONTROL_TYPE ControlType;

#ifndef MOTOR_PARAMA_STRUCT
#define MOTOR_PARAMA_STRUCT 
/**
 * @brief モータの固有のパラメータを格納する構造体
 */
struct MotorParams
{
    float L = 0.0;
    float R = 0.0;
    float D = 0.0;
    float J = 0.0;
    float Kt = 0.0;
    int p_pairs = 0;
    float gear_ratio = 1.0;
};
#endif

class MotorController
{
public:
    /**
     * @brief 制御方式ごとに指令値とフィードバックを格納する構造体
     */
    struct MotorState
    {
        float ref = 0.0;
        float res = 0.0;
    };

    MotorState current;
    MotorState velocity;
    MotorState position;

    /**
     * @brief コンストラクタ
     *
     * @param _type 制御方式
     * @param _params 設定したMotorParamの構造体
     * @param _Ts 制御周期 [s]
     * @param omega_v 外乱オブザーバーのローパスフィルタのカットオフ周波数（多分。。。）
     */
    MotorController(ControlType _type, MotorParams *_params, float _Ts, float omega_v);

    /**
     * @brief 制御のスタート。最初に一回だけ実行する
     */
    void start();

    /**
     * @brief メインの処理を行う関数
     * @note コンストラクタで設定した制御周期 [s]ごとに実行する
     */
    void update();

    /**
     * @brief 出力の取得
     *
     * @return float
     */
    float get_output();

    /**
     * @brief 現在の制御方式ごとにMotorStateの構造体のアドレスを返す関数
     *
     * @return MotorState*
     */
    MotorState *get_state();

    /**
     * @brief 指令値を設定する関数
     *
     * @param value
     */
    void set_reference(float value);

    /**
     * @brief フィードバック値を設定する関数
     *
     * @param value
     */
    void set_response(float value);

    /**
     * @brief 電流の最大値、最小値を設定する関数
     *
     * @param _min 最小値 [A]
     * @param _max 最大値 [A]
     */
    void set_limit(float _min, float _max);

    /**
     * @brief PIDゲイン設定
     * 
     * @param kp 
     * @param ki 
     * @param kd 
     */
    void set_pid_gain(float kp, float ki, float kd);

    /**
     * @brief 有効化
     */
    void enable();

    /**
     * @brief 無効化
     */
    void disable();

    /**
     * @brief 有効か無効かを取得
     *
     * @return true
     * @return false
     */
    bool is_enabled();

    DOB dob;
    Ikako_PID pid;

protected:
    ControlType type;
    MotorParams *params;
    float min = 0.0;
    float max = 0.0;
    float Ts = 0.0;
    bool enabled = false;
    float output = 0.0;
};

#endif