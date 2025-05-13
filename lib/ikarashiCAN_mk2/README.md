# いかこうのCANライブラリmk2
新しいCANライブラリです。

mbedのCANライブラリが使いにくすぎて、かなり苦労しました。

そのうちesa書きます。

# サンプルコード
```Cpp
#include <mbed.h>
#include <ikarashiCAN_mk2.h>
#include <button_state.h>

ikarashiCAN_mk2 can1(D15, D14, 0);
ikarashiCAN_mk2 can2(PB_5, PB_6, 1);
DigitalOut led1(LED1);
DigitalIn button(USER_BUTTON);
Button_state btn_state;
Thread thread1;
Ticker ticker;

uint8_t counter = 0;
uint8_t data_array[8] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
unsigned int target_id = 0;
int counter_ms = 0;
uint8_t btn_in = 0;

//送信はスレッドで1ms間隔ぐらいでやるといい
void send_function()
{
    while (1)
    {
        //送信
        if (can1.write(0))
        {
            counter++;
        }
        else
        {
        }
        ThisThread::sleep_for(1ms);
    }
}

void main_update();

int main()
{
    btn_state.set(&btn_in);
    led1 = 0;
    printf("main()\n");
    thread1.start(&send_function);

    //受信は絶対に割り込み
    can1.read_start();
    can2.read_start();

    //id0~7まで受信
    can1.filter(1, 0, 0xfffffff8, 0);

    //id2のみ受信
    // can1.filter(0, 2, 0, 0);

    //id4のみ受信
    // can1.filter(0, 4, 0, 0);

    //id8のみ受信
    // can1.filter(0, 8, 0, 0);

    while (1)
    {
        main_update();
        printf("\nsf:%d  rf:%d  rmsg:%d  cnt:%d  id:%d  tar:%d  this:%d", can1.get_send_flag(), can1.get_read_flag(), can1.get_byte(0), counter, can1.get_id(), can1.get_target_id(), can1.get_this_id());
        printf("  rf2:%d  rmsg2:%d  tar:%d", can2.get_read_flag(), can2.get_byte(0), can2.get_target_id());
    }
}

void main_update()
{
    btn_in = button.read();
    led1 = can1.get_read_flag();

    //適当なデータの生成
    for (int i = 0; i < sizeof(data_array); i++)
        data_array[i] = counter * i;
    data_array[0] = counter;

    //データのセット
    can1.set(data_array, sizeof(data_array));

    //ボタンを押すたびにIDのカウントアップ
    if (btn_state.state() == 1)
    {
        target_id++;
        can1.set_this_id(target_id % 10 + 1);
    }
}
```
