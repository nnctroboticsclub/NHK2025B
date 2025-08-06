#include "mbed.h"
#include "N_servo.h"
#include "N_steer.h"
#include "N_controller.h"
#include "N_puropo.h"
#include "N_robomas.h"
#include "N_PID.h"
#include "definitions.h"

#define CH_B 5
#define CH_VR 6
#define CH_C 7
#define CH_E 8
#define CH_H 9
#define CH_F 10

DigitalOut button1(pins.SW1);
DigitalOut button2(pins.SW2);
DigitalOut button3(pins.SW3);

// サーボパラメータ
std::array<ServoParameter, NUM_OF_SERVO> servo_param{{
    [] { ServoParameter p; p.id = 1; return p; }(), // 前
    [] { ServoParameter p; p.id = 2; return p; }(), // 後
}};

// ロボマスパラメータ
std::array<RobomasParameter, NUM_OF_ROBOMAS> robomas_param{{
    [] { RobomasParameter p; p.robomas_id = 3; return p; }(), // 前右
    [] { RobomasParameter p; p.robomas_id = 4; return p; }(), // 後右
    [] { RobomasParameter p; p.robomas_id = 1; return p; }(), // 後左
    [] { RobomasParameter p; p.robomas_id = 2; return p; }(), // 前左
}};

// PID
std::array<PidParameter, NUM_OF_ROBOMAS> robomas_pid_param{
    [] { PidParameter p; p.kp = 0.146; p.ki = 0.01e-3; p.kd = 0.14*1.2; p.out_max = 2.0; return p; }(),
    [] { PidParameter p; p.kp = 0.14; p.ki = 0.015e-3; p.kd = 0.14*1.2; p.out_max = 2.0; return p; }(),
    [] { PidParameter p; p.kp = 0.14; p.ki = 0.007e-3; p.kd = 0.145*1.2; p.out_max = 2.0; return p; }(),
    [] { PidParameter p; p.kp = 0.146; p.ki = 0.0105e-3; p.kd = 0.14*2.5; p.out_max = 2.0; return p; }(),
};

NHK2025B_Servo servo(servo_param);
NHK2025B_Robomas robomas(robomas_param);
NHK2025B_Steer steer;
NHK2025B_Controller controller;
NHK2025B_Puropo puropo;
NHK2025B_PID robomas_pid(robomas_pid_param);

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
    // 1msごとの周期処理（なるべく短く）
    robomas.update_ts();
    puropo.update_ts();
    robomas_pid.setProcessValue(0, robomas.getVeclocity(0));
    robomas_pid.setProcessValue(1, robomas.getVeclocity(1));
    robomas_pid.setProcessValue(2, robomas.getVeclocity(2));
    robomas_pid.setProcessValue(3, robomas.getVeclocity(3));
    robomas_pid.update_ts();
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

// デバッグ用出力関数（使用時コメントアウト解除）
void print_debug()
{
    printf("is_ok:%d,\n", puropo.getCommunicatable(0));
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

    can2.read_start();

    float cmd_angle = 0;
    float cmd_vel = 0;

    // 前後輪のホイールベース
    float wheel_base = 900e-3; // [m]
    // 左右輪のトレッド幅
    float track_width = 400e-3; // [m]
    // 車輪の半径
    float wheel_radius = 76e-3; // [m]
    float turning_radius;
    float back_omegaL, back_omegaR, front_omegaL, front_omegaR;
    float *wheel_omega[] = {&front_omegaR, &front_omegaL, &back_omegaL, &back_omegaR};
    float back_vL, back_vR, front_vL, front_vR;
    float *wheel_v[] = {&front_vR, &front_vL, &back_vL, &back_vR};

    enum
    {
        HIGH_SPEED = 10,
        NOMAL_SPEED = 3,
        LOW_SPEED = 1,
    } speed_mode;

    while (true)
    {

        if (cnt_1ms > 100)
        {
            cnt_1ms = 0;
        }

        led[1] = puropo.getCommunicatable(0);
        ES = puropo.getCommunicatable(0) & (puropo.getAxis(0, CH_F) == -1);
        switch (int(puropo.getAxis(0, CH_E)))
        {
        case 0:
            speed_mode = NOMAL_SPEED;
            break;
        case 1:
            speed_mode = HIGH_SPEED;
            break;
        case -1:
            speed_mode = LOW_SPEED;
            break;
        }

        cmd_angle = -puropo.getLeftX(0) * M_PI / 180 * 35;                                         // [rad]
        cmd_vel = puropo.getLeftY(0) * wheel_radius * M3508_MAX_RAD_PER_SEC * (speed_mode / 10.0); // [m/s]
        turning_radius = wheel_base / (2 * sin(cmd_angle * 2));

        front_vR = cmd_vel + (cmd_vel * (track_width / 2) / turning_radius);
        front_vL = cmd_vel - (cmd_vel * (track_width / 2) / turning_radius);
        back_vR = cmd_vel + (cmd_vel * (track_width / 2) / turning_radius);
        back_vL = cmd_vel - (cmd_vel * (track_width / 2) / turning_radius);

        front_omegaR = -front_vR / wheel_radius;
        front_omegaL = front_vL / wheel_radius;
        back_omegaR = -back_vR / wheel_radius;
        back_omegaL = back_vL / wheel_radius;

        int reverse = 1;

        robomas_pid.setGoalValue(0, front_omegaR);
        robomas_pid.setGoalValue(1, front_omegaL);
        robomas_pid.setGoalValue(2, back_omegaL);
        robomas_pid.setGoalValue(3, back_omegaR);

        cmd_vel *= 5.0;

        servo.setAngle(0, M_PI / 180 * (120 + 20 * puropo.getAxis(0, CH_VR)) + cmd_angle);
        servo.setAngle(1, M_PI / 180 * (120 - 20 * puropo.getAxis(0, CH_VR)) - cmd_angle * reverse);

        robomas.setCurrent(0, robomas_pid.getOutput(0));
        robomas.setCurrent(1, robomas_pid.getOutput(1));
        robomas.setCurrent(2, robomas_pid.getOutput(2));
        robomas.setCurrent(3, robomas_pid.getOutput(3));

        update();
        ThisThread::sleep_for(1ms); // 過負荷防止
    }
}
