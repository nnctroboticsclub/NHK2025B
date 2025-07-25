nishiwakiMDCを動かすライブラリです。まだdoxygen書いてないので、サンプルコードを元に使ってください。

そのうちesa書きます。

# 更新(2023/9/9)
PIDライブラリとlpfライブラリを結合しました。最強のMDCライブラリになりました

# サンプルコード
```C
#include "ikakoMDC.h"
#include "ikarashiCAN_mk2.h"

#define PI 3.14159265359

Thread sender; //スレッド（CANの送信用）のインスタンス
Ticker counter; //カウント用
DigitalOut es(PB_1); //非常停止
ikarashiCAN_mk2 can1(PB_8, PB_9, 0); //CAN1のインスタンス
ikarashiCAN_mk2 can2(PB_5, PB_6, 0); //CAN2のインスタンス（このコードでは使っていない。つまり飾り）

//モータ一つ一つのインスタンス
ikakoMDC motor[]{
    //モータの番号（1~4）、最小スピード、最大スピード、制御周期[s]、目標値までの時間[s]、Pゲイン、Iゲイン、Dゲイン、インターバル
    ikakoMDC(1, -50, 50, 0.001, 0.0, 2.7, 0, 0.000015, 0.01),
    ikakoMDC(2, -50, 50, 0.001, 0.0, 2.7, 0, 0.000015, 0.01),
    ikakoMDC(3, -50, 50, 0.001, 0.0, 2.7, 0, 0.000015, 0.01),
    ikakoMDC(4, -50, 50, 0.001, 0.0, 2.7, 0, 0.000015, 0.01),
};
ikakoMDC motor2[]{
    ikakoMDC(5, -50, 50, 0.001, 0.0, 2.7, 0, 0.000015, 0.01),
    ikakoMDC(6, -50, 50, 0.001, 0.0, 2.7, 0, 0.000015, 0.01),
    ikakoMDC(7, -50, 50, 0.001, 0.0, 2.7, 0, 0.000015, 0.01),
    ikakoMDC(8, -50, 50, 0.001, 0.0, 2.7, 0, 0.000015, 0.01),
};

//モータ駆動信号を送受信するクラスのインスタンス
ikakoMDC_sender mdc[]{
    //ikakoMDCのインスタンスのアドレス、制御したいモータの数（1~4）、使うCANのインスタンスのアドレス、MDCのID
    ikakoMDC_sender(motor, 4, &can1, 1),
    ikakoMDC_sender(motor2, 4, &can1, 2),
};

//スレッド用関数。主にCANの送信用。1ms間隔で送信するといい感じ
void mdc_sender()
{
    while(1)
    {
        mdc[0].send();
        mdc[1].send();
        ThisThread::sleep_for(1ms);
    }
}

//Ticker用関数。1ms間隔でcntが+1されるため、1msごとのカウントを簡単に実装できる。
int cnt = 0;
void count_1ms()
{
    cnt++;
}

int main()
{
    int motor_speed[4] = {0, 0, 0, 0};
    int encoder_data[4] = {0, 0, 0, 0};

    //CANの受信割り込みのスタート
    can1.read_start();
    can2.read_start();

    counter.attach(&count_1ms, 1ms); //カウントアップ関数の割り込みスタート
    sender.start(&mdc_sender); //CAN送信用スレッドのスタート
    es = 1;
    while (1)
    {
        for(int i=0; i<(int)(sizeof(motor_speed) / sizeof(motor_speed[0])); i++)
        {
            motor_speed[i] = (int)(sin((float)cnt * 90 / 1000 * PI / 180) * 100);

            //モータのスピードはset_speed関数で操作する。ここにセットされた値がスレッド内の関数によって送信され、モータが回る。
            motor[i].set_speed(motor_speed[i]);
            motor2[i].set_speed(motor_speed[i]);
        }

        //MDCについてるロータリーエンコーダの値も取得できるよ!!
        if(mdc[0].read_enc() && can1.get_read_flag())
        {
            for(int i=0; i<(int)(sizeof(encoder_data) / sizeof(encoder_data[0])); i++)
            {
                encoder_data[i] = motor[i].get_enc();
            }
        }
        // printf("%d ", motor_speed[0]);
        printf("speed:%d %d %d %d  ", motor[0].get_speed(), motor[1].get_speed(), motor[2].get_speed(), motor[3].get_speed());
        printf("encoder:%d %d %d %d  ", encoder_data[0], encoder_data[1], encoder_data[2], encoder_data[3]);
        printf(" r:%d s:%d\n", can1.get_read_flag(), can1.get_send_flag());
    }
}
```