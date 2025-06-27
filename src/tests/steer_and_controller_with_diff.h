#include "mbed.h"
#include "N_servo.h"
#include "N_steer.h"
#include "N_controller.h"
#include "N_puropo.h"
#include "N_robomas.h"
#include "definitions.h"

#define torqueGain 0.1f // ここの値いい感じにする

// サーボパラメータ設定
std::array<ServoParameter, NUM_OF_SERVO> servo_param{{
    []
    { ServoParameter p; p.id = 1; return p; }(),
    []
    { ServoParameter p; p.id = 2; return p; }(),
}};

// ロボマスパラメータ設定
std::array<RobomasParameter, NUM_OF_ROBOMAS> robomas_param{{
    []
    { RobomasParameter p; p.robomas_id = 1; return p; }(),
    []
    { RobomasParameter p; p.robomas_id = 2; return p; }(),
    []
    { RobomasParameter p; p.robomas_id = 3; return p; }(),
    []
    { RobomasParameter p; p.robomas_id = 4; return p; }(),
}};

NHK2025B_Servo servo(servo_param);
NHK2025B_Robomas robomas(robomas_param);
NHK2025B_Steer steer;
NHK2025B_Controller controller;
NHK2025B_Puropo puropo;

Thread thread;
Ticker ticker;

void send_thread()
{
    while (true)
    {
        servo.write();
        robomas.write();
        ThisThread::sleep_for(1ms);
    }
}

int cnt_1ms = 0;
void update_1ms()
{
    // 今後使ってくならこのままでもいいけどpuropo.update_ts()のみならmain()の中で呼び出してもいい
    // robomas.update_ts();
    puropo.update_ts();
    cnt_1ms++;
}

void update()
{
    controller.update();
    puropo.update();
    servo.update();
    robomas.update();
    can1.update();
    can2.update();
}

// デバッグ出力関数
void print_debug()
{
    // printf("is_ok:%d,\n", puropo.getCommunicatable(0));

    // steer.print_debug();
    // can1.print_debug();
    // can2.print_debug();

    float torque_diff_val = torqueFeedback();
    printf("torque_diff: %f\ttorque stronger:%s\n",
           abs(torque_diff_val),
           (torque_diff_val > 0.1f) ? "left" : "right");
}

float torqueFeedback()
{
    float torque0 = robomas.getTorque(0);
    float torque1 = robomas.getTorque(1);
    return torque0 - torque1;
}

int main()
{
    servo.setup();
    puropo.setup();
    robomas.setup();

    steer.setup();
    controller.setup();
    ES = 1;

    thread.start(&send_thread);
    ticker.attach(&update_1ms, 1ms);

    float torque_diff = 0.0f;

    while (true)
    {
        if (cnt_1ms > 100)
        {
            cnt_1ms = 0;
            print_debug();
            puts("");
        }

        led[1] = puropo.getCommunicatable(0);
        ES = puropo.getCommunicatable(0);

        servo.setAngle(1, M_2_PI + puropo.getLeftX(0) * M_PI / 180 * 25);
        servo.setAngle(0, M_2_PI - puropo.getLeftX(0) * M_PI / 180 * 25);

        torque_diff = torqueFeedback();

        robomas.setCurrent(0, puropo.getLeftY(0) * 5.0 + torque_diff * torqueGain);
        robomas.setCurrent(1, (puropo.getLeftY(0) * 5.0 + torque_diff * torqueGain) * -1.0);
        update();
        ThisThread::sleep_for(1ms); // ちょっと待ってあげたほうがいいかも
    }
}

/*
旋回のテスト用コード
while (true)
{
    if (cnt_1ms > 100)
    {
        cnt_1ms = 0;
        print_debug();
        puts("");
    }

    led[1] = puropo.getCommunicatable(0);
    ES = puropo.getCommunicatable(0);

    // servo.setAngle(1, M_2_PI + puropo.getLeftX(0) * M_PI / 180 * 25);
    // servo.setAngle(0, M_2_PI - puropo.getLeftX(0) * M_PI / 180 * 25);

    robomas.setCurrent(0, puropo.getLeftY(0) * 5.0);
    robomas.setCurrent(1, (puropo.getLeftY(0) * 5.0));
    update();
    ThisThread::sleep_for(1ms); // ちょっと待ってあげたほうがいいかも
}
*/