#pragma once
#include "N_robomas.h"

/**
 * @brief ステア角に基づいて左右のモータ回転数を差動制御するクラス
 */
class DifferentialController
{
public:
    /**
     * @brief 初期化処理（将来拡張用）
     */
    void Setup();

    /**
     * @brief 差動補正係数を設定する
     * @param k 補正係数
     */
    void SetCorrection(float k);

    /**
     * @brief ステア角と速度に基づいてロボマスへ出力する
     * @param angle ステア角（ラジアン）
     * @param velocity 指令速度（-100〜100）
     * @param robomas ロボマス制御オブジェクト
     */
    void Update(float angle, float velocity, NHK2025B_Robomas& robomas);

private:
    float correction_k_ = 1.0f;

    float Clamp(float value, float min_val, float max_val);
    float CalculateRatio(float angle);
    float CalculateLeftCurrent(float velocity, float ratio);
    float CalculateRightCurrent(float left_current, float ratio);
    void ApplyCurrentToRobomas(float left_current, float right_current, NHK2025B_Robomas& robomas);
};
