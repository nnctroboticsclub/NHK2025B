//旧バージョン　うごかないよ
/*
#include "ikakoMDC.h"
#include "ikarashiCAN_mk2.h"

Thread sender;
Ticker counter;
ikarashiCAN_mk2 can1(PB_8, PB_9, 0);
ikakoMDC mdc[]{
    ikakoMDC(&can1, 1), // main
    ikakoMDC(&can1, 2), // stair
};

//送信用関数
void send_mdc()
{
    while (1)
    {
        mdc[0].write();
        ThisThread::sleep_for(1ms);
        mdc[1].write();
        ThisThread::sleep_for(1ms);
    }
}

//時間を測定する関数。1ms間隔
int cnt = 0;
void count_time()
{
    cnt++;
}

int main()
{
    int motor_speed[4] = {0, 0, 0, 0};
    counter.attach(count_time, 1ms);
    sender.start(send_mdc);

    while (1)
    {
        //mdc[1]のエンコーダ1~4の値を取得
        if (mdc[1].read_enc())
        {
            for(int i=0; i<4; i++){
                printf("enc%d:%d", i, mdc[1].get_enc(i));
            }
        }
        else
        {
        }

        //10ms間隔でcanをresetすると通信がいい感じになる
        if (cnt > 10)
        {
            can1.reset();
        }

        //mdc[0],mdc[1]のモータ1~4を10%の速度で正方向に駆動させる
        for (int i = 0; i < 4; i++)
        {
            motor_speed[i] = 10;
            mdc[0].set_speed(i, motor_speed[i]);
            mdc[1].set_speed(i, motor_speed[i]);
        }
    }
}
*/