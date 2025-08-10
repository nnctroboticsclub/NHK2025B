#include <mbed.h>
#include "definitions.h"
#include "N_ikakoMDC.h"
#include "pinconfig.h"

DigitalIn button1(pins.SW1);
DigitalIn button2(pins.SW2);
DigitalIn button3(pins.SW3);

std::array<IkakoMdcParameter, NUM_OF_IKAKO_MD> params
{
    []{IkakoMdcParameter p;
        p.id = 1,p.is_enable=true;
    return p;}(),
    []{IkakoMdcParameter p;
        p.id = 2,p.is_enable=true;
    return p;}(),
    []{IkakoMdcParameter p;
        p.id = 3,p.is_enable=true;
    return p;}(),
    []{IkakoMdcParameter p;
        p.id = 4,p.is_enable=true;
    return p;}(),
};

NHK2025B_IkakoMDC ikako_md(params);
Thread thread;
Ticker ticker;

void send_thread()
{
    while(true)
    {
        ikako_md.write();
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
    ikako_md.setup();
    thread.start(&send_thread);
    ticker.attach(&update_ts,1ms);
    // can1.read_start();
    // can2.read_start();
    float current = 0;
    float ratio[] = {0.5, 0.5, -0.5, -0.25};
    ES = 1;
    while(1){
        if(cnt_1ms > 100){
            printf("current:");
            for(int i=0;i<4;i++) printf("%0.2f,",current * ratio[i]);
            print_debug();
            puts("");
            cnt_1ms = 0;
            current += button1 * 0.02;
            current -= button2 * 0.02;
        }

        if(button3) current = 0;
        if(current >= 1.0) current = 0.99999;
        if(current <= -1.0) current = -0.99999;

        for(int i=0;i<4;i++){
            ikako_md.setPower(i, current * ratio[i]);
        }

        ikako_md.update();
        can1.update();
        can2.update();
        ThisThread::sleep_for(1ms);
    }
}
