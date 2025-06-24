#include "mbed.h"
#include "N_servo.h"
#include "N_steer.h"
#include "N_controller.h"
#include "N_puropo.h"
#include "N_robomas.h"
#include "definitions.h"

// サーボパラメータ設定
std::array<ServoParameter, NUM_OF_SERVO> servo_param{{
    []{ ServoParameter p; p.id = 1; return p; }(),
    []{ ServoParameter p; p.id = 2; return p; }(),
}};

// ロボマスパラメータ設定
std::array<RobomasParameter, NUM_OF_ROBOMAS> robomas_param{{
    []{ RobomasParameter p; p.robomas_id = 1; return p;}(),
    []{ RobomasParameter p; p.robomas_id = 2; return p;}(),
    []{ RobomasParameter p; p.robomas_id = 3; return p;}(),
    []{ RobomasParameter p; p.robomas_id = 4; return p;}(),
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
    robomas.update_ts();
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
    // controller.print_debug();
    // puropo.print_debug();
    // printf("front: %f(%d), back: %f(%d), velocity: %f\n",
    //        steer.getFrontAngle(), rad2deg(steer.getFrontAngle()),
    //        steer.getBackAngle(), rad2deg(steer.getBackAngle()),
    //        steer.getVelocity());
    printf("is_ok:%d,\n",puropo.getCommunicatable(0));
    
    steer.print_debug();
    can1.print_debug();
    can2.print_debug();
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

    Timer time_ms;
    time_ms.start();
    int ms;
    while (true)
    {
        if (cnt_1ms > 100)
        {
            cnt_1ms = 0;
            // print_debug();
            // puts("");
        }
        printf("%d\n",ms - time_ms.read_ms());
        ms = time_ms.read_ms();

        controller.setSteerDirection(puropo.getLeftX(0));
        controller.setSteerVelocity(puropo.getLeftY(0));
        controller.setSteerTurn(puropo.getRightX(0));

        steer.setTurn(controller.getSteerTurn());
        steer.setDirection(controller.getSteerDirection());
        steer.setVelocity(controller.getSteerVelocity());

        servo.setAngle(0, steer.getFrontAngle());
        servo.setAngle(1, steer.getBackAngle());

        robomas.setCurrent(0, steer.getVelocity());
        robomas.setCurrent(1, steer.getVelocity());
        robomas.setCurrent(2, -steer.getVelocity());
        robomas.setCurrent(3, -steer.getVelocity());

        update();
    }
}