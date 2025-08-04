#include <mbed.h>
#include "N_PID.h"
#include "N_puropo.h"
#include "N_robomas.h"
#include "N_rohmMD.h"
#include "N_servo.h"
#include "N_steer.h"
#include "N_arm.h"

#define CH_A 5
#define CH_B 6
#define CH_C 7
#define CH_E 8
#define CH_F 9
#define CH_G 5
#define CH_H 5
#define CH_VR 5

NHK2025B_Puropo puropo;

NHK2025B_Servo servo;
NHK2025B_Robomas robomas;

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