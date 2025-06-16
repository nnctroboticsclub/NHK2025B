#include <mbed.h>
#include "definitions.h"
#include "N_robomas.h"
#include "N_puropo.h"
#include "N_controller.h"
#include "N_arm.h"

std::array<RobomasParameter, NUM_OF_ROBOMAS> robomas_params{
    []{RobomasParameter p;
        p.type = RobomasParameter::TYPE_OF_M2006;
        return p;
    }()
};
NHK2025B_Robomas robomas(robomas_params);
NHK2025B_Puropo puropo;
NHK2025B_Controller controller;
NHK2025B_Arm arm;

Thread thread;
Ticker ticker;

void setup()
{
    robomas.setup();
    puropo.setup();
    controller.setup();
    arm.setup();
}

void update()
{
    robomas.update();
    puropo.update();
    controller.update();
    arm.udpate();
}

int cnt_1ms = 0;
void update_1ms()
{
    cnt_1ms++;
}

void send_thread()
{
    while(1){
        robomas.write();
        ThisThread::sleep_for(1ms);
    }
}

void print_debug()
{
    can1.print_debug();
}

int main()
{
    setup();
    can1.read_start();
    thread.start(&send_thread);
    ticker.attach(&update_1ms,1ms);
    while(1){
        if(cnt_1ms > 100){
            print_debug();
            puts("");
            cnt_1ms = 0;
        }
        controller.setArmEffort(puropo.getLeftY(0));
        arm.setEffort(0,controller.getArmEffort());
        robomas.setCurrent(0,arm.getEffort(0));
        update();
    }
}