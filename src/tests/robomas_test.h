#include <mbed.h>
#include "definitions.h"
#include "N_robomas.h"

DigitalIn button(pins.SW1);

std::array<RobomasParameter, NUM_OF_ROBOMAS> params{
    []
    {RobomasParameter p;
        p.robomas_id = 3;
    return p; }(),
    []
    {RobomasParameter p;
        p.robomas_id = 5,p.type = RobomasParameter::TYPE_OF_M2006,p.ican_ptr = &can2;
    return p; }()};

NHK2025B_Robomas robomas(params);
Thread thread;
Ticker ticker;

void send_thread()
{
    while (true)
    {
        robomas.write();
        ThisThread::sleep_for(1ms);
    }
}

int cnt_1ms = 0;
void update_ts()
{
    robomas.update_ts();
    cnt_1ms++;
}

void print_debug()
{
    can2.print_debug();
    // robomas.print_debug();
}

int main()
{
    robomas.setup();
    thread.start(&send_thread);
    ticker.attach(&update_ts, 1ms);
    can1.read_start();
    can2.read_start();
    DigitalOut myemc(PA_5);
    DigitalIn mybo(BUTTON1);
    float goal = 22;
    float sensor;
    float error;
    float kp = 0.03;
    float power;
    int cnt_100ms = 0;
    while (1)
    {
        ES = 1;
        myemc = 1;
        if(cnt_1ms > 100){
            //print_debug();
            //printf("%f",robomas.getAbsAngle(0));
            printf("%lf,   %lf,   %lf\n",sensor,goal,error);
            puts("");
            cnt_1ms = 0;
            cnt_100ms++;
        }
        // if(button){
        //    robomas.resetState(0);
        // }
        // for(int i=0;i<NUM_OF_ROBOMAS;i++){
        //     robomas.setCurrent(i,cnt_100ms / 20.0);
        // }
        cnt_100ms %= 20;
        robomas.update();
        can1.update();
        can2.update();
        robomas.getAbsAngle(0);
        sensor = robomas.getAbsAngle(0);
        error = goal - sensor;
        if(error<=22 && error>=0){  //22がちょうどいいかも
            if(mybo==1){
              power=error*kp;  
            }
            else if(mybo==0){
                power=error*-kp;
            }
        }
        else{
            power = 0;
        }
        robomas.setCurrent(0,power);
        ThisThread::sleep_for(1ms);
    }
}