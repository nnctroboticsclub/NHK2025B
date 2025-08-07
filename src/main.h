#include <mbed.h>
#include "N_PID.h"
#include "N_puropo.h"
#include "N_robomas.h"
#include "N_rohmMD.h"
#include "N_servo.h"
#include "N_steer.h"
#include "N_arm.h"
#include "QEI.h"

// NUM_OF_ROBOMAS_M3508 4
// NUM_OF_ROBOMAS_M2006 2
// NUM_OF_SERVO 2
// NUM_OF_ROHM_MD 2
// NUM_OF_PUROPO 2
// NUM_OF_PID_CONTROLLER 4
// NUM_OF_ARM 2

#define CH_A 10
#define CH_B 
#define CH_C 9
#define CH_E 
#define CH_F 8
#define CH_G 7
#define CH_H 6
#define CH_VR 5
#define STEER_ROBOMAS_OF(x) (x+0)
#define ARM_ROBOMAS_OF(x) (x+4)
#define ARM_HAND_PID_OF(x) (x+0)
#define ARM_JOINT_PID_OF(x) (x+2)

std::array<RobomasParameter, NUM_OF_ROBOMAS> robomas_params{
    // 右前輪
    []{RobomasParameter p;
        p.robomas_id = 1, p.type = RobomasParameter::TYPE_OF_M3508, p.ican_ptr = &can1;
        return p;
    }(),
    // 左前輪
    []{RobomasParameter p;
        p.robomas_id = 2, p.type = RobomasParameter::TYPE_OF_M3508, p.ican_ptr = &can1;
        return p;
    }(),
    // 左後輪
    []{RobomasParameter p;
        p.robomas_id = 3, p.type = RobomasParameter::TYPE_OF_M3508, p.ican_ptr = &can1;
        return p;
    }(),
    // 右後輪
    []{RobomasParameter p;
        p.robomas_id = 4, p.type = RobomasParameter::TYPE_OF_M3508, p.ican_ptr = &can1;
        return p;
    }(),
    // 右ハンド
    []{RobomasParameter p;
        p.robomas_id = 1, p.type = RobomasParameter::TYPE_OF_M2006, p.ican_ptr = &can2;
        return p;
    }(),
    // 左ハンド
    []{RobomasParameter p;
        p.robomas_id = 2, p.type = RobomasParameter::TYPE_OF_M2006, p.ican_ptr = &can2;
        return p;
    }(),
};

std::array<RohmMdParameter, NUM_OF_ROHM_MD> rohm_params{
    // 右ジョイント
    []{RohmMdParameter p;
        p.id = 1, p.ican = &can2;
        return p;
    }(),
    // 左ジョイント
    []{RohmMdParameter p;
        p.id = 2, p.ican = &can2;
        return p;
    }(),
};

std::array<ServoParameter, NUM_OF_SERVO> servo_params{
    // 前ステア
    []{ServoParameter p;
        p.id = 1, p.board_id = 1, p.ican = &can1;
        return p;
    }(),
    // 後ステア
    []{ServoParameter p;
        p.id = 2, p.board_id = 1, p.ican = &can1;
        return p;
    }()
};

std::array<PidParameter, NUM_OF_PID_CONTROLLER> arm_pid_params{
    // ハンド右
    []{PidParameter p;
        p.kp = 0.05;
        p.out_max = 0.9;
        return p;
    }(),
    // ハンド左
    []{PidParameter p;
        p.kp = 0.05;
        p.out_max = 0.9;
        return p;
    }(),
    // ジョイント右
    []{PidParameter p;
        p.kp = 0.05;
        p.out_max = 0.9;
        return p;
    }(),
    // ジョイント左
    []{PidParameter p;
        p.kp = 0.05;
        p.out_max = 0.9;
        return p;
    }()
};

std::array<PidParameter, NUM_OF_PID_CONTROLLER> steer_robomas_pid_params{
    // 右前輪
    []{PidParameter p;
        p.kp = 0.05;
        p.out_max = 0.9;
        return p;
    }(),
    // 左前輪
    []{PidParameter p;
        p.kp = 0.05;
        p.out_max = 0.9;
        return p;
    }(),
    // 左後輪
    []{PidParameter p;
        p.kp = 0.05;
        p.out_max = 0.9;
        return p;
    }(),
    // 右後輪
    []{PidParameter p;
        p.kp = 0.05;
        p.out_max = 0.9;
        return p;
    }(),
};

SteerParameter steer_parameter = []{
    SteerParameter p;
    return p;
}();

std::array<ArmParameter, NUM_OF_ARM> arm_parameter{
    // 右
    []{ArmParameter p;
        return p;
    }(),
    // 左
    []{ArmParameter p;
        return p;
    }()
};

NHK2025B_Puropo puropo;

NHK2025B_Servo servo(servo_params);
NHK2025B_Robomas robomas(robomas_params);
NHK2025B_RohmMD rohm(rohm_params);
NHK2025B_PID arm_pid(arm_pid_params);
NHK2025B_PID steer_pid(steer_robomas_pid_params);
NHK2025B_Arm arm(arm_parameter);
NHK2025B_Steer steer(steer_parameter);

Timer enc_timer;
std::array<QEI, 2> arm_qei{
    QEI(PC_10, PC_11, NC, 50, &enc_timer, QEI::X2_ENCODING), // CH_A
    QEI(PC_12, PD_2, NC, 50, &enc_timer, QEI::X2_ENCODING)  // CH_B
};

Ticker ticker;
Thread thread;

void setup(){
    robomas.setup();
    rohm.setup();
    servo.setup();
    puropo.setup();
    steer.setup();
    arm.setup();

}

void update(){
    robomas.update();
    rohm.update();

    steer.update();
    arm.update();
    puropo.update();
    can1.update();
    can2.update();
}

int cnt_1ms = 0;
void update_1ms(){
    cnt_1ms++;
    // アームのpidのプロセス値をセット
    arm_pid.setProcessValue(
        ARM_HAND_PID_OF((int)Direction2::RIGHT),
        arm_qei[(int)Direction2::RIGHT].getAngle()
    );
    arm_pid.setProcessValue(
        ARM_HAND_PID_OF((int)Direction2::LEFT),
        arm_qei[(int)Direction2::LEFT].getAngle()
    );
    arm_pid.setProcessValue(
        ARM_JOINT_PID_OF((int)Direction2::RIGHT),
        robomas.getAngle(ARM_ROBOMAS_OF((int)Direction2::RIGHT))
    );
    arm_pid.setProcessValue(
        ARM_JOINT_PID_OF((int)Direction2::LEFT), 
        robomas.getAngle(ARM_ROBOMAS_OF((int)Direction2::LEFT))
    );
    // ステアのpidのプロセス値をセット
    steer_pid.setProcessValue(
        (int)Wheel::FRONT_RIGHT,
        robomas.getAngle(STEER_ROBOMAS_OF((int)Wheel::FRONT_RIGHT))
    );
    steer_pid.setProcessValue(
        (int)Wheel::FRONT_LEFT,
        robomas.getAngle(STEER_ROBOMAS_OF((int)Wheel::FRONT_LEFT))
    );
    steer_pid.setProcessValue(
        (int)Wheel::BACK_LEFT,
        robomas.getAngle(STEER_ROBOMAS_OF((int)Wheel::BACK_LEFT))
    );
    steer_pid.setProcessValue(
        (int)Wheel::BACK_RIGHT,
        robomas.getAngle(STEER_ROBOMAS_OF((int)Wheel::BACK_RIGHT))
    );
    // pidのアップデート
    arm_pid.update_ts();
    steer_pid.update_ts();

    puropo.update_ts();
    robomas.update_ts();
}

void send_thread(){
    while(1){
        robomas.write();
        ThisThread::sleep_for(1ms);
        rohm.write();
        ThisThread::sleep_for(1ms);
        servo.write();
        ThisThread::sleep_for(1ms);
    }
}


int main(){
    setup();
    ticker.attach(&update_1ms,1ms);
    thread.start(&send_thread);
    can1.read_start();
    can2.read_start();
    enc_timer.start();
    int cnt_100ms;
    while(1){
        if(cnt_1ms > 100){
            printf(">FRomg: %f\n", steer.getOmega(Wheel::FRONT_RIGHT));
            printf(">FLomg: %f\n", steer.getOmega(Wheel::FRONT_LEFT));
            printf(">BLomg: %f\n", steer.getOmega(Wheel::BACK_LEFT));
            printf(">BRomg: %f\n", steer.getOmega(Wheel::BACK_RIGHT));
            printf(">Fang: %f\n", steer.getAngle(Direction1::FRONT));
            printf(">Bang: %f\n", steer.getAngle(Direction1::BACK));

            // printf(">Rang: %f\n", arm.getArmAngle(Direction2::RIGHT));
            // printf(">Lang: %f\n", arm.getArmAngle(Direction2::LEFT));
            // printf(">Rpos: %f\n", arm.getArmAngle(Direction2::RIGHT));
            // printf(">Lpos: %f\n", arm.getArmAngle(Direction2::LEFT));
            puts("");
            cnt_1ms = 0;
            cnt_100ms++;
        }
        ES = puropo.getCommunicatable(0);
        // プロポからそれぞれの値を取得
        arm.setHolding(
            Direction2::RIGHT,
            puropo.getAxis(0, CH_A)==-1.0
        );
        arm.setHolding(
            Direction2::LEFT,
            puropo.getAxis(0, CH_A)==-1.0
        );
        arm.setArmAngle(
            Direction2::RIGHT,
            puropo.getRightY(0) * arm_parameter[(int)Direction2::RIGHT].max_arm_angle
        );
        arm.setArmAngle(
            Direction2::LEFT,
            puropo.getLeftY(0) * arm_parameter[(int)Direction2::LEFT].max_arm_angle
        );

        if(puropo.getLeftY(0)!=0.0){
            steer.setVelocity(puropo.getLeftY(0));
            steer.setTurnAngle(puropo.getLeftX(0) * steer_parameter.steer_max_angle);
        }else if(puropo.getRightY(0)!=0.0){
            steer.setVelocity(puropo.getRightY(0));
            steer.setDirection(puropo.getRightX(0) * steer_parameter.steer_max_angle);
        }else{
            steer.setVelocity(0.0);
            steer.setTurnAngle(0.0);
            steer.setDirection(0.0);
        }

        // pidにセット
        arm_pid.setGoalValue(
            ARM_HAND_PID_OF((int)Direction2::RIGHT),
            arm.getGripPosition(Direction2::RIGHT)
        );
        arm_pid.setGoalValue(
            ARM_HAND_PID_OF((int)Direction2::LEFT),
            arm.getGripPosition(Direction2::LEFT)
        );
        arm_pid.setGoalValue(
            ARM_JOINT_PID_OF((int)Direction2::RIGHT),
            arm.getArmAngle(Direction2::RIGHT)
        );
        arm_pid.setGoalValue(
            ARM_JOINT_PID_OF((int)Direction2::LEFT),
            arm.getArmAngle(Direction2::LEFT)
        );

        steer_pid.setGoalValue(
            (int)Wheel::FRONT_RIGHT,
            steer.getOmega(Wheel::FRONT_RIGHT)
        );
        steer_pid.setGoalValue(
            (int)Wheel::FRONT_LEFT,
            steer.getOmega(Wheel::FRONT_LEFT)
        );
        steer_pid.setGoalValue(
            (int)Wheel::BACK_LEFT,
            steer.getOmega(Wheel::BACK_LEFT)
        );
        steer_pid.setGoalValue(
            (int)Wheel::BACK_RIGHT,
            steer.getOmega(Wheel::BACK_RIGHT)
        );

        // アクチュエータに出力をセット
        robomas.setCurrent(
            ARM_ROBOMAS_OF((int)Direction2::RIGHT),
            arm_pid.getOutput(ARM_HAND_PID_OF((int)Direction2::RIGHT))
        );
        robomas.setCurrent(
            ARM_ROBOMAS_OF((int)Direction2::LEFT),
            arm_pid.getOutput(ARM_HAND_PID_OF((int)Direction2::LEFT))
        );
        rohm.setPower(
            (int)Direction2::RIGHT,
            arm_pid.getOutput(ARM_JOINT_PID_OF((int)Direction2::RIGHT))
        );
        rohm.setPower(
            (int)Direction2::LEFT,
            arm_pid.getOutput(ARM_JOINT_PID_OF((int)Direction2::LEFT))
        );
        
        robomas.setCurrent(
            STEER_ROBOMAS_OF((int)Wheel::FRONT_RIGHT),
            steer_pid.getOutput((int)Wheel::FRONT_RIGHT)
        );
        robomas.setCurrent(
            STEER_ROBOMAS_OF((int)Wheel::FRONT_LEFT),
            steer_pid.getOutput((int)Wheel::FRONT_LEFT)
        );
        robomas.setCurrent(
            STEER_ROBOMAS_OF((int)Wheel::BACK_LEFT),
            steer_pid.getOutput((int)Wheel::BACK_LEFT)
        );
        robomas.setCurrent(
            STEER_ROBOMAS_OF((int)Wheel::BACK_RIGHT),
            steer_pid.getOutput((int)Wheel::BACK_RIGHT)
        );
        servo.setAngle(
            (int)Direction1::FRONT,
            steer.getAngle(Direction1::FRONT)
        );
        servo.setAngle(
            (int)Direction1::BACK,
            steer.getAngle(Direction1::BACK)
        );
        update();
    }
}