#include <mbed.h>
#include "definitions.h"
#include "N_rohmMD.h"
#define PI 3.141592653589793

std::array<RohmMdParameter, NUM_OF_ROHM_MD> params
{
    []{RohmMdParameter p;
        p.id = 1;
    return p;}(),
    []{RohmMdParameter p;
        p.id = 2;
    return p;}(),
    []{RohmMdParameter p;
        p.id = 3;
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
    double loop_cnt = 0;
    while(1){
        if(cnt_1ms > 100){
            printf("%.2f,",(sin(loop_cnt) * 2 * PI) / 10);
            print_debug(); 
            printf("%d", loop_cnt);
            puts("");
            cnt_1ms = 0;
        }
        for(int i=0;i<4;i++){
            rohm.setPower(i,(sin(loop_cnt) * 2 * PI) / 10);
        }
        loop_cnt = loop_cnt + 1e-4;
        rohm.update();
        can1.update();
        can2.update();
    }
}