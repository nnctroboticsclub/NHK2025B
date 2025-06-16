#include "US_Sensor.h"

US_Sensor::US_Sensor(PinName trig, PinName echo) : trig_pin(trig), echo_pin(echo) {
    
}


void US_Sensor::shot(){
    trig_pin.write(1);
    wait_us(10);
    trig_pin.write(0);
}

void US_Sensor::falling_callback(){
    timer.stop();
    distance_ = timer.read();
    falling_frag = 0;
}

void US_Sensor::rising_callback(){
    timer.reset();
    timer.start();
    falling_frag = 1;
}

void US_Sensor::setup(){
    echo_pin.fall(callback(this, &US_Sensor::falling_callback));
    echo_pin.rise(callback(this, &US_Sensor::rising_callback));
    US_Sensor::shot();
    getDistance(); 
}