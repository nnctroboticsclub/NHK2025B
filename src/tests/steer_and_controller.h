#include "mbed.h"
#include "N_servo.h"
#include "N_steer.h"
#include "N_controller.h"
#include "N_puropo.h"
#include "definitions.h"

// サーボパラメータ設定
std::array<ServoParameter, NUM_OF_SERVO> servo_param{{
    []{ ServoParameter p; p.id = 1; return p; }(),
    []{ ServoParameter p; p.id = 2; return p; }(),
}};

NHK2025B_Servo servo(servo_param);
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
        ThisThread::sleep_for(1ms);
    }
}

int cnt_1ms = 0;
void update_1ms()
{
    puropo.update_ts();
    cnt_1ms++;
}

// デバッグ出力関数
void print_debug()
{
    controller.print_debug();
    // puropo.print_debug();
    // printf("front: %f(%d), back: %f(%d), velocity: %f\n",
    //        steer.getFrontAngle(), rad2deg(steer.getFrontAngle()),
    //        steer.getBackAngle(), rad2deg(steer.getBackAngle()),
    //        steer.getVelocity());
    // printf("is_ok:%d,",puropo.getCommunicatable(0));
    
    can1.print_debug();
}

int main()
{
    servo.setup();
    puropo.setup();

    steer.setup();
    controller.setup();
    ES = 1;

    thread.start(&send_thread);
    ticker.attach(&update_1ms, 1ms);

    while (true)
    {
        if (cnt_1ms > 100)
        {
            cnt_1ms = 0;
            print_debug();
            puts("");
        }

        controller.setSteerDirection(puropo.getLeftX(0));
        controller.setSteerVelocity(puropo.getLeftY(0));
        controller.setSteerTurn(puropo.getRightX(0));

        steer.setTurn(controller.getSteerTurn());
        steer.setDirection(controller.getSteerDirection());
        steer.setVelocity(controller.getSteerVelocity());

        servo.setAngle(0, steer.getFrontAngle());
        servo.setAngle(1, steer.getBackAngle());

        controller.update();
        puropo.update();
        servo.update();
    }
}