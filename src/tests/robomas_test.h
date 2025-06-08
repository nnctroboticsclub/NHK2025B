#include <mbed.h>
#include "definitions.h"
#include "N_robomas.h"


std::array<RobomasParameter, NUM_OF_ROBOMAS> params
{
    []{RobomasParameter p;
        p.robomas_id = 1,p.type = RobomasParameter::TYPE_OF_M3508,p.ican_ptr = &can1;
    return p;}(),
    []{RobomasParameter p;
        p.robomas_id = 2,p.type = RobomasParameter::TYPE_OF_M2006,p.ican_ptr = &can1;
    return p;}()
};

NHK2025B_Robomas robomas(params);
Thread thread;
Ticker ticker;

void send_thread()
{
    while(true)
    {
        robomas.write();
        ThisThread::sleep_for(1ms);
    }
}

int cnt_1ms = 0;
void tick_1ms()
{
    cnt_1ms++;
}

void print_debug()
{
    can1.print_debug();
    can2.print_debug();
}

int main()
{
    // puts("program start");
    robomas.setup();
    thread.start([]{
        while(true){
            robomas.write();
            ThisThread::sleep_for(1ms);
        }
    });
    ticker.attach(&tick_1ms,1ms);
    // puts("robomas setup");
    int print_cnt = 0;
    // puts("loop start");
    while(1){
        if(cnt_1ms > 100){
            printf("%f,",cnt_1ms / 100.0f);
            print_debug();
            puts("");
            print_cnt++;
            cnt_1ms = 0;
        }
        for(int i=0;i<2;i++){
            robomas.setCurrent(i,print_cnt / 100.0);
        }
        robomas.update();
    }
}