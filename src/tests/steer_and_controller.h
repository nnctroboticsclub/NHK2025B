#include "mbed.h"
#include "N_servo.h"
#include "N_steer.h"
#include "N_controller.h"
#include "N_puropo.h"
#include "N_robomas.h"
#include "definitions.h"

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
    printf("is_ok:%d,\n", puropo.getCommunicatable(0));

    // steer.print_debug();
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

    float cmd_angle = 0;
    float cmd_vel = 0;
    // 前輪のタイヤの中心から後輪のタイヤの中心までの長さ
    float wheel_base = 900e-3; // [m]
    // 右輪のタイヤの中心から左輪のタイヤの中心までの長さ
    float track_width = 400e-3; // [m]
    // タイヤの半径
    float wheel_radius = 76e-3; // [m]
    float turning_radius; // 回転半径(右輪と左輪の中心が円の線を通る)[m]
    float back_omegaL,back_omegaR,front_omegaL,front_omegaR; // 各車輪の角速度[rad/s]
    float *wheel_omega[] = {&front_omegaR, &front_omegaL, &back_omegaL, &back_omegaR}; // 角速度の配列
    float back_vL,back_vR,front_vL,front_vR; // 各車輪の速度[m/s]
    float *wheel_v[] = {&front_vR, &front_vL, &back_vL, &back_vR}; // 角速度の配列

    // Timer time_ms;
    // time_ms.start();
    // int ms;
    while (true)
    {
        // if (cnt_1ms > 100)
        // {
        //     cnt_1ms = 0;
        //     printf("ang:%0.3f,vel:%0.3f,R:%0.3f,L:%0.3f",cmd_angle,cmd_vel, back_omegaR,back_omegaL);
        //     print_debug();
        //     puts("");
        // }
        // printf("%d\n",ms - time_ms.read_ms());
        // ms = time_ms.read_ms();

        // controller.setSteerDirection(puropo.getLeftX(0));
        // controller.setSteerVelocity(puropo.getLeftY(0));
        // controller.setSteerTurn(puropo.getRightX(0));
        led[1] = puropo.getCommunicatable(0);
        ES = puropo.getCommunicatable(0);

        // steer.setTurn(controller.getSteerTurn());
        // steer.setDirection(controller.getSteerDirection());
        // steer.setVelocity(controller.getSteerVelocity());

        // servo.setAngle(0, steer.getFrontAngle());
        // servo.setAngle(1, steer.getBackAngle());

        // robomas.setCurrent(0, steer.getVelocity());
        // robomas.setCurrent(1, steer.getVelocity());
        // robomas.setCurrent(2, -steer.getVelocity());
        // robomas.setCurrent(3, -steer.getVelocity());
        cmd_angle = puropo.getLeftX(0) * M_PI / 180 * 35; // [rad]
        cmd_vel = puropo.getLeftY(0); // [m/s]

        turning_radius = wheel_base / (2 * sin(cmd_angle));

        front_vR = cmd_vel - (cmd_vel*(track_width/2) / turning_radius);
        front_vL = cmd_vel + (cmd_vel*(track_width/2) / turning_radius);
        back_vR = cmd_vel - (cmd_vel*(track_width/2) / turning_radius);
        back_vL = cmd_vel + (cmd_vel*(track_width/2) / turning_radius);

        front_omegaR = front_vR / wheel_radius;
        front_omegaL = -front_vL / wheel_radius;
        back_omegaR = back_vR / wheel_radius;
        back_omegaL = -back_vL / wheel_radius;
        
        cmd_vel *= 5.0;

        servo.setAngle(1, M_PI / 180 * 120 + cmd_angle);
        servo.setAngle(0, M_PI / 180 * 120 - cmd_angle);
        robomas.setCurrent(0, cmd_vel);
        robomas.setCurrent(1, -cmd_vel);
        
        // robomas.setCurrent(0,back_omegaR);
        // robomas.setCurrent(1,back_omegaL);
        update();
        ThisThread::sleep_for(1ms); // ちょっと待ってあげたほうがいいかも
    }
}

// #include "mbed.h"
// #include "N_servo.h"
// #include "N_steer.h"
// #include "N_controller.h"
// #include "N_puropo.h"
// #include "N_robomas.h"
// #include "definitions.h"
 
// // === 車体パラメータ ===
// #define MAX_ANGULAR_SPEED 2.6f        //最大角速度(rad/s)
// #define MAX_DRIVE_SPEED 5.0f                            // 出力最大値
// #define TRACK_WIDTH 0f                               // 車体幅(タイヤとタイヤの距離)（m）
 
// void control() {
//     float stickX = puropo.getLeftX(0);
//     float stickY = puropo.getLeftY(0);
 
//     //角速度計算
//     float angular_speed = pow(stickX, 1.5) * MAX_ANGULAR_SPEED;
 
//     // 駆動速度計　
//     float drive_speed = stickY * MAX_DRIVE_SPEED;
 
//     // 左右モーター出力計算（差動制御）
//     float left_speed = -1 * (drive_speed - (angular_speed * (TRACK_WIDTH / 2.0f)));
//     float right_speed = drive_speed + (angular_speed * (TRACK_WIDTH / 2.0f));
 
//     // モーター出力
//     robomas.setCurrent(0,left_speed);
//     robomas.setCurrent(1,right_speed);
// }
 
 
// // サーボパラメータ設定
// std::array<ServoParameter, NUM_OF_SERVO> servo_param{{
//     []
//     { ServoParameter p; p.id = 1; return p; }(),
//     []
//     { ServoParameter p; p.id = 2; return p; }(),
// }};
 
// // ロボマスパラメータ設定
// std::array<RobomasParameter, NUM_OF_ROBOMAS> robomas_param{{
//     []
//     { RobomasParameter p; p.robomas_id = 1; return p; }(),
//     []
//     { RobomasParameter p; p.robomas_id = 2; return p; }(),
//     []
//     { RobomasParameter p; p.robomas_id = 3; return p; }(),
//     []
//     { RobomasParameter p; p.robomas_id = 4; return p; }(),
// }};
 
// NHK2025B_Servo servo(servo_param);
// NHK2025B_Robomas robomas(robomas_param);
// NHK2025B_Steer steer;
// NHK2025B_Controller controller;
// NHK2025B_Puropo puropo;
 
// Thread thread;
// Ticker ticker;
 
// void send_thread()
// {
//     while (true)
//     {
//         servo.write();
//         robomas.write();
//         ThisThread::sleep_for(1ms);
//     }
// }
 
// int cnt_1ms = 0;
// void update_1ms()
// {
//     // 今後使ってくならこのままでもいいけどpuropo.update_ts()のみならmain()の中で呼び出してもいい
//     // robomas.update_ts();
//     puropo.update_ts();
//     cnt_1ms++;
// }
 
// void update()
// {
//     controller.update();
//     puropo.update();
//     servo.update();
//     robomas.update();
//     can1.update();
//     can2.update();
// }
 
// // デバッグ出力関数
// void print_debug()
// {
//     // controller.print_debug();
//     // puropo.print_debug();
//     // printf("front: %f(%d), back: %f(%d), velocity: %f\n",
//     //        steer.getFrontAngle(), rad2deg(steer.getFrontAngle()),
//     //        steer.getBackAngle(), rad2deg(steer.getBackAngle()),
//     //        steer.getVelocity());
//     printf("is_ok:%d,\n", puropo.getCommunicatable(0));
 
//     steer.print_debug();
//     can1.print_debug();
//     can2.print_debug();
// }
 
// int main()
// {
//     servo.setup();
//     puropo.setup();
//     robomas.setup();
 
//     steer.setup();
//     controller.setup();
//     ES = 1;
 
//     thread.start(&send_thread);
//     ticker.attach(&update_1ms, 1ms);
 
//     // Timer time_ms;
//     // time_ms.start();
//     // int ms;
//     while (true)
//     {
//         if (cnt_1ms > 100)
//         {
//             cnt_1ms = 0;
//             print_debug();
//             puts("");
//         }
//         // printf("%d\n",ms - time_ms.read_ms());
//         // ms = time_ms.read_ms();
 
//         // controller.setSteerDirection(puropo.getLeftX(0));
//         // controller.setSteerVelocity(puropo.getLeftY(0));
//         // controller.setSteerTurn(puropo.getRightX(0));
//         led[1] = puropo.getCommunicatable(0);
//         ES = puropo.getCommunicatable(0);
 
//         // steer.setTurn(controller.getSteerTurn());
//         // steer.setDirection(controller.getSteerDirection());
//         // steer.setVelocity(controller.getSteerVelocity());
 
//         // servo.setAngle(0, steer.getFrontAngle());
//         // servo.setAngle(1, steer.getBackAngle());
 
//         // robomas.setCurrent(0, steer.getVelocity());
//         // robomas.setCurrent(1, steer.getVelocity());
//         // robomas.setCurrent(2, -steer.getVelocity());
//         // robomas.setCurrent(3, -steer.getVelocity());
 
//         // servo.setAngle(1, M_PI_2 + puropo.getLeftX(0) * M_PI / 180 * 25);
//         // servo.setAngle(0, M_PI_2 - puropo.getLeftX(0) * M_PI / 180 * 25);
//         // robomas.setCurrent(0, puropo.getLeftY(0) * 5.0);
//         // robomas.setCurrent(1, -puropo.getLeftY(0) * 5.0);
//         control();
//         update();
//         ThisThread::sleep_for(1ms); // ちょっと待ってあげたほうがいいかも
//     }
// }
