#include <mbed.h>
#include "N_ikakoMDC.h"

std::array<IkakoMdcParameter, NUM_OF_IKAKO_MD> params{{
    []{IkakoMdcParameter p; p.id=1,p.is_enable=true; return p;}(),
    []{IkakoMdcParameter p; p.id=2,p.is_enable=true; return p;}(),
    []{IkakoMdcParameter p; p.id=3,p.is_enable=true; return p;}(),
    []{IkakoMdcParameter p; p.id=4,p.is_enable=true; return p;}(),
}};
NHK2025B_IkakoMDC ikako_mdc(params);
Thread thread;
Ticker ticker;

void send_thread()
{
    while(true){
        ikako_mdc.write();
        ThisThread::sleep_for(1ms);
    }
}

int cnt_1ms = 0;
void update_1ms()
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
    ikako_mdc.setup();
    thread.start(send_thread);
    ticker.attach(update_1ms,1ms);
    int cnt_100ms=0;
    float current = 0.5;
    while(true){
        if(cnt_1ms>100){
            printf("current:%f",current);
            print_debug();
            puts("");
            cnt_1ms = 0;
            cnt_100ms++;
        }
        current = (cnt_100ms%20<10)?-0.5+cnt_100ms%10/10.0:(0.5-cnt_100ms%10/10.0);
        for(int i=0;i<NUM_OF_IKAKO_MD;i++){
            ikako_mdc.setPower(i,current);
        }
        ikako_mdc.update();
    }
}