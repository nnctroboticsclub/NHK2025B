#include "mbed.h"
#include "N_servo.h"
#include "N_steer.h"
#include "N_controller.h"
#include "N_puropo.h"
#include "N_robomas.h"
#include "N_PID.h"
#include "definitions.h"

#define CH_B 5
#define CH_C 7
#define CH_E 8
#define CH_F 10

DigitalOut button1(pins.SW1);
DigitalOut button2(pins.SW2);
DigitalOut button3(pins.SW3);

// サーボパラメータ設定
std::array<ServoParameter, NUM_OF_SERVO> servo_param{{
    []
    { ServoParameter p; p.id = 1; return p; }(), // 前
    []
    { ServoParameter p; p.id = 2; return p; }(), // 後
}};

// ロボマスパラメータ設定
std::array<RobomasParameter, NUM_OF_ROBOMAS> robomas_param{{
    []
    { RobomasParameter p; p.robomas_id = 3; return p; }(), // 右前
    []
    { RobomasParameter p; p.robomas_id = 4; return p; }(), // 左前
    []
    { RobomasParameter p; p.robomas_id = 1; return p; }(), // 左後
    []
    { RobomasParameter p; p.robomas_id = 2; return p; }(), // 右後
}};

// PID
std::array<PidParameter, NUM_OF_ROBOMAS> robomas_pid_param{
    []{ PidParameter p; 
        p.kp = 5.0;
        return p;}(),
    []{ PidParameter p; 
        p.kp = 5.0;
        // p.kd = 0.101, 
        p.out_max = 2.0;
        return p;}(),
    []{ PidParameter p;
        p.kp = 0.3;
        // p.kd = 0.001;
        p.ki = 0.1e-3;
        return p;}(),
    []{ PidParameter p; 
        p.kp = 0.3;
        // p.kd = 0.001;
        p.ki = 0.1e-3;
        return p;}(),
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
    // 今後使ってくならこのままでもいいけどpuropo.update_ts()のみならmain()の中で呼び出してもいい
    robomas.update_ts();
    puropo.update_ts();
    robomas_pid.setProcessValue(2,robomas.getVeclocity(2));
    robomas_pid.setProcessValue(3,robomas.getVeclocity(3));
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
    puts("");
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
    enum{
        HIGH_SPEED = 10,
        NOMAL_SPEED = 3,
        LOW_SPEED = 1,
    }speed_mode;

    // Timer time_ms;
    // time_ms.start();
    // int ms;
    while (true)
    {
        if (cnt_1ms > 100)
        {
            cnt_1ms = 0;
            printf(">back_OmegaL:%f\n",back_omegaL);
            printf(">back_OmegaR:%f\n",back_omegaR);
            printf(">real_OmegaL:%f\n",robomas.getVeclocity(2));
            printf(">real_OmegaR:%f\n",robomas.getVeclocity(3));
            printf(">back_currentL:%f\n",robomas_pid.getOutput(2));
            printf(">back_currentR:%f\n",robomas_pid.getOutput(3));
            printf(">kp:%f\n",robomas_pid_param[2].kp);
            printf(">ki:%f\n",robomas_pid_param[2].ki);
            // print_debug();
            // puts("");
            for(int i=0;i<4;i++){
                robomas_pid_param[i];
                switch(int(puropo.getAxis(0, CH_C))){
                    case 0: robomas_pid_param[i].kp += 0.1 * (puropo.getAxis(0, CH_B) == 1);
                    case 1: robomas_pid_param[i].ki += 0.1 * (puropo.getAxis(0, CH_B) == 1);
                    case -1: robomas_pid_param[i].kd += 0.1 * (puropo.getAxis(0, CH_B) == 1);
                }
                // robomas_pid.setParameter(i,robomas_pid_param[i]);
                if(button3){
                    robomas.resetState(i);
                }
            }
        }
        // printf("ang:%0.3f,vel:%0.3f,R:%0.3f,L:%0.3f",cmd_angle,cmd_vel, back_omegaR,back_omegaL);
        // printf("%d\n",ms - time_ms.read_ms());
        // ms = time_ms.read_ms();

        // controller.setSteerDirection(puropo.getLeftX(0));
        // controller.setSteerVelocity(puropo.getLeftY(0));
        // controller.setSteerTurn(puropo.getRightX(0));
        led[1] = puropo.getCommunicatable(0);
        ES = puropo.getCommunicatable(0) & (puropo.getAxis(0, CH_F) == 1);
        switch(int(puropo.getAxis(0, CH_E))){
            case 0: speed_mode = NOMAL_SPEED; break;
            case 1: speed_mode = HIGH_SPEED; break;
            case -1: speed_mode = LOW_SPEED; break;
            default: break;
        }

        // steer.setTurn(controller.getSteerTurn());
        // steer.setDirection(controller.getSteerDirection());
        // steer.setVelocity(controller.getSteerVelocity());

        // servo.setAngle(0, steer.getFrontAngle());
        // servo.setAngle(1, steer.getBackAngle());

        // robomas.setCurrent(0, steer.getVelocity());
        // robomas.setCurrent(1, steer.getVelocity());
        // robomas.setCurrent(2, -steer.getVelocity());
        // robomas.setCurrent(3, -steer.getVelocity());
        cmd_angle = -puropo.getLeftX(0) * M_PI / 180 * 35; // [rad]
        cmd_vel = puropo.getLeftY(0) * wheel_radius * M3508_MAX_RAD_PER_SEC * (speed_mode / 10.0); // [m/s]

        turning_radius = wheel_base / (2 * sin(cmd_angle * 2));

        front_vR = cmd_vel + (cmd_vel*(track_width/2) / turning_radius);
        front_vL = cmd_vel - (cmd_vel*(track_width/2) / turning_radius);
        back_vR = cmd_vel + (cmd_vel*(track_width/2) / turning_radius);
        back_vL = cmd_vel - (cmd_vel*(track_width/2) / turning_radius);

        float speedest_vel = max(abs(front_vL),abs(front_vR));
        if(speedest_vel > wheel_radius * M3508_MAX_RAD_PER_SEC){
            float scale = (wheel_radius * M3508_MAX_RAD_PER_SEC) /speedest_vel;
            for(int i=0;i<4;i++){
                *wheel_v[i] *= scale;
            }
        }

        front_omegaR = -front_vR / wheel_radius;
        front_omegaL = front_vL / wheel_radius;
        back_omegaR = -back_vR / wheel_radius;
        back_omegaL = back_vL / wheel_radius;

        robomas_pid.setGoalValue(2,back_omegaL);
        robomas_pid.setGoalValue(3,back_omegaR);
        
        cmd_vel *= 5.0;

        servo.setAngle(0, M_PI / 180 * 120 + cmd_angle);
        servo.setAngle(1, M_PI / 180 * 120 - cmd_angle);
        // robomas.setCurrent(0, cmd_vel);
        // robomas.setCurrent(1, -cmd_vel);
        
        // robomas.setCurrent(0,back_omegaR);
        // robomas.setCurrent(1,back_omegaL);

        robomas.setCurrent(2, robomas_pid.getOutput(2));
        robomas.setCurrent(3, robomas_pid.getOutput(3));
        // robomas.setCurrent(2,1.0);
        // robomas.setCurrent(3,5.0);
        update();
        ThisThread::sleep_for(1ms); // ちょっと待ってあげたほうがいいかも
    }
}
