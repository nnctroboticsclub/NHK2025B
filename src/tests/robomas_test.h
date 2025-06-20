#include <mbed.h>
#include "definitions.h"
#include "N_robomas.h"

std::array<RobomasParameter, NUM_OF_ROBOMAS> params{
    []{RobomasParameter p;
    return p;}(),
    []{RobomasParameter p;
        p.robomas_id = 5,p.type = RobomasParameter::TYPE_OF_M2006;
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
void update_ts()
{
    cnt_1ms++;
}

void print_debug()
{
    can1.print_debug();
    // can2.print_debug();
}

int main()
{
    robomas.setup();
    thread.start(&send_thread);
    ticker.attach(&update_ts,1ms);
    can1.read_start();
    // can2.read_start();
    int loop_cnt = 0;
    ES = 1;
    while(1){
        if(cnt_1ms > 100){
            printf("%.2f,",loop_cnt / 2000.0f);
            print_debug();
            puts("");
            cnt_1ms = 0;
        }
        for(int i=0;i<NUM_OF_ROBOMAS;i++){
            robomas.setCurrent(i,loop_cnt / 2000.0);
        }
        loop_cnt++;
        loop_cnt = loop_cnt%10000;
        robomas.update();
        can1.update();
        // can2.update();
    }
}