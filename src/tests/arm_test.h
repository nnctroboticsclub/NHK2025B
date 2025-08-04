// #include <mbed.h>
// #include "definitions.h"
// #include "N_robomas.h"
// #include "N_puropo.h"
// #include "N_controller.h"
// #include "N_arm.h"
// #include "N_rohmMD.h"
// #include "N_PID.h"
// #include "QEI.h"

// DigitalIn button1(BUTTON1);
// DigitalIn button2(pins.SW2);
// DigitalIn button3(pins.SW3);
// Timer enc_timer;
// QEI qei1(PC_10,PC_11,NC,50,&enc_timer,QEI::X2_ENCODING);
// QEI qei2(PC_12,PD_2,NC,50,&enc_timer,QEI::X2_ENCODING);

// std::array<RobomasParameter, NUM_OF_ROBOMAS> robomas_params{
//     []{RobomasParameter p;
//         p.robomas_id = 2, p.type = RobomasParameter::TYPE_OF_M2006, p.ican_ptr = &can1;
//         return p;
//     }()
// };
// std::array<RohmMdParameter, NUM_OF_ROHM_MD> rohm_params{{
//     []{RohmMdParameter p;
//         p.id = 1, p.ican = &can2;
//         return p;}(),
//     []{RohmMdParameter p;
//         p.id = 2, p.ican = &can2;
//         return p;}(),
// }};
// std::array<PidParameter, NUM_OF_ROHM_MD> rohm_pid_params{
//     []{PidParameter p;
//         p.kp = 0.05;
//         p.out_max = 0.9;
//         return p;
//     }(),
//     []{PidParameter p;
//         p.kp = 0.05;
//         p.out_max = 0.9;
//         return p;
//     }()
// };
// NHK2025B_Robomas robomas(robomas_params);
// NHK2025B_RohmMD rohm(rohm_params);
// NHK2025B_PID rohm_pid(rohm_pid_params);
// // NHK2025B_Puropo puropo;
// // NHK2025B_Puropo puropo;
// // NHK2025B_Controller controller;
// // NHK2025B_Arm arm;

// Thread thread;
// Ticker ticker;

// /**
//  * @brief 2次方程式の解の公式
//  * 
//  * @param a 2次の係数
//  * @param b 1次の係数
//  * @param c 0次の係数
//  * @param large 大きい方の解(虚数解の場合は操作しない)
//  * @param small 小さい方の解(虚数解の場合は操作しない)
//  * 
//  * @return 判別式Dの計算結果
//  */
// float quadraticFormula(float a, float b, float c, float *large=NULL, float *small=NULL){
//     float disc = b*b - 4*a*c;
//     if(disc<0){
//         return disc;
//     }
//     if(large!=NULL)*large = (-b + sqrt(disc)) / (2*a);
//     if(small!=NULL)*small = (-b - sqrt(disc)) / (2*a);
// }

// // 根元のアームの長さ
// float L1; // [mm]
// // 先端のアームの長さ
// float L2; // [mm]
// /**
//  * @brief アームの先端が目標の高さ,距離になるように計算する
//  * 
//  * @param d 目標距離
//  * @param h 目標高度
//  * @param angle1 根元の方のアームの角度
//  * @param angle2 先端の方のアームの角度
//  * 
//  * @return -1: 到達不可
//  */
// int calcArmAngle(float d, float h, float *angle1, float *angle2){
//     float s = L1*L1 - L2*L2 - d*d - h*h;
//     float small_x, large_x; // 二つの円の交点のx座標
//     if(quadraticFormula(
//         1+(d*d/h*h),
//         d * s / (h*h),
//         s / (4 * h * h) - L1*L1,
//         &large_x,
//         &small_x
//     ) < 0) return -1;
//     if(small_x >= 0){
//         *angle1 = acos(small_x/L1);
//         *angle2 = M_PI - *angle1 - acos((d-small_x)/L2);
//     }else{
//         *angle1 = acos(large_x/L1);
//         *angle2 = M_PI - *angle1 - acos((d-large_x)/L2);
//     }
// }

// void setup()
// {
//     robomas.setup();
//     rohm.setup();
//     // puropo.setup();
//     // controller.setup();
//     // arm.setup();
//     enc_timer.start();
//     // can1.read_start();
//     // can2.read_start();
// }

// void update()
// {
//     robomas.update();
//     rohm.update();
//     // puropo.update();
//     // controller.update();
//     // arm.udpate();
//     can1.update();
//     can2.update();
// }

// int cnt_1ms = 0;
// void update_1ms()
// {
//     robomas.update_ts();
//     // puropo.update_ts();
//     rohm_pid.setProcessValue(0, qei1.getAngle());
//     rohm_pid.setProcessValue(1, qei2.getAngle());
//     rohm_pid.update_ts();
//     cnt_1ms++;
// }

// void send_thread()
// {
//     while(1){
//         // robomas.write();
//         rohm.write();
//         ThisThread::sleep_for(1ms);
//     }
// }

// void print_debug()
// {
//     // robomas.print_debug();
//     // puropo.print_debug();
//     // can2.print_debug();
//     can2.print_debug();
// }

// int main()
// {
//     setup();
//     thread.start(&send_thread);
//     ticker.attach(&update_1ms,1ms);
//     ES=1;
//     float current = 0;
//     int cnt100ms = 0;
//     float cmd_pos =0 ;
//     while(1){
//         if(cnt_1ms > 100){
//             // printf(">current:%f\n",current);
//             printf(">cmd_pos:%f\n",cmd_pos);
//             printf(">enc1:%f\n",qei1.getAngle());
//             printf(">enc2:%f\n",qei2.getAngle());
//             printf(">current1:%f\n",rohm_pid.getOutput(0));
//             // printf(">current2:%f\n",rohm_pid.getOutput(1));
//             // print_debug();
//             puts("");
//             cnt_1ms = 0;
//             current += (!button1) * 0.01;
//             current -= button2 * 0.1;
//             cnt100ms++;
//         }
//         // controller.setArmEffort(puropo.getLeftY(0));
//         // arm.setEffort(0,controller.getArmEffort());
//         // 動かなかったら69, 70,73行目コメントアウトして,下行のコメントアウトを解除してください
//         // robomas.setCurrent(0,puropo.getLeftY(0) * 5.0);
//         // robomas.setCurrent(0,arm.getEffort(0));
//         // ES = puropo.getCommunicatable(0);
//         if(button3) current = 0;
//         if(current > 5.0) current = 5.0;
//         if(current < -5.0) current = -5.0;
//         robomas.setCurrent(0,current);
//         // current = sin(cnt100ms / 10.0 * M_PI);
//         // cmd_pos = sin(cnt100ms/ 10.0 * M_PI) * 90.0;
//         if(!button1)cmd_pos = -30;
//         else cmd_pos = 0;
//         current = 0.9;
//         if(!button1)current = -0.3;
//         // rohm.setPower(0,-current);
//         // rohm.setPower(1,-current);
//         rohm_pid.setGoalValue(0, cmd_pos);
//         // rohm_pid.setGoalValue(1, 0);
//         rohm.setPower(0, rohm_pid.getOutput(0));
//         // rohm.setPower(1, rohm_pid.getOutput(1));
//         update();
//         // ThisThread::sleep_for();
//     }
// }

// // #include <mbed.h>
// // #include "QEI.h"

// // QEI qei1();
// // QEI qei2();

// // int main()
// // {
// //     ;
// // }


#include <mbed.h>
#include "definitions.h"
#include "N_robomas.h"
#include "N_puropo.h"
#include "N_controller.h"
#include "N_arm.h"
#include "N_rohmMD.h"
#include "N_PID.h"
#include "QEI.h"

DigitalIn button1(BUTTON1);
DigitalIn button2(pins.SW2);
DigitalIn button3(pins.SW3);
Timer enc_timer;
QEI qei1(PC_10,PC_11,NC,50,&enc_timer,QEI::X2_ENCODING);
QEI qei2(PC_12,PD_2,NC,50,&enc_timer,QEI::X2_ENCODING);

std::array<RobomasParameter, NUM_OF_ROBOMAS> robomas_params{
    []{RobomasParameter p;
        p.robomas_id = 2, p.type = RobomasParameter::TYPE_OF_M2006, p.ican_ptr = &can1;
        return p;
    }()
};
std::array<RohmMdParameter, NUM_OF_ROHM_MD> rohm_params{{
    []{RohmMdParameter p;
        p.id = 1, p.ican = &can2;
        return p;}(),
    []{RohmMdParameter p;
        p.id = 2, p.ican = &can2;
        return p;}(),
}};
std::array<PidParameter, NUM_OF_ROHM_MD> rohm_pid_params{
    []{PidParameter p;
        p.kp = 0.05;
        p.out_max = 0.9;
        return p;
    }(),
    []{PidParameter p;
        p.kp = 0.05;
        p.out_max = 0.9;
        return p;
    }()
};
std::array<ArmParameter, 2> arm_parameter{
    []{ArmParameter p;
        return p;
    }(),
    []{ArmParameter p;
        return p;
    }(),
};
NHK2025B_Robomas robomas(robomas_params);
NHK2025B_RohmMD rohm(rohm_params);
NHK2025B_PID rohm_pid(rohm_pid_params);
NHK2025B_Arm arm;
// NHK2025B_Puropo puropo;
// NHK2025B_Puropo puropo;
// NHK2025B_Controller controller;
// NHK2025B_Arm arm;

Thread thread;
Ticker ticker;

void setup()
{
    robomas.setup();
    rohm.setup();
    // puropo.setup();
    // controller.setup();
    arm.setup();
    enc_timer.start();
    // can1.read_start();
    // can2.read_start();
}

void update()
{
    robomas.update();
    rohm.update();
    // puropo.update();
    // controller.update();
    arm.udpate();
    can1.update();
    can2.update();
}

int cnt_1ms = 0;
void update_1ms()
{
    robomas.update_ts();
    // puropo.update_ts();
    rohm_pid.setProcessValue(0, qei1.getAngle());
    rohm_pid.setProcessValue(1, qei2.getAngle());
    rohm_pid.update_ts();
    cnt_1ms++;
}

void send_thread()
{
    while(1){
        // robomas.write();
        rohm.write();
        ThisThread::sleep_for(1ms);
    }
}

void print_debug()
{
    // robomas.print_debug();
    // puropo.print_debug();
    // can2.print_debug();
    can2.print_debug();
}

int main()
{
    setup();
    thread.start(&send_thread);
    ticker.attach(&update_1ms,1ms);
    ES=1;
    float current = 0;
    int cnt100ms = 0;
    float cmd_pos =0 ;
    while(1){
        if(cnt_1ms > 100){
            // printf(">current:%f\n",current);
            printf(">cmd_pos:%f\n",arm.getArmAngle(Direction2::RIGHT));
            printf(">enc1:%f\n",qei1.getAngle());
            printf(">enc2:%f\n",qei2.getAngle());
            printf(">current1:%f\n",rohm_pid.getOutput(0));
            // printf(">current2:%f\n",rohm_pid.getOutput(1));
            // print_debug();
            puts("");
            cnt_1ms = 0;
            current += (!button1) * 0.01;
            current -= button2 * 0.1;
            cnt100ms++;
        }
        if(button3) current = 0;
        if(current > 5.0) current = 5.0;
        if(current < -5.0) current = -5.0;
        robomas.setCurrent(0,current);
        if(!button1)arm.setGripPosition(Direction2::RIGHT, 30*M_PI*180);
        else arm.setGripPosition(Direction2::RIGHT, 0);
        current = 0.9;
        if(!button1)current = -0.3;
        rohm_pid.setGoalValue(0, arm.getGripPosition(Direction2::RIGHT));
        rohm.setPower(0, rohm_pid.getOutput(0));
        update();
    }
}