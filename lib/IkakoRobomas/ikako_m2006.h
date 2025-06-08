#include <mbed.h>

#ifndef IKAKO_M2006_LIB
#define IKAKO_M2006_LIB

#include <ikarashiCAN_mk2.h>
#include <algorithm>
#include "ikako_robomas.h"

class IkakoM2006 : public IkakoMotor
{
public:
    /**
     * @brief コンストラクタ
     * @note set_paramsと全く同じ
     *
     * @param _id
     */
    IkakoM2006(int _id);
    IkakoM2006();

    /**
     * @brief パラメータを設定する関数
     * @note この関数の中でMotorParams（L、R、Dとか）の設定もしている。D、Jの値は適当なのでいい感じに設定することを推奨
     *
     * @param _id m2006のid 1=8
     */
    void set_params(int _id);

    const float kt = 0.18;             // トルク定数
    const int p_pairs = 7;             // 極数
    const float L = 64.22e-6;          // 相インダクタンス
    const float R = 0.461;             // 抵抗 [ohm]
    const float gear_ratio = 36.0 / 1; // デフォルトのギアボックスの比

private:
    const int POSITION_INT_RES_MAX = 8191;
    const float CURRENT_LIMIT = 10.0;
    const float CONVERT_CURRENT_REF_RATIO = 10000.0 / CURRENT_LIMIT;

    const float CONVERT_CURRENT_TO_FLOAT = 1.0 / CONVERT_CURRENT_REF_RATIO;
    const float CONVERT_VELOCITY_TO_FLOAT = 2.0 * M_PI / 60.0f;
    const float CONVERT_POSITION_TO_FLOAT = 2.0 * M_PI / (float)POSITION_INT_RES_MAX;
};

#endif