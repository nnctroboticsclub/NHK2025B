#include "rohm_md.h"

RohmMD::RohmMD(ikarashiCAN_mk2 *_can, int _id)
{
    // canのアドレスとmdのidを保持
    can = _can;
    id = _id;
    now_id = can->get_this_id();
}

RohmMD::RohmMD(){}

void RohmMD::set(uint8_t mode, float ref)
{
    // モードと指令値を送信配列に格納
    ref_data.mode = mode;
    // refを一回ごとに1bit変更
    // -1 < ref < 1にする
    ref = (ref < 1)?ref:0.99999;
    ref = (ref > -1)?ref:-0.99999;
    ref_data.ref = ref * pow(2, 15);
}

bool RohmMD::send()
{
    // 指定したidにデータを送信
    can->set(ref_data.array, sizeof(ref_data.array));
    can->set_this_id(id);
    bool return_value = can->write(0);
    // can->set_this_id(now_id);
    return return_value;
}

bool RohmMD::read()
{
    // 受信したcanデータフレームが指定したidならデータを受け取る
    bool return_value = false;
    if (can->get_id() == id)
    {
        can->get(res_data.array, sizeof(res_data.array));
        return_value = true;
    }
    return return_value;
}

float RohmMD::get_vel()
{
    return (float)res_data.velocity / 100;
}

float RohmMD::get_cur()
{
    return (float)res_data.current / 100;
}

float RohmMD::get_rad()
{
    return (float)res_data.radian / 100;
}

void RohmMD::print_debug()
{
    printf("ref:");
    for(int i=0; i<(int)sizeof(ref_data.array); i++)
        printf("%d ", ref_data.array[i]);
    printf("|res:");
    for(int i=0; i<(int)sizeof(res_data.array); i++)
        printf("%d ", res_data.array[i]);
}