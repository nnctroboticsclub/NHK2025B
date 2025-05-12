#include <mbed.h>
#include "N_controller.h"
#include "N_PID.h"
#include "N_puropo.h"
#include "N_robomas.h"
#include "N_servo.h"
#include "N_steer.h"

Ticker ticker;
Thread thread;

void update_1ms(){
    ;
}

void send_thread(){
    while(1){
        ThisThread::sleep_for(1ms);
    }
}

void setup(){
    ticker.attach(&update_1ms,1ms);
    thread.start(&send_thread);
}

void update(){
    ;
}

int main(){
    setup();
    while(1){
        update();
    }
}