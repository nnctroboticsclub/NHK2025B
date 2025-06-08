#ifndef IKAKO_ROBOMAS_LIB
#define IKAKO_ROBOMAS_LIB

#include <mbed.h>
#include <ikarashiCAN_mk2.h>
#include <MotorController.h>
#include <vector>

/**
 * @brief モータのパラメータ、データをまとめる構造体
 *
 */
struct ikako_robomas_motor_config
{
    int id;
    int read_flag;

    union motor_current
    {
        int16_t current;
        uint8_t data_array[2];
    };
    motor_current mc;

    union data_frame_feed_back
    {
        struct
        {
            int16_t angle;
            int16_t rpm;
            int16_t current;
            int8_t temperature;
        } __attribute__((packed));
        uint8_t data_array[8];
    } __attribute__((packed));
    data_frame_feed_back df_fb;

    float current_limit = 0;
    float convert_current_ref_ratio = 0;
    float convert_current_to_float = 0;
    float convert_position_to_float = 0;
    float convert_velocity_to_float = 0;
};

/**
 * @brief モータの基底クラス。メインの処理をここに書いて、各クラスで定数の設定とかする
 *
 */
class IkakoMotor
{
public:
    /**
     * @brief パラメータを設定する関数
     * @note この関数の中でMotorParams（L、R、Dとか）の設定もしている。D、Jの値は適当なのでいい感じに設定することを推奨
     *
     * @param _id m3508のid 1=8
     */
    virtual void set_params(int _id);

    /**
     * @brief スピード（電流値 [A]）をセット
     *
     * @param current -20.0 ~  20.0
     */
    void set_ref(float current);

    /**
     * @brief 電流のフィードバック値の取得
     *
     * @return float
     */
    float get_current();

    /**
     * @brief 角度のフィードバック値の取得
     *
     * @return float
     */
    float get_angle();

    /**
     * @brief 速度[rad/s]の取得
     *
     * @param gear_ratio モータのギア比
     * @return float
     */
    float get_vel(float gear_ratio = 0);

    /**
     * @brief トルク[N/A]の取得
     *
     * @return float
     */
    float get_torque();

    /**
     * @brief モータの温度の取得
     *
     * @return float
     */
    float get_temperature();

    /**
     * @brief データの受信の確認
     *
     * @return int
     */
    int get_read_flag();

    /**
     * @brief ロボマスモータ用のパラメータ構造体のポインタを返す関数
     *
     * @return ikako_robomas_motor_config*
     */
    ikako_robomas_motor_config *get_motor();

    /**
     * @brief MotorController用のパラメータ構造体のポインタを返す関数
     *
     * @return MotorParams*
     */
    MotorParams *get_motor_params();

protected:
    ikako_robomas_motor_config config;
    MotorParams params;
    void swap_two_elements(uint8_t *array);
};

/**
 * @brief モータのデータをデータフレームにまとめてcanバスに送信するクラス
 *
 */
class IkakoRobomasSender
{
public:
    /**
     * @brief コンストラクタ
     *
     * @param _ican ikarashiCANのインスタンスのアドレス
     */
    IkakoRobomasSender(ikarashiCAN_mk2 *_ican);

    /**
     * @brief canデータの送信
     *
     * @return int
     */
    int write();

    /**
     * @brief canデータの受信。ここで受信してからそれぞれのモータにデータがパースされる
     * @note それぞれのモータのフィードバックの取得はモータのクラスから行う
     *
     */
    void read();

    /**
     * @author 高野 絆
     * @brief canを変更する
     */
    void setIcan(ikarashiCAN_mk2 *ican_){
        ican = ican_;
        memset(&df_0x200,0,sizeof(data_frame_0x200));
        memset(&df_0x1FF,0,sizeof(data_frame_0x1FF));
    }
    
    /**
     * @brief モータ全てのパラメータ構造体のポインタをセットする。制御したいモータの数だけ実行する必要がある。
     *
     * @param _motor
     */
    void set_motors(ikako_robomas_motor_config *_motor);

    /**
     * @brief printデバッグ用関数
     *
     */
    void print_debug();

    union data_frame_0x200
    {
        // ID1 ~ 4
        int16_t current[4];
        uint8_t data_array[8];
    } __attribute__((packed));

    union data_frame_0x1FF
    {
        // ID5 ~ 8
        int16_t current[4];
        uint8_t data_array[8];
    } __attribute__((packed));

    data_frame_0x200 df_0x200;
    data_frame_0x1FF df_0x1FF;

    std::vector<ikako_robomas_motor_config *> motor;

private:
    /**
     * @brief 配列の隣り合った要素二つの順番を逆にする
     *
     * @param array 順番を変えたい配列の先頭アドレス
     */
    void swap_two_elements(uint8_t *array);

    ikarashiCAN_mk2 *ican;
    int cnt;
};

#endif