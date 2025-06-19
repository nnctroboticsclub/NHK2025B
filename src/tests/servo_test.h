#include <mbed.h>
#include <definitions.h>
#include "N_servo.h"

std::array<ServoParameter,NUM_OF_SERVO>params{{
    []{ServoParameter p;
        p.id = 0, p.board_id = 2;
        return p;}(),
    []{
        ServoParameter p;
        p.id = 1, p.board_id = 2, p.ican = &can2;
        return p;
    }(),
    []{
        ServoParameter p;
        p.id = 2, p.board_id = 2;
        return p;
    }(),
}};
NHK2025B_Servo servo(params);
Thread thread;
Ticker ticker;

void send_thread()
{
    while(true)
    {
        servo.write();
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
    servo.setup();
    thread.start(&send_thread);
    ticker.attach(&update_1ms,1ms);
    int loop_cnt = 0;
    int wave;
    while(true)
    {
        if(cnt_1ms > 100){
            printf("loop cnt:%d,",loop_cnt);
            print_debug();
            puts("");
            cnt_1ms = 0;
        }
        wave = loop_cnt % (int)1e6;
        wave = (wave > 0.5e6)?loop_cnt % (int)0.5e6:0.5e6 - loop_cnt % (int)0.5e6;
        for(int i=0;i<NUM_OF_SERVO;i++){
            servo.setAngle(i,wave * 1e-6 * M_PI);
        }
        servo.update();
        can1.update();
        loop_cnt++;
    }
}