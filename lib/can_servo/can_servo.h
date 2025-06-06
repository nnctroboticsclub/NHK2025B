#include <mbed.h>
#include "ikarashiCAN_mk2.h"

#ifndef CAN_SERVO
#define CAN_SERVO

class can_servo
{
public:

    /**
     * @brief コンストラクタ
     * 
     * @param _ican ikarashiCAN_mk2のインスタンスのアドレス
     * @param _id can_servoのID
     */
    can_servo(ikarashiCAN_mk2 *_ican, int _id);
    can_servo();

    /**
     * @brief サーボの値をセットする関数
     * 
     * @param num サーボモータの番号（0〜7）
     * @param param サーボの出力の割合 0~255
     */
    void set(int num, uint8_t param);

    /**
     * @brief 送信関数
     * 
     * @return int 1:送信済み 0:送信不可
     */
    int send();

    uint8_t get(int num);

private:

    ikarashiCAN_mk2 *ican;
    int cnt;
    int id;

    union can_servo_data_format_1
    {
        struct
        {
            uint8_t servo[8];
        } __attribute__((packed));
        uint8_t data_array[8];
    } __attribute__((packed));

    can_servo_data_format_1 csdf;
    
};

#endif