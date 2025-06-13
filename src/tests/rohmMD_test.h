#include <mbed.h>
#include "definitions.h"
#include "N_rohmMD.h"

std::array<RohmMdParameter, NUM_OF_ROHM_MD> params
{
    []{RohmMdParameter p;
        p.id = 1;
    return p;}(),
    []{RohmMdParameter p;
        p.id = 2;
    return p;}(),
    []{RohmMdParameter p;
        p.id = 3,p.ican = &can2;
    return p;}(),
    []{RohmMdParameter p;
        p.id = 4;
    return p;}(),
};

NHK2025B_RohmMD rohm(params);
Thread thread;
Ticker ticker;

void send_thread()
{
    while(true)
    {
        rohm.write();
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
    can2.print_debug();
}

int main()
{
    rohm.setup();
    thread.start(&send_thread);
    ticker.attach(&update_ts,1ms);
    // can1.read_start();
    // can2.read_start();
    int loop_cnt = 0;
    while(1){
        if(cnt_1ms > 100){
            printf("%.2f,",loop_cnt / 1000.0);
            print_debug();
            puts("");
            cnt_1ms = 0;
        }
        for(int i=0;i<4;i++){
            rohm.setPower(i,loop_cnt / 1000.0);
        }
        loop_cnt++;
        loop_cnt = loop_cnt%1000;
        rohm.update();
        can1.update();
        can2.update();
    }
}