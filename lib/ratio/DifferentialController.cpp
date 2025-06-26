#include "DifferentialController.h"

void DifferentialController::Setup()
{
    //なに書けばいいのか
}

void DifferentialController::SetCorrection(float k)
{
    correction_k_ = k;
}

void DifferentialController::Update(float angle, float velocity, NHK2025B_Robomas& robomas)
{
    float ratio = CalculateRatio(angle);
    float left_current = CalculateLeftCurrent(velocity, ratio);
    float right_current = CalculateRightCurrent(left_current, ratio);
    ApplyCurrentToRobomas(left_current, right_current, robomas);
}

float DifferentialController::Clamp(float value, float min_val, float max_val)
{
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

float DifferentialController::CalculateRatio(float angle)
{
    float ratio = 1.0f + angle * correction_k_;
    return Clamp(ratio, 0.5f, 2.0f);  // right / left
}

float DifferentialController::CalculateLeftCurrent(float velocity, float ratio)
{
    return velocity * 2.0f / (1.0f + ratio);
}

float DifferentialController::CalculateRightCurrent(float left_current, float ratio)
{
    return left_current * ratio;
}

void DifferentialController::ApplyCurrentToRobomas(float left_current, float right_current, NHK2025B_Robomas& robomas)
{
    robomas.setCurrent(0,  left_current);
    robomas.setCurrent(1, -left_current);
    robomas.setCurrent(2,  right_current);
    robomas.setCurrent(3, -right_current);
}
