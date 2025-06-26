#include <mbed.h>
#include "definitions.h"
#include "N_robomas.h"
#include "N_puropo.h"
#include "N_controller.h"
#include "N_arm.h"

DigitalIn button1(pins.SW1);
DigitalIn button2(pins.SW2);
DigitalIn button3(pins.SW3);

std::array<RobomasParameter, NUM_OF_ROBOMAS> robomas_params{
    []{RobomasParameter p;
        p.robomas_id = 2, p.type = RobomasParameter::TYPE_OF_M2006;
        return p;
    }()
};
NHK2025B_Robomas robomas(robomas_params);
// NHK2025B_Puropo puropo;
// NHK2025B_Controller controller;
// NHK2025B_Arm arm;

Thread thread;
Ticker ticker;

void setup()
{
    robomas.setup();
    // puropo.setup();
    // controller.setup();
    // arm.setup();
    can1.read_start();
    can2.read_start();
}

void update()
{
    robomas.update();
    // puropo.update();
    // controller.update();
    // arm.udpate();
    can1.update();
    can2.update();
}

int cnt_1ms = 0;
void update_1ms()
{
    robomas.update_ts();
    // puropo.update_ts();
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
    robomas.print_debug();
    // puropo.print_debug();
    can2.print_debug();
}

int main()
{
    setup();
    can2.read_start();
    thread.start(&send_thread);
    ticker.attach(&update_1ms,1ms);
    ES=1;
    float current = 0;
    while(1){
        if(cnt_1ms > 100){
            printf("current:%f,",current);
            print_debug();
            puts("");
            cnt_1ms = 0;
            current += button1 * 0.1;
            current -= button2 * 0.1;
        }
        // controller.setArmEffort(puropo.getLeftY(0));
        // arm.setEffort(0,controller.getArmEffort());
        // 動かなかったら69, 70,73行目コメントアウトして,下行のコメントアウトを解除してください
        // robomas.setCurrent(0,puropo.getLeftY(0) * 5.0);
        // robomas.setCurrent(0,arm.getEffort(0));
        // ES = puropo.getCommunicatable(0);
        if(button3) current = 0;
        if(current > 5.0) current = 5.0;
        if(current < -5.0) current = -5.0;
        robomas.setCurrent(0,current);
        update();
        // ThisThread::sleep_for();
    }
}